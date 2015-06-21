/*
    Copyright 2008-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb.hpp"
#include <fost/db>
#include <fost/log>
#include <fost/unicode>
#include <fost/jsondb.hpp>

#include <fost/exception/not_null.hpp>
#include <fost/exception/out_of_range.hpp>
#include <fost/exception/transaction_fault.hpp>
#include <fost/exception/unexpected_eof.hpp>


using namespace fostlib;
namespace bfs = boost::filesystem;


/*
    fostlib::jsondb
*/


#ifdef DEBUG
const bool c_jsondb_pretty_print_default = true;
#else
const bool c_jsondb_pretty_print_default = false;
#endif
const setting<bool> fostlib::c_jsondb_pretty_print(
    L"fost-orm/Cpp/fost-jsondb/blobdb.cpp",
    L"JSON DB", "Pretty print database files",
    c_jsondb_pretty_print_default, true);


const setting<string> fostlib::c_jsondb_root(
    L"fost-orm/Cpp/fost-jsondb/blobdb.cpp",
    L"JSON DB", "Default file location",
    "", true);


namespace {
    const bfs::wpath ext_backup(".backup");
    const bfs::wpath ext_temp(".tmp");


    void do_save( const json &j, const bfs::wpath &path ) {
        if ( bfs::exists(path) ) {
#ifndef ANDROID
            bfs::wpath backup(path);
            backup.replace_extension(ext_backup);
            if ( bfs::exists(backup) )
                bfs::remove(backup);
            bfs::create_hard_link(path, backup);
#endif
        } else if ( !path.parent_path().empty() ) {
            bfs::create_directories(path.parent_path());
        }
        bfs::wpath tmp(path);
        tmp.replace_extension(ext_temp);
        utf::save_file(tmp, json::unparse(j, c_jsondb_pretty_print.value()));
        bfs::rename(tmp, path);
    }
    json *construct( const bfs::wpath &filename, const nullable< json > &default_db ) {
        string content(utf::load_file(filename, string()));
        try {
            if ( content.empty() ) {
                do_save(default_db.value(), filename);
                return new json(default_db.value());
            } else {
                return new json(json::parse(content));
            }
        } catch ( exceptions::exception &e ) {
            insert(e.data(), "blobdb", "filename", filename);
            insert(e.data(), "blobdb", "file-content", content);
            insert(e.data(), "blobdb", "initial-data", default_db);
            throw;
        }
    }
}


fostlib::jsondb::jsondb()
: m_blob( new json ) {
}

fostlib::jsondb::jsondb( const string &fn, const nullable< json > &default_db )
: filename(get_db_path(coerce<boost::filesystem::wpath>(fn))),
        m_blob(boost::lambda::bind(construct, filename().value(), default_db)) {
}

fostlib::jsondb::jsondb( const bfs::wpath &fn, const nullable< json > &default_db )
: filename(get_db_path(fn)),
        m_blob(boost::lambda::bind(construct, filename().value(), default_db)) {
}


bfs::wpath fostlib::jsondb::get_db_path(const bfs::wpath &filename) {
    return join_paths(
        coerce<bfs::wpath>(c_jsondb_root.value()), filename);
}


std::size_t fostlib::jsondb::post_commit(
    const_operation_signature_type fn
) {
    m_post_commit.push_back(fn);
    return m_post_commit.size();
}


/*
    fostlib::jsondb::local
*/


namespace {
    json dump( json &j, const jcursor &p ) {
        return json( j[ p ] );
    }

    void do_insert( json &db, const jcursor &k, const json &v ) {
        k.insert( db, v );
    }
    void do_push_back( json &db, const jcursor &k, const json &v ) {
        k.push_back( db, v );
    }
    void do_update( json &db, const jcursor &k, const json &v, const json &old ) {
        if ( db.has_key( k ) && db[ k ] == old )
            k( db ) = v;
        else if ( db.has_key( k ) && db[ k ] != old )
            throw exceptions::transaction_fault( L"The value being updated is not the value that was meant to be updated" );
        else
            throw exceptions::null( L"This key position is empty so cannot be updated" );
    }
    void do_set( json &db, const jcursor &k, const json &v ) {
        if ( db.has_key(k) )
            k( db ) = v;
        else
            k.insert( db, v );
    }
    void do_remove( json &db, const jcursor &k, const json &old ) {
        if ( db.has_key( k ) && db[ k ] == old )
            k.del_key( db );
        else if ( db.has_key( k ) && db[ k ] != old )
            throw exceptions::transaction_fault( L"The value being deleted is not the value that was meant to be deleted" );
        else
            throw exceptions::null( L"This key position has already been deleted" );
    }

    json &do_commit( json &j, const jsondb::operations_type &ops ) {
        json db( j );
        try {
            for ( jsondb::operations_type::const_iterator op( ops.begin() ); op != ops.end(); ++op )
                (*op)( j );
        } catch ( ... ) {
            j = db;
            throw;
        }
        return j;
    }
}


fostlib::jsondb::local::local( jsondb &db, const jcursor &pos )
: m_db( db ), m_position( pos ) {
    refresh();
}


fostlib::jsondb::local::local(local &&l)
: m_db(l.m_db), m_local(std::move(l.m_local)),
        m_operations(std::move(l.m_operations)),
        m_pre_commit(std::move(l.m_pre_commit)),
        m_post_commit(std::move(l.m_post_commit)) {
}


void fostlib::jsondb::local::refresh() {
    m_local = m_db.m_blob.synchronous< json >(
        boost::lambda::bind(dump, boost::lambda::_1, m_position));
}


std::size_t fostlib::jsondb::local::pre_commit(
    operation_signature_type fn
) {
    m_pre_commit.push_back(fn);
    return m_pre_commit.size();
}

std::size_t fostlib::jsondb::local::post_commit(
    const_operation_signature_type fn
) {
    m_post_commit.push_back(fn);
    return m_post_commit.size();
}


std::size_t fostlib::jsondb::local::transformation(
    operation_signature_type fn
) {
    m_operations.push_back(fn);
    return m_operations.size();
}


void fostlib::jsondb::local::commit() {
    /// Add the pre-commit hooks to the end of the transformations
    for ( auto fn : m_pre_commit )
        transformation(fn);
    m_pre_commit.clear();
    // Add save to the end of the transformation
    if ( !m_db.filename().isnull() ) {
        transformation(
            [this](json &j) {
                do_save(j, m_db.filename().value());
            });
    }
    try {
        // Run the transformations and commit
        m_local = m_db.m_blob.synchronous< json >(
            boost::lambda::bind(
                do_commit, boost::lambda::_1, m_operations));
        // Run transaction post-commit hooks
        for ( auto fn : m_post_commit )
             fn(m_local);
        m_post_commit.clear();
        // Run database post-commit hooks
        for ( auto fn : m_db.m_post_commit )
            fn(m_local);
    } catch ( ... ) {
        rollback();
        throw;
    }
    m_operations.clear();
}


void fostlib::jsondb::local::rollback() {
    m_pre_commit.clear();
    m_post_commit.clear();
    m_operations.clear();
    refresh();
}


jsondb::local &fostlib::jsondb::local::insert( const jcursor &position, const json &item ) {
    do_insert( m_local, position, item );
    m_operations.push_back( boost::lambda::bind( do_insert, boost::lambda::_1, position, item ) );
    return *this;
}


jsondb::local &fostlib::jsondb::local::push_back(
    const jcursor &position, const json &item
) {
    do_push_back( m_local, position, item );
    m_operations.push_back(boost::lambda::bind(
        do_push_back, boost::lambda::_1, position, item));
    return *this;
}


jsondb::local &fostlib::jsondb::local::update( const jcursor &position, const json &item ) {
    json oldvalue = m_local[ position ];
    position.replace( m_local, item );
    transformation(
        boost::lambda::bind( do_update, boost::lambda::_1, position, item, oldvalue ));
    return *this;
}


jsondb::local &fostlib::jsondb::local::set( const jcursor &position, const json &item ) {
    transformation( boost::lambda::bind( do_set,
        boost::lambda::_1, position, item ) );
    return *this;
}


jsondb::local &fostlib::jsondb::local::remove( const jcursor &position ) {
    json oldvalue = m_local[ position ];
    position.del_key( m_local );
    transformation(
        boost::lambda::bind( do_remove, boost::lambda::_1, position, oldvalue ) );
    return *this;
}


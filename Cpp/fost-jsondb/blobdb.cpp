/*
    Copyright 2008-2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb.hpp"
#include <fost/counter>
#include <fost/db>
#include <fost/db-driver>
#include <fost/log>
#include <fost/unicode>
#include <fost/jsondb.hpp>
#include <fost/schema.hpp>

#include <fost/exception/not_null.hpp>
#include <fost/exception/out_of_range.hpp>
#include <fost/exception/transaction_fault.hpp>
#include <fost/exception/unexpected_eof.hpp>

#include <future>


using namespace fostlib;
namespace bfs = boost::filesystem;


/*
    fostlib::jsondb
*/


const module fostlib::c_fost_orm_jsondb(c_fost_orm, "jsondb");


namespace {
    performance p_created(c_fost_orm_jsondb, "db", "created");
    performance p_loaded(c_fost_orm_jsondb, "db", "loaded");
}


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
    using lock_type = std::unique_lock<std::mutex>;

    const bfs::path ext_backup(".backup");
    const bfs::path ext_temp(".tmp");


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
}


fostlib::jsondb::jsondb(const bfs::wpath &fn, const nullable< json > &default_db)
: filename(get_db_path(fn)) {
    /// We can safely access the JSON directly here because no other operation
    /// is possilbe until the constructor returnes
    string content(utf::load_file(filename().value(), string()));
    try {
        if ( content.empty() ) {
            if ( not default_db ) {
                throw exceptions::null("Initial database data must be provided "
                    "when database is backed to the file system and the file is empty");
            } else {
                do_save(default_db.value(), filename().value());
                data = default_db.value();
                ++p_created;
            }
        } else {
            data = json::parse(content);
            ++p_loaded;
        }
    } catch ( exceptions::exception &e ) {
        insert(e.data(), "blobdb", "filename", filename().value());
        insert(e.data(), "blobdb", "file-content", content);
        insert(e.data(), "blobdb", "initial-data", default_db);
        throw;
    }
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
}


fostlib::jsondb::local::local( jsondb &db, const jcursor &pos )
: m_db( db ), m_position( pos ) {
    refresh();
}


fostlib::jsondb::local::local(local &&l)
: m_db(l.m_db),
    m_local(std::move(l.m_local)),
    m_position(std::move(l.m_position)),
    m_operations(std::move(l.m_operations)),
    m_pre_commit(std::move(l.m_pre_commit)),
    m_post_commit(std::move(l.m_post_commit))
{
}


void fostlib::jsondb::local::rebase(const jcursor &pos) {
    if ( m_operations.size() || m_pre_commit.size() || m_post_commit.size() ) {
        throw fostlib::exceptions::not_implemented(
            __FUNCTION__, "This transaction has already been used");
    }
    m_position = pos;
    refresh();
}


void fostlib::jsondb::local::refresh() {
    lock_type lock(m_db.control);
    m_local = m_db.data[m_position];
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
    /// Add save to the end of the transformation. Access to the
    /// filename is safe because it's a const member of m_db.
    if ( m_db.filename() ) {
        transformation(
            [this](const jcursor &, json &j) {
                do_save(j, m_db.filename().value());
            });
    }
    try {
        /// All of the following runs inside the lock because we can't
        /// access anything on m_db without holding it. This forces
        /// serialisation of all post commit code as well, which should
        /// make that easier to reason about
        lock_type lock(m_db.control);
        json db(m_db.data);
        try {
            for ( auto op : m_operations )
                (op)(m_position, m_db.data);
            m_operations.clear();
        } catch ( ... ) {
            m_db.data = db;
            throw;
        }
        m_local = m_db.data[m_position];
        /// Run transaction post-commit hooks
        for ( auto fn : m_post_commit )
                fn(jcursor(), m_local);
        m_post_commit.clear();
        /// Run database post-commit hooks
        for ( auto fn : m_db.m_post_commit )
            fn(m_position, m_db.data);
    } catch ( ... ) {
        rollback();
        throw;
    }
}


void fostlib::jsondb::local::rollback() {
    m_pre_commit.clear();
    m_post_commit.clear();
    m_operations.clear();
    refresh();
}


jsondb::local &fostlib::jsondb::local::insert(const jcursor &position, const json &item) {
    do_insert(m_local, position, item);
    transformation([position, item](const auto &jc, auto &js) {
            do_insert(js, jc / position, item);
        });
    return *this;
}


jsondb::local &fostlib::jsondb::local::push_back(
    const jcursor &position, const json &item
) {
    do_push_back( m_local, position, item );
    transformation([position, item](const auto &jc, auto &js) {
            do_push_back(js, jc / position, item);
        });
    return *this;
}


jsondb::local &fostlib::jsondb::local::update(const jcursor &position, const json &item) {
    auto old = m_local[position];
    position.replace(m_local, item);
    transformation(
        [position, item, old = std::move(old)](const auto &jc, auto &js) {
            do_update(js, jc / position, item, old);
        });
    return *this;
}


jsondb::local &fostlib::jsondb::local::set(const jcursor &position, const json &item) {
    do_set(m_local, position, item);
    transformation(
        [position, item](const auto &jc, auto &js) {
            do_set(js, jc / position, item);
        });
    return *this;
}


jsondb::local &fostlib::jsondb::local::remove(const jcursor &position) {
    auto old = m_local[position];
    position.del_key(m_local);
    transformation(
        [position, old = std::move(old)](const auto &jc, auto &js) {
            do_remove(js, jc / position, old);
        });
    return *this;
}


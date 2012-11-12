/*
    Copyright 2008-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb.hpp"
#include <fost/db>
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


namespace {
    void do_save( const json &j, const bfs::wpath &path ) {
        if ( bfs::exists(path) ) {
            bfs::wpath backup(path);
            backup.replace_extension(L".backup");
            if ( bfs::exists(backup) )
                bfs::remove(backup);
            bfs::create_hard_link(path, backup);
        }
        bfs::wpath tmp(path);
        tmp.replace_extension(L".tmp");
        utf::save_file(tmp, json::unparse(j, false));
#if ( BOOST_VERSION_MAJOR < 46 )
        if ( bfs::exists(path) )
            bfs::remove(path);
#endif
        bfs::rename(tmp, path);
    }
    json *construct( const bfs::wpath &filename, const nullable< json > &default_db ) {
        string content(utf::load_file(filename, string()));
        if ( content.empty() ) {
            do_save( default_db.value(), filename );
            return new json(default_db.value());
        } else
            return new json(json::parse(content));
    }
}


fostlib::jsondb::jsondb()
: m_blob( new json ) {
}

fostlib::jsondb::jsondb( const string &filename, const nullable< json > &default_db )
: m_blob(boost::lambda::bind(
            construct, coerce<boost::filesystem::wpath>(filename), default_db)),
        filename(coerce<boost::filesystem::wpath>(filename)) {
}

fostlib::jsondb::jsondb( const bfs::wpath &filename, const nullable< json > &default_db )
: m_blob(boost::lambda::bind(construct, filename, default_db)), filename(filename) {
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

void fostlib::jsondb::local::refresh() {
    m_local = m_db.m_blob.synchronous< json >( boost::lambda::bind( dump, boost::lambda::_1, m_position ) );
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
    m_operations.push_back( boost::lambda::bind( do_update, boost::lambda::_1, position, item, oldvalue ) );
    return *this;
}

jsondb::local &fostlib::jsondb::local::set( const jcursor &position, const json &item ) {
    m_operations.push_back( boost::lambda::bind( do_set,
        boost::lambda::_1, position, item ) );
    return *this;
}

jsondb::local &fostlib::jsondb::local::remove( const jcursor &position ) {
    json oldvalue = m_local[ position ];
    position.del_key( m_local );
    m_operations.push_back( boost::lambda::bind( do_remove, boost::lambda::_1, position, oldvalue ) );
    return *this;
}

std::size_t fostlib::jsondb::local::post_commit(
    const_operation_signature_type fn
) {
    m_post_commit.push_back(fn);
    return m_post_commit.size();
}

void fostlib::jsondb::local::commit() {
    if ( !m_db.filename().isnull() )
        m_operations.push_back( boost::lambda::bind( do_save, boost::lambda::_1, m_db.filename().value() ) );
    try {
        m_local = m_db.m_blob.synchronous< json >(
            boost::lambda::bind(
                do_commit, boost::lambda::_1, m_operations));
        for ( const_operations_type::iterator f_it(m_post_commit.begin());
                f_it != m_post_commit.end(); ++f_it )
             (*f_it)(m_local);
        m_post_commit.clear();
    } catch ( ... ) {
        rollback();
        throw;
    }
    m_operations.clear();
}

void fostlib::jsondb::local::rollback() {
    m_operations.clear();
    refresh();
}

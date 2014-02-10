/*
    Copyright 1998-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-cache.hpp"
#include <fost/cache.hpp>

#include <fost/exception/not_implemented.hpp>
#include <fost/exception/not_null.hpp>
#include <fost/exception/null.hpp>


using namespace fostlib;


namespace {
    void do_nothing(fostcache*) {}
}
boost::thread_specific_ptr< fostcache > fostlib::fostcache::s_instance( do_nothing );


fostlib::detail::fostcache_dbc::fostcache_dbc() {
}
fostlib::detail::fostcache_dbc::fostcache_dbc( dbconnection &dbc )
: m_dbc_ptr( new dbconnection( dbc.configuration() ) ) {
}


namespace {
    void initialise( boost::thread_specific_ptr< fostcache > &instance, fostcache &cache ) {
        if ( instance.get() )
            throw exceptions::not_null( L"There is already a fostcache in this thread" );
        else
            instance.reset( &cache );
    }
}
fostlib::fostcache::fostcache( mastercache &master )
: fostcache_dbc( master.connection() ), mastercache( *m_dbc_ptr ), m_master( &master ) {
    initialise( s_instance, *this );
}
fostlib::fostcache::fostcache( dbconnection &dbc )
: mastercache( dbc ), m_master( NULL ) {
    initialise( s_instance, *this );
}

fostlib::fostcache::~fostcache() {
    if ( s_instance.get() == this )
        s_instance.reset();
}

bool fostlib::fostcache::exists() {
    return s_instance.get();
}
fostcache &fostlib::fostcache::instance() {
    if ( !s_instance.get() )
        throw exceptions::null( L"There is no fostcache in this thread" );
    return *s_instance;
}


const fostlib::meta_instance &fostlib::fostcache::operator [] ( const string &n ) const {
    type_registry_collection::const_iterator p( m_types.find( n ) );
    if ( p == m_types.end() && m_master )
        return (*m_master)[ n ];
    else if ( p != m_types.end() )
        return *p->second;
    else
        throw exceptions::null( L"Type not found in the cache type registry", n );
}


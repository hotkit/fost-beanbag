/*
    Copyright 1998-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-cache.hpp"
#include <fost/cache.hpp>

#include <fost/exception/null.hpp>


using namespace fostlib;


fostlib::mastercache::mastercache( dbconnection &dbc )
: m_dbc( dbc ) {
}

fostlib::mastercache::~mastercache() {
}


mastercache &fostlib::mastercache::type( boost::shared_ptr< meta_instance > type ) {
    if ( !type.get() )
        throw fostlib::exceptions::null( L"Cannot add a null fostlib::meta_instance pointer to a cache's type list" );
    if ( m_caches.find( type ) == m_caches.end() ) {
        m_caches[ type ] = boost::shared_ptr< fostlib::objectcache< instance > >(
            new fostlib::objectcache< instance >
        );
        m_types[ type->fq_name() ] = type;
    }
    return *this;
}

const fostlib::meta_instance &fostlib::mastercache::operator [] ( const string &n ) const {
    type_registry_collection::const_iterator p( m_types.find( n ) );
    if ( p != m_types.end() )
        return *p->second;
    else
        throw exceptions::null( L"Type not found in the cache type registry", n );
}

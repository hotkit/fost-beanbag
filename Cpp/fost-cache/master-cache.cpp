/*
    Copyright 1998-2009, Felspar Co Ltd. http://fost.3.felspar.com/
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


fostlib::mastercache::mastercache( dbconnection &dbc )
: m_dbc( dbc ) {
}

fostlib::mastercache::~mastercache() {
}


mastercache &fostlib::mastercache::type( boost::shared_ptr< fostlib::meta_instance > type ) {
    if ( m_caches.find( type ) == m_caches.end() )
        m_caches[ type ] = boost::shared_ptr< fostlib::objectcache< meta_instance > >(
            new fostlib::objectcache< meta_instance >
        );
    return *this;
}

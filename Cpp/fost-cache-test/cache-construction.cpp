/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-cache-test.hpp"
#include <fost/cache>
#include <fost/exception/not_null.hpp>
#include <fost/exception/null.hpp>


FSL_TEST_SUITE( construction );


FSL_TEST_FUNCTION( objectcache ) {
    fostlib::objectcache< fostlib::instance > cache;
}


FSL_TEST_FUNCTION( fostcache ) {
    // Asking for the instance before we create one throws a null exception
    FSL_CHECK_EXCEPTION( fostlib::fostcache::instance(), fostlib::exceptions::null& );
    FSL_CHECK_EQ( fostlib::fostcache::exists(), false );

    // Now make a cache
    fostlib::dbconnection dbc( L"master" );
    fostlib::fostcache cache( dbc );
    FSL_CHECK_EQ( fostlib::fostcache::exists(), true );

    // Creating a second one throws a not null exception
    FSL_CHECK_EXCEPTION( fostlib::fostcache cache2( dbc ), fostlib::exceptions::not_null& );
}


FSL_TEST_FUNCTION( mastercache ) {
    fostlib::dbconnection dbc( L"master" );
    fostlib::mastercache master( dbc );
    fostlib::fostcache cache( master );

    FSL_CHECK_NEQ( &master.connection(), &cache.connection() );
    FSL_CHECK_EQ( master.connection().configuration(), cache.connection().configuration() );
    FSL_CHECK_EQ( cache.connection().configuration()[ L"read" ], fostlib::json( L"master" ) );
}

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


FSL_TEST_SUITE( type_registration );


FSL_TEST_FUNCTION( fostcache ) {
    fostlib::dbconnection dbc( L"master" );
    fostlib::fostcache cache( dbc );
    // We can now register types either through the stack reference or via the instance()
    // Registering a type is idempotent
    boost::shared_ptr< fostlib::meta_instance > type( new fostlib::meta_instance( L"simple" ) );
    cache.type( type );
    fostlib::fostcache::instance().type( type );

    FSL_CHECK_EQ( &cache[ L"simple" ], type.get() );
}


FSL_TEST_FUNCTION( master ) {
    fostlib::dbconnection dbc( L"master" );
    fostlib::mastercache master( dbc );

    boost::shared_ptr< fostlib::meta_instance > type( new fostlib::meta_instance( L"simple" ) );
    master.type( type );

    FSL_CHECK_EQ( &master[ L"simple" ], type.get() );
}


FSL_TEST_FUNCTION( master_slave ) {
    fostlib::dbconnection dbc( L"master" );
    fostlib::mastercache master( dbc );
    fostlib::fostcache cache( master );

    boost::shared_ptr< fostlib::meta_instance >
        master_type( new fostlib::meta_instance( L"master_type" ) ),
        slave_type( new fostlib::meta_instance( L"slave_type" ) )
    ;
    master.type( master_type );
    cache.type( slave_type );

    FSL_CHECK_EQ( &master[ L"master_type" ], master_type.get() );
    FSL_CHECK_EQ( &cache[ L"master_type" ], master_type.get() );

    FSL_CHECK_EXCEPTION( master[ L"slave_type" ], fostlib::exceptions::null& );
    FSL_CHECK_EQ( &cache[ L"slave_type" ], slave_type.get() );
}

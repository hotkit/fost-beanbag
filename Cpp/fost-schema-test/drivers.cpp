/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/db>


using namespace fostlib;


FSL_TEST_SUITE( drivers );


FSL_TEST_FUNCTION( dynlib ) {
    FSL_CHECK_EXCEPTION( dbconnection( L"nodriver/", L"nodriver/" ), exceptions::data_driver& );
    FSL_CHECK_NOTHROW( dbconnection( L"json/", L"json/" ) );
}

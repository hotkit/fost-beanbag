/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-sql.hpp"
#include <fost/db-driver-sql.hpp>


using namespace fostlib;


/*
    fostlib::sql_driver
*/


fostlib::sql_driver::sql_driver( const string &name )
: dbinterface( name ) {
}


/*
    fostlib::sql_driver::write
*/


fostlib::sql_driver::write::write( fostlib::dbinterface::read &reader )
: dbinterface::write( reader ) {
}

const sql_driver &fostlib::sql_driver::write::driver() const {
    return dynamic_cast< const sql_driver & >( m_connection.driver() );
}

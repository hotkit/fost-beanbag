/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/detail/db-driver.hpp>


using namespace fostlib;


void fostlib::sql::sql_statement_tag::do_encode( fostlib::wliteral from, fostlib::string &into ) {
    into = from;
}
void fostlib::sql::sql_statement_tag::do_encode( const fostlib::string &from, fostlib::string &into ) {
    into = from;
}
void fostlib::sql::sql_statement_tag::check_encoded( const fostlib::string & ) {
    /*
        We don't have any implementation here as there is nothing meaningful that we can check
    */
}

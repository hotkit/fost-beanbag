/*
    Copyright 2009-2013,Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-sql.hpp"
#include <fost/db-driver-sql.hpp>


using namespace fostlib;


sql::statement fostlib::sql_driver::mangle( const int64_t field_value ) const {
    return sql::statement( coerce< string >( field_value ) );
}


sql::statement fostlib::sql_driver::mangle( const string &field_value ) const {
    sql::statement quote( L"'" );
    return quote + sql::statement( replace_all( field_value, L"'", L"''" ) ) + quote;
}


fostlib::sql::table_name fostlib::sql_driver::table_name( const meta_instance &mi ) const {
    return sql::table_name( mi.fq_name( L"_" ) );
}

fostlib::sql::column_name fostlib::sql_driver::column_name( const meta_attribute &ma ) const {
    return sql::column_name( ma.name() );
}

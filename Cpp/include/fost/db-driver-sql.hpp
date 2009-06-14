/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_DB_DRIVER_SQL_HPP
#define FOST_DB_DRIVER_SQL_HPP
#pragma once


#include <fost/db-driver>


namespace fostlib {


    class FOST_SQL_DECLSPEC sql_driver : public dbinterface {
    protected:
        explicit sql_driver( const string &driver_name );
    };


}


#endif // FOST_DB_DRIVER_SQL_HPP

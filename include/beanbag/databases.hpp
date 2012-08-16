/*
    Copyright 2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once
#include <fost/jsondb>


namespace beanbag {


    /// Shorter name for the jsondb pointer type we use
    typedef boost::shared_ptr< fostlib::jsondb > jsondb_ptr;

    /// Return a reference to the requested database
    jsondb_ptr database(const fostlib::json &which);

    /// Add an alias to the database
    void alias(const fostlib::string &name, jsondb_ptr db);

    /// Insert a specified JSON blog into the databases with the specified name. Useful for testing.
    void test_database(const fostlib::string &name, const fostlib::json &blob);


}


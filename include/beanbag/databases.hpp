/*
    Copyright 2012-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once
#ifndef BEANBAG_DATABASE_HPP
#define BEANBAG_DATABASE_HPP


#include <beanbag/jsondb_ptr.hpp>


namespace beanbag {


    /// The beanbag module
    extern const fostlib::module c_beanbag;


    /// Return a reference to the requested database
    FOST_BEANBAG_DECLSPEC
    jsondb_ptr database(const fostlib::json &which);

    /// Remove a beanbag and delete the file from backing store
    FOST_BEANBAG_DECLSPEC
    void remove(jsondb_ptr db);

    /// Insert a specified JSON blob into the databases with the specified name.
    /// Useful for testing.
    FOST_BEANBAG_DECLSPEC
    jsondb_ptr test_database(
            const fostlib::string &name, const fostlib::json &blob);


}


#endif // BEANBAG_DATABASE_HPP

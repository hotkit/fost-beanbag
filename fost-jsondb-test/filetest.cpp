/*
    Copyright 2005-2014, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb-test.hpp"
#include <fost/string>
#include <fost/main>
#include <fost/test>
#include <fost/jsondb>
#include <fost/unicode>

#include <boost/filesystem.hpp>


using namespace fostlib;


namespace {
    const fostlib::setting<fostlib::string> c_data_root(
            "fost-jsondb-test-file",
            fostlib::c_jsondb_root,
            fostlib::coerce<fostlib::string>(fostlib::unique_filename()));
}


FSL_MAIN(
        L"fost-jsondb-test-file",
        L"fost-jsondb-test-file\n"
        L"Test the file handling for Fost 4 JSON blobs\n"
        L"Copyright (c) 2005-2014, Felspar Co. Ltd.")
(fostlib::ostream &out, fostlib::arguments &args) {
    boost::filesystem::wpath dbname(coerce<boost::filesystem::wpath>(guid()));

    // Put an empty JSON blob in the database, then insert some data
    {
        jsondb database(dbname, json());
        jsondb::local loc1(database);
        loc1.insert(jcursor(10), json(true)).commit();
    }

    // We can now reload that database and remove the item from the end
    {
        jsondb database(dbname);
        jsondb::local loc1(database);
        loc1.remove(jcursor(10)).commit();
    }

    return 0;
}

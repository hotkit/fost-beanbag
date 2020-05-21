/**
    Copyright 2005-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-jsondb-test.hpp"
#include <fost/string>
#include <fost/main>
#include <fost/test>
#include <fost/jsondb>
#include <fost/unicode>


namespace {
    const fostlib::setting<fostlib::string> c_data_root(
            "fost-jsondb-test-file",
            fostlib::c_jsondb_root,
            fostlib::coerce<fostlib::string>(fostlib::unique_filename()));
}


FSL_MAIN(
        "fost-jsondb-test-file",
        "fost-jsondb-test-file\n"
        "Test the file handling for Fost 4 JSON blobs\n"
        "Copyright (c) 2005-Red Anchor Trading Co. Ltd.")
(fostlib::ostream &out, fostlib::arguments &args) {
    fostlib::fs::path dbname(
            fostlib::coerce<fostlib::fs::path>(fostlib::guid()));

    // Put an empty JSON blob in the database, then insert some data
    {
        fostlib::jsondb database(dbname, fostlib::json{});
        fostlib::jsondb::local loc1(database);
        loc1.insert(fostlib::jcursor(10), fostlib::json{true}).commit();
    }

    // We can now reload that database and remove the item from the end
    {
        fostlib::jsondb database(dbname);
        fostlib::jsondb::local loc1(database);
        loc1.remove(fostlib::jcursor(10)).commit();
    }

    return 0;
}

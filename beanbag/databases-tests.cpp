/*
    Copyright 2012-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/test>
#include <beanbag/beanbag>


namespace {
    const fostlib::setting<fostlib::string> c_data_root(
        "fost-beanbag/databases-tests.cpp", fostlib::c_jsondb_root,
        fostlib::coerce<fostlib::string>(fostlib::unique_filename()));
}


FSL_TEST_SUITE(databases);


FSL_TEST_FUNCTION(can_inject_database) {
    fostlib::json config;
    fostlib::insert(config, "key", "value");
    beanbag::test_database("can_inject_database", config);

    fostlib::jsondb::local trans(*
        beanbag::database(fostlib::json("can_inject_database")));
    FSL_CHECK_EQ(config["key"], trans["key"]);
}


FSL_TEST_FUNCTION(can_delete_database) {
    fostlib::json config;
    fostlib::insert(config, "name", "can_delete_database");
    fostlib::insert(config, "filepath", "can_delete_database.json");
    fostlib::insert(config, "initial", fostlib::json::object_t());
    beanbag::jsondb_ptr db(beanbag::database(config));
    FSL_CHECK(db.get());
    FSL_CHECK(not db->filename().isnull());
    FSL_CHECK(boost::filesystem::exists(db->filename().value()));
    beanbag::remove(db);
    FSL_CHECK(not boost::filesystem::exists(db->filename().value()));
}


/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/log>
#include <fost/test>
#include <beanbag/beanbag>


namespace {
    const fostlib::module c_test(beanbag::c_beanbag, "test");
    const fostlib::module c_test_database(c_test, "database");
}

FSL_TEST_SUITE(databases);


FSL_TEST_FUNCTION(can_inject_database) {
    fostlib::json config, db;
    fostlib::insert(config, "key", "value");
    /// We have to keep a copy of the returned pointer in order to keep
    /// the database alive for the cache.
    auto pdb = beanbag::test_database("can_inject_database", config);

    fostlib::insert(db, "name", "can_inject_database");
    fostlib::insert(db, "filepath", "can_inject_database.json");
    fostlib::insert(db, "initial", fostlib::json::object_t());

    fostlib::jsondb::local trans(*beanbag::database(db));
    FSL_CHECK_EQ(config["key"], trans["key"]);
}


FSL_TEST_FUNCTION(can_delete_database) {
    fostlib::json config;
    fostlib::insert(config, "name", "can_delete_database");
    fostlib::insert(config, "filepath", "can_delete_database.json");
    fostlib::insert(config, "initial", fostlib::json::object_t());
    beanbag::jsondb_ptr db(beanbag::database(config));
    FSL_CHECK(db.get());
    FSL_CHECK(db->filename().has_value());
    fostlib::log::debug(c_test_database)("filename", db->filename().value());
    FSL_CHECK(boost::filesystem::exists(db->filename().value()));
    beanbag::remove(db);
    FSL_CHECK(not boost::filesystem::exists(db->filename().value()));
}

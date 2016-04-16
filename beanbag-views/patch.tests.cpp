/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "patch.hpp"
#include <fost/datetime>
#include <fost/insert>
#include <fost/push_back>
#include <fost/test>
#include <beanbag/jsondb_ptr.hpp>


FSL_TEST_SUITE(patch);


FSL_TEST_FUNCTION(empty_json) {
    fostlib::json empty;
    auto ops = beanbag::patch::operations(empty);
    FSL_CHECK_EQ(ops.size(), 0u);
}


FSL_TEST_FUNCTION(empty_array) {
    fostlib::json empty = fostlib::json::array_t();
    auto ops = beanbag::patch::operations(empty);
    FSL_CHECK_EQ(ops.size(), 0u);
}


FSL_TEST_FUNCTION(json_object_now) {
    auto before = fostlib::timestamp::now();

    fostlib::json now;
    fostlib::insert(now, "!", "op:now");
    fostlib::push_back(now, "path", "time");
    auto ops = beanbag::patch::operations(now);
    FSL_CHECK_EQ(ops.size(), 1u);
    FSL_CHECK(bool(ops[0]));

    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }
    auto after = fostlib::timestamp::now();
    auto recorded = fostlib::coerce<fostlib::timestamp>(
        fostlib::jsondb::local(db).data()["time"]);
    FSL_CHECK(before < recorded);
    FSL_CHECK(recorded < after);
}


FSL_TEST_FUNCTION(json_object_set) {
    fostlib::json set;
    fostlib::insert(set, "!", "op:set");
    fostlib::push_back(set, "path", "location");
    fostlib::insert(set, "value", "key", true);
    auto ops = beanbag::patch::operations(set);
    FSL_CHECK_EQ(ops.size(), 1u);
    FSL_CHECK(bool(ops[0]));

    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        trans.set("something", "here").commit();
    }
    fostlib::json expected;
    fostlib::insert(expected, "something", "here");
    FSL_CHECK_EQ(fostlib::jsondb::local(db).data(), expected);

    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }
    fostlib::insert(expected, "location", "key", true);
    FSL_CHECK_EQ(fostlib::jsondb::local(db).data(), expected);
}


FSL_TEST_FUNCTION(json_z_array) {
    auto before = fostlib::timestamp::now();

    fostlib::json set;
    fostlib::insert(set, 0, "!", "op:set");
    fostlib::push_back(set, 0, "path", "location");
    fostlib::insert(set, 0, "value", "key", true);
    fostlib::insert(set, 1, "!", "op:now");
    fostlib::push_back(set, 1, "path", "time");
    auto ops = beanbag::patch::operations(set);
    FSL_CHECK_EQ(ops.size(), 2u);
    FSL_CHECK(bool(ops[0]));
    FSL_CHECK(bool(ops[1]));

    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        ops[1](trans);
        trans.commit();
    }
    auto after = fostlib::timestamp::now();
    fostlib::jsondb::local trans(db);

    // Result of operation 0
    FSL_CHECK_EQ(trans["location"]["key"], fostlib::json(true));

    // Result of operation 1
    auto recorded = fostlib::coerce<fostlib::timestamp>(
        fostlib::jsondb::local(db).data()["time"]);
    FSL_CHECK(before < recorded);
    FSL_CHECK(recorded < after);
}


/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
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


FSL_TEST_FUNCTION(json_object_add) {
    fostlib::json add;
    fostlib::insert(add, "!", "op:add");
    fostlib::insert(add, "amount", 5);
    fostlib::push_back(add, "path", "counter");
    auto ops = beanbag::patch::operations(add);
    FSL_CHECK_EQ(ops.size(), 1u);
    FSL_CHECK(bool(ops[0]));

    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }
    FSL_CHECK_EQ(fostlib::jsondb::local(db)["counter"], fostlib::json(5));
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }
    FSL_CHECK_EQ(fostlib::jsondb::local(db)["counter"], fostlib::json(10));
}


FSL_TEST_FUNCTION(json_object_created) {
    auto before = fostlib::timestamp::now();

    fostlib::json create;
    fostlib::insert(create, "!", "op:created");
    fostlib::push_back(create, "path", "time");
    auto ops = beanbag::patch::operations(create);
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

    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }
    recorded = fostlib::coerce<fostlib::timestamp>(
            fostlib::jsondb::local(db).data()["time"]);
    FSL_CHECK(before < recorded);
    FSL_CHECK(recorded < after);
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


FSL_TEST_FUNCTION(json_object_remove) {
    fostlib::jcursor location("location");
    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        trans.insert(location, true).commit();
    }

    fostlib::json remove;
    fostlib::insert(remove, "!", "op:remove");
    fostlib::push_back(remove, "path", "location");
    auto ops = beanbag::patch::operations(remove);
    FSL_CHECK_EQ(ops.size(), 1u);
    FSL_CHECK(bool(ops[0]));

    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        trans.commit();
    }

    {
        fostlib::jsondb::local trans(db);
        FSL_CHECK(not trans.has_key(location));
    }
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
    fostlib::insert(set, 2, "!", "op:add");
    fostlib::insert(set, 2, "amount", 2);
    fostlib::push_back(set, 2, "path", "counter");
    auto ops = beanbag::patch::operations(set);
    FSL_CHECK_EQ(ops.size(), 3u);
    FSL_CHECK(bool(ops[0]));
    FSL_CHECK(bool(ops[1]));
    FSL_CHECK(bool(ops[2]));

    fostlib::jsondb db;
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        ops[1](trans);
        ops[2](trans);
        trans.commit();
    }
    auto middle = fostlib::timestamp::now();
    {
        fostlib::jsondb::local trans(db);

        // Result of operation 0
        FSL_CHECK_EQ(trans["location"]["key"], fostlib::json(true));

        // Result of operation 1
        auto recorded = fostlib::coerce<fostlib::timestamp>(
                fostlib::jsondb::local(db).data()["time"]);
        FSL_CHECK(before < recorded);
        FSL_CHECK(recorded < middle);

        // Result of operation 2
        FSL_CHECK_EQ(fostlib::jsondb::local(db)["counter"], fostlib::json(2));
    }
    {
        fostlib::jsondb::local trans(db);
        ops[0](trans);
        ops[1](trans);
        ops[2](trans);
        trans.commit();
    }
    auto after = fostlib::timestamp::now();
    {
        fostlib::jsondb::local trans(db);

        // Result of operation 0
        FSL_CHECK_EQ(trans["location"]["key"], fostlib::json(true));

        // Result of operation 1
        auto recorded = fostlib::coerce<fostlib::timestamp>(
                fostlib::jsondb::local(db).data()["time"]);
        FSL_CHECK(middle < recorded);
        FSL_CHECK(recorded < after);

        // Result of operation 2
        FSL_CHECK_EQ(fostlib::jsondb::local(db)["counter"], fostlib::json(4));
    }
}

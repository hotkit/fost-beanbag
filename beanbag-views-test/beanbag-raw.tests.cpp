/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/insert>
#include <fost/push_back>
#include <fost/test>
#include <beanbag/beanbag>


FSL_TEST_SUITE(beanbag_raw);


/// Needs external linkage (!), for C++03 anyway
extern const char setup_default_name[] = "beanbag.test";


namespace {
    template<typename V = beanbag::raw_view, const char N[] = setup_default_name>
    struct setup {
        setup(bool with_template = true) : view(N), status(0) {
            fostlib::insert(options, "database", N);
            if (with_template) {
                /**
                    With cmake the file gets copied over to the build location
                    from the [CMakeLists.txt](./CMakeLists.txt) file.
                 */
                fostlib::insert(
                        options, "html", "template", "beanbag-raw.tests.html");
            }
        }

        const V view;
        fostlib::mime::mime_headers headers;
        fostlib::json database, options;
        fostlib::host host;
        int status;
        boost::shared_ptr<fostlib::mime> response;
        beanbag::jsondb_ptr dbp;

        void do_request(
                const fostlib::string &method,
                const fostlib::string &pathname,
                const fostlib::string &body_data = fostlib::string()) {
            dbp = beanbag::test_database(N, database);
            auto body = std::make_unique<fostlib::binary_body>(
                    fostlib::coerce<std::vector<unsigned char>>(
                            fostlib::coerce<fostlib::utf8_string>(body_data)),
                    headers);
            fostlib::http::server::request req(
                    method,
                    fostlib::coerce<fostlib::url::filepath_string>(pathname),
                    std::move(body));
            std::pair<boost::shared_ptr<fostlib::mime>, int> res =
                    view(options, pathname, req, host);
            response = res.first;
            status = res.second;
        }
    };
}


FSL_TEST_FUNCTION(get_non_existent_path_returns_404) {
    setup<> env;
    env.headers.set("Accept", "application/json");
    env.do_request("GET", "/not/a/path/");
    FSL_CHECK_EQ(env.status, 404);
    FSL_CHECK_EQ(env.response->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK(
            fostlib::coerce<fostlib::string>(*env.response)
                    .find("Resource not found")
            != fostlib::string::npos);
}


FSL_TEST_FUNCTION(get_with_path_gives_200) {
    setup<> env;
    env.headers.set("Accept", "application/json");
    fostlib::insert(env.database, "is", "a", "path", true);
    env.do_request("GET", "/is/a/path/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK_EQ("true\n", fostlib::coerce<fostlib::string>(*env.response));
}


FSL_TEST_FUNCTION(get_without_accept_and_with_server_template) {
    setup<> env;
    fostlib::insert(env.database, "is", "a", "path", true);
    env.do_request("GET", "/is/a/path/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK(fostlib::coerce<fostlib::string>(*env.response)
                      .startswith("<!doctype xhtml>"));
}


FSL_TEST_FUNCTION(get_without_accept_and_without_server_template) {
    setup<> env(false);
    fostlib::insert(env.database, "is", "a", "path", true);
    env.do_request("GET", "/is/a/path/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK_EQ("true\n", fostlib::coerce<fostlib::string>(*env.response));
    FSL_CHECK_EQ(env.response->headers()["Content-Type"].value(), "text/plain");
}


FSL_TEST_FUNCTION(get_html_has_etag) {
    setup<> env;
    env.do_request("GET", "/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK_EQ(env.response->headers()["Content-Type"].value(), "text/html");
    FSL_CHECK(env.response->headers().exists("ETag"));
    FSL_CHECK_EQ(env.response->headers()["ETag"].value()[0], '"');
    FSL_CHECK_NEQ(env.response->headers()["ETag"].value()[1], '"');
}


FSL_TEST_FUNCTION(get_json_has_etag) {
    setup<> env;
    env.headers.set("Accept", "application/json");
    env.do_request("GET", "/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK_EQ(
            env.response->headers()["Content-Type"].value(),
            "application/json");
    FSL_CHECK(env.response->headers().exists("ETag"));
    FSL_CHECK_EQ(
            env.response->headers()["ETag"].value(),
            "\"37a6259cc0c1dae299a7866489dff0bd\"");
}


FSL_TEST_FUNCTION(put_to_new_path) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.do_request("PUT", "/new/path/3/", "null");
    FSL_CHECK_EQ(put.status, 201);
    FSL_CHECK_EQ(
            put.response->headers()["Content-Type"].value(),
            "application/json");
    FSL_CHECK(put.response->headers().exists("ETag"));
}


FSL_TEST_FUNCTION(put_with_unicode) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.do_request("PUT", "/new/path/4/", fostlib::string(L"\"\\u2014\""));
    FSL_CHECK_EQ(put.status, 201);
    FSL_CHECK_EQ(
            put.response->headers()["Content-Type"].value(),
            "application/json");
    FSL_CHECK_EQ(
            fostlib::coerce<fostlib::string>(*put.response),
            fostlib::string("\"\xE2\x80\x94\"\n"));
    boost::shared_ptr<fostlib::jsondb> db(
            beanbag::database(put.options["database"]));
    fostlib::jsondb::local content(*db);
    FSL_CHECK_EQ(content["new"]["path"][4], fostlib::json(L"\x2014"));
}


FSL_TEST_FUNCTION(conditional_put_matches) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.headers.set("If-Match", "\"37a6259cc0c1dae299a7866489dff0bd\"");
    put.do_request("PUT", "/", "[]");
    FSL_CHECK_EQ(put.status, 201);
    FSL_CHECK_EQ(
            put.response->headers()["Content-Type"].value(),
            "application/json");
}


FSL_TEST_FUNCTION(conditional_put_does_not_match) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.headers.set("If-Match", "\"invalid-etag-value\"");
    put.do_request("PUT", "/", "[]");
    FSL_CHECK_EQ(put.status, 412);
    FSL_CHECK_EQ(put.response->headers()["Content-Type"].value(), "text/html");
}


FSL_TEST_FUNCTION(conditional_put_does_not_match_path_does_not_exist) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.headers.set("If-Match", "\"invalid-etag-value\"");
    put.do_request("PUT", "/path/", "[]");
    FSL_CHECK_EQ(put.status, 412);
    FSL_CHECK_EQ(put.response->headers()["Content-Type"].value(), "text/html");
}


FSL_TEST_FUNCTION(conditional_put_matches_wildcard) {
    setup<> put;
    fostlib::insert(put.database, "path", fostlib::json());
    put.headers.set("Accept", "application/json");
    put.headers.set("If-Match", "*");
    put.do_request("PUT", "/path/", "[]");
    FSL_CHECK_EQ(put.status, 200);
    FSL_CHECK_EQ(
            put.response->headers()["Content-Type"].value(),
            "application/json");
}


FSL_TEST_FUNCTION(conditional_put_does_not_match_wildcard) {
    setup<> put;
    put.headers.set("Accept", "application/json");
    put.headers.set("If-Match", "*");
    put.do_request("PUT", "/path/", "[]");
    FSL_CHECK_EQ(put.status, 412);
    FSL_CHECK_EQ(put.response->headers()["Content-Type"].value(), "text/html");
}


FSL_TEST_FUNCTION(patch) {
    setup<> patch;
    fostlib::insert(patch.database, "path", fostlib::json::object_t());
    fostlib::json transforms;
    fostlib::insert(transforms, "transforms", "!", "op:set");
    fostlib::push_back(transforms, "transforms", "path", "location");
    fostlib::insert(transforms, "transforms", "value", true);

    fostlib::json expected;
    fostlib::insert(expected, "path", "location", true);

    patch.do_request(
            "PATCH", "/path/", fostlib::json::unparse(transforms, true));

    FSL_CHECK_EQ(patch.status, 200);
    fostlib::jsondb::local content(*patch.dbp);
    FSL_CHECK_EQ(content.data(), expected);
}


FSL_TEST_FUNCTION(delete_on_empty_database) {
    setup<> env;
    fostlib::insert(env.database, "path", fostlib::json());
    env.do_request("DELETE", "/path/");
    FSL_CHECK_EQ(env.status, 200);
}


FSL_TEST_FUNCTION(delete_beanbag) {
    setup<> env;
    env.do_request("DELETE", "/");
    FSL_CHECK_EQ(env.status, 200);
}


namespace {
    class no_del : public beanbag::raw_view {
      public:
        no_del(const fostlib::string &n) : raw_view(n) {}

      protected:
        int do_delete_check(
                int fallback,
                const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &host,
                fostlib::jsondb::local &db,
                const fostlib::jcursor &position) const {
            return 403;
        }
    };
}
extern const char no_del_view[] = "beanbag.test.no-del";
FSL_TEST_FUNCTION(del_disallowd) {
    setup<no_del, no_del_view> env;
    env.do_request("DELETE", "/");
    FSL_CHECK_EQ(env.status, 403);
}


FSL_TEST_FUNCTION(etag_empty) {
    setup<> env;
    // null
    FSL_CHECK_EQ(
            env.view.etag(fostlib::json()),
            "\"37a6259cc0c1dae299a7866489dff0bd\"");
}


FSL_TEST_FUNCTION(etag_empty_object) {
    setup<> env;
    // {}
    FSL_CHECK_EQ(
            env.view.etag(fostlib::json::object_t()),
            "\"99914b932bd37a50b983c5e7c90ae93b\"");
}


FSL_TEST_FUNCTION(etag_empty_array) {
    setup<> env;
    // []
    FSL_CHECK_EQ(
            env.view.etag(fostlib::json::array_t()),
            "\"d751713988987e9331980363e24189ce\"");
}


FSL_TEST_FUNCTION(etag_data) {
    setup<> env;
    fostlib::json data;
    // null
    FSL_CHECK_EQ(env.view.etag(data), "\"37a6259cc0c1dae299a7866489dff0bd\"");
    fostlib::insert(data, "key1", true);
    fostlib::insert(data, "Key1", false);
    // {"Key1":false,"key1":true}
    FSL_CHECK_EQ(env.view.etag(data), "\"27cfa6ee311d6e30abd463b33dc1dc6e\"");
    fostlib::push_back(data, "array1", 0);
    fostlib::push_back(data, "array1", 0);
    fostlib::push_back(data, "array1", 0);
    fostlib::push_back(data, "array1", fostlib::json::object_t());
    // {"Key1":false,"array1":[0,0,0,{}],"key1":true}
    FSL_CHECK_EQ(env.view.etag(data), "\"927d3596274733e668a65d3fda95915f\"");
    fostlib::insert(data, "array1", 3, "@context", "embedded");
    // {"Key1":false,"array1":[0,0,0,{"@context":"embedded"}],"key1":true}
    FSL_CHECK_EQ(env.view.etag(data), "\"46eccc62fe08b8e1b694ff0c1b5a6c56\"");
}

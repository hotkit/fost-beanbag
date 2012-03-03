/*
    Copyright 2012 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/test>
#include "databases.hpp"
#include <beanbag/beanbag>


FSL_TEST_SUITE(beanbag_raw);


namespace {
    struct setup {
        setup()
        : view("beanbag.test"), status(0) {
            beanbag::test_database("beanbag.test", config);
            fostlib::insert(options, "database", "beanbag.test");
            fostlib::insert(options, "html", "template",
                "../../usr/share/beanbag/raw/template.html");
        }

        const beanbag::raw_view view;
        fostlib::mime::mime_headers headers;
        fostlib::json config, options;
        fostlib::host host;
        int status;
        boost::shared_ptr<fostlib::mime> response;

        void do_request(
                const fostlib::string &method,
                const fostlib::string &pathname,
                const fostlib::ascii_printable_string &query_string = "") {
            std::auto_ptr< fostlib::binary_body > body(new fostlib::binary_body(headers));
            fostlib::http::server::request req(
                method, fostlib::coerce<fostlib::url::filepath_string>(pathname), body);
            std::pair<boost::shared_ptr<fostlib::mime>, int> res =
                view(options, pathname, req, host);
            response = res.first;
            status = res.second;
        }
    };
}


FSL_TEST_FUNCTION(get_html_has_etag) {
    setup env;
    env.do_request("GET", "/");
    FSL_CHECK_EQ(env.status, 200);
    FSL_CHECK_EQ(
        env.response->headers()["Content-Type"].value(),
        "text/html");
    FSL_CHECK(env.response->headers().exists("ETag"));
}


// FSL_TEST_FUNCTION(get_json_has_etag) {
//     setup env;
//     env.do_request("GET", "/", "__=");
//     FSL_CHECK_EQ(env.status, 200);
//     FSL_CHECK_EQ(
//         env.response->headers()["Content-Type"].value(),
//         "application/json");
//     FSL_CHECK(env.response->headers().exists("ETag"));
//     FSL_CHECK_EQ(
//         env.response->headers()["ETag"].value(),
//         "\"37a6259cc0c1dae299a7866489dff0bd\"");
// }


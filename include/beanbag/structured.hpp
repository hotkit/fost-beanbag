/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once
#ifndef BEANBAG_STRUCTURED_HPP
#define BEANBAG_STRUCTURED_HPP


#include <beanbag/raw.hpp>


namespace beanbag {


    class FOST_BEANBAG_VIEWS_DECLSPEC structured_view : public raw_view {
        fostlib::jcursor relocated(
                fostlib::jsondb::local &db, fostlib::jcursor position) const;

      public:
        structured_view(const fostlib::string &name);

        std::pair<fostlib::json, int>
                get(const fostlib::json &options,
                    const fostlib::string &pathname,
                    fostlib::http::server::request &req,
                    const fostlib::host &,
                    fostlib::jsondb::local &db,
                    const fostlib::jcursor &position) const;

        int put(const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &,
                fostlib::jsondb::local &db,
                const fostlib::jcursor &position) const;

        int del(const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &,
                fostlib::jsondb::local &db,
                const fostlib::jcursor &position) const;
    };


}


#endif // BEANBAG_STRUCTURED_HPP

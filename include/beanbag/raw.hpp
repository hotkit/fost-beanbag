/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once
#ifndef BEANBAG_RAW_HPP
#define BEANBAG_RAW_HPP


#include <fost/urlhandler>
#include <beanbag/jsondb_ptr.hpp>


namespace beanbag {


    extern const class FOST_BEANBAG_VIEWS_DECLSPEC raw_view :
    public fostlib::urlhandler::view {
      public:
        /// Construct a raw view giving it the provided name
        raw_view(const fostlib::string &name);

        /// The standard view implementation operator
        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &) const;

        /// Return the database to use
        virtual beanbag::jsondb_ptr database(
                const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &) const;

        /// Calculate the position into the JSON structure for the given path
        virtual fostlib::jcursor position(const fostlib::string &pathname) const;

        /// Return the JSON that will form the basis of a GET response
        virtual std::pair<fostlib::json, int>
                get(const fostlib::json &options,
                    const fostlib::string &pathname,
                    fostlib::http::server::request &req,
                    const fostlib::host &,
                    fostlib::jsondb::local &db,
                    const fostlib::jcursor &position) const;

        /// Return status code for the PATCH response. The body will be from the
        /// GET handler
        virtual int
                patch(const fostlib::json &options,
                      const fostlib::string &pathname,
                      fostlib::http::server::request &req,
                      const fostlib::host &,
                      fostlib::jsondb::local &db,
                      const fostlib::jcursor &position) const;

        /// Return status code for the PUT response. The body will be from the
        /// GET handler
        virtual int
                put(const fostlib::json &options,
                    const fostlib::string &pathname,
                    fostlib::http::server::request &req,
                    const fostlib::host &,
                    fostlib::jsondb::local &db,
                    const fostlib::jcursor &position) const;

        /// Return the status code for the DELETE response
        virtual int
                del(const fostlib::json &options,
                    const fostlib::string &pathname,
                    fostlib::http::server::request &req,
                    const fostlib::host &,
                    fostlib::jsondb::local &db,
                    const fostlib::jcursor &position) const;

        /// Return the ETag for the specified part of the JSON structure
        virtual fostlib::string etag(const fostlib::json &structure) const;

        /// Prepare a JSON response
        virtual boost::shared_ptr<fostlib::mime> json_response(
                const fostlib::json &options,
                const fostlib::json &body,
                fostlib::mime::mime_headers &headers,
                const fostlib::jcursor &position_jc) const;

        /// Prepare a HTML response
        virtual boost::shared_ptr<fostlib::mime> html_response(
                const fostlib::json &options,
                const fostlib::json &body,
                fostlib::mime::mime_headers &headers,
                const fostlib::jcursor &position_jc) const;

      protected:
        /// Called before a beanbag is removed. To override the removal return
        /// anything but 200
        virtual int do_delete_check(
                int fallback,
                const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &,
                fostlib::jsondb::local &db,
                const fostlib::jcursor &position) const;

      private:
        int do_412_check(
                int fallback,
                const fostlib::json &options,
                const fostlib::string &pathname,
                fostlib::http::server::request &req,
                const fostlib::host &,
                fostlib::jsondb::local &db,
                const fostlib::jcursor &position) const;
    } c_raw_view;


}


#endif // BEANBAG_RAW_HPP

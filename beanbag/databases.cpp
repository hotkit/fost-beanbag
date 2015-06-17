/*
    Copyright 2012-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "beanbag.hpp"
#include <beanbag/databases.hpp>
#include <f5/threading/map.hpp>
#include <fost/insert>
#include <fost/unicode>


namespace {
    f5::tsmap<fostlib::string, boost::weak_ptr<fostlib::jsondb>,
        f5::weak_ptr_promotion_policy<boost::weak_ptr<fostlib::jsondb>>>
            g_databases;
}


beanbag::jsondb_ptr beanbag::database(
    const fostlib::json &which
) {
    try {
        fostlib::nullable<fostlib::string> which_name(which.get<fostlib::string>());
        fostlib::string name;
        if ( which_name.isnull() )
            name = fostlib::coerce<fostlib::string>(which["name"]);
        else
            name = which_name.value();

        beanbag::jsondb_ptr cptr;
        auto make = [&which, &cptr]() {
                if ( !which.has_key("filepath") )
                    throw fostlib::exceptions::null(
                        "Beanbag configurations must specify a file path in the 'filepath' member");
                fostlib::json tplate;
                if ( which.has_key("template") ) {
                    tplate = fostlib::json::parse(fostlib::utf::load_file(
                        fostlib::coerce<boost::filesystem::wpath>(which["template"])));
                } else if ( which.has_key("initial") ) {
                    tplate = which["initial"];
                } else {
                    // Without some sort of template we insist the file exists
                    cptr =  boost::make_shared<fostlib::jsondb>(
                            fostlib::coerce<fostlib::string>(which["filepath"]));
                    return cptr;
                }
                // With a template of some sort we can create a new disk file
                cptr = boost::make_shared< fostlib::jsondb>(
                        fostlib::coerce<fostlib::string>(which["filepath"]),
                        tplate);
                return cptr;
            };
        auto fptr = g_databases.add_if_not_found(name, make);
        if ( cptr ) {
            // cptr is set if there was a cache miss and the lambda generated
            // a new value
            return cptr;
        } else if ( fptr ) {
            // fptr is set (and has a value) if we got a cache hit and the
            // database is still held somewhere
            return fptr;
        } else {
            // There is a dead reference in the table -- do something....
            // just not something with a race
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "When we get a dead reference");
        }
    } catch ( fostlib::exceptions::exception &e ) {
        fostlib::insert(e.data(), "opening-beanbag", which);
        throw;
    }
}


void beanbag::remove(beanbag::jsondb_ptr db) {
    throw fostlib::exceptions::not_implemented(__FUNCTION__);
//     { // Don't hold the lock while we hit the file system
//         boost::mutex::scoped_lock lock(g_mutex);
//         for ( databases_t::iterator iter(g_databases.begin()); iter != g_databases.end(); ) {
//             if ( iter->second == db ) {
//                 g_databases.erase(iter++); // Note post increment
//             } else {
//                 ++iter;
//             }
//         }
//     }
//     if ( not db->filename().isnull() ) {
//         boost::filesystem::remove(db->filename().value());
//     }
}


void beanbag::test_database(const fostlib::string &name, const fostlib::json &blob) {
    jsondb_ptr db(boost::make_shared<fostlib::jsondb>());
    fostlib::jsondb::local transaction(*db);
    transaction.insert(fostlib::jcursor(), blob);
    transaction.commit();
    g_databases.insert_or_assign(name, db);
}


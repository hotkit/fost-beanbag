/*
    Copyright 2012-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "beanbag.hpp"
#include <beanbag/databases.hpp>
#include <f5/threading/map.hpp>
#include <fost/counter>
#include <fost/insert>
#include <fost/log>
#include <fost/unicode>
#include <atomic>


const fostlib::module beanbag::c_beanbag("beanbag");


namespace {
    f5::tsmap<fostlib::string, boost::weak_ptr<fostlib::jsondb>,
        f5::weak_ptr_promotion_policy<boost::weak_ptr<fostlib::jsondb>>>
            g_databases;

    fostlib::performance p_bound(beanbag::c_beanbag, "database", "cache-size");
    fostlib::performance p_loaded(beanbag::c_beanbag, "database", "loaded");
    fostlib::performance p_found_alive(beanbag::c_beanbag, "database", "found-alive");
    fostlib::performance p_found_dead(beanbag::c_beanbag, "database", "found-dead");
    fostlib::performance p_expired(beanbag::c_beanbag, "database", "expired");
}


beanbag::jsondb_ptr beanbag::database(
    const fostlib::json &which
) {
    static std::atomic<int64_t> created{0};
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
                ++created;
                if ( which.has_key("template") ) {
                    tplate = fostlib::json::parse(fostlib::utf::load_file(
                        fostlib::coerce<boost::filesystem::wpath>(which["template"])));
                } else if ( which.has_key("initial") ) {
                    tplate = which["initial"];
                } else {
                    // Without some sort of template we insist the file exists
                    ++p_loaded;
                    cptr =  boost::make_shared<fostlib::jsondb>(
                            fostlib::coerce<fostlib::string>(which["filepath"]));
                    return cptr;
                }
                // With a template of some sort we can create a new disk file
                ++p_loaded;
                cptr = boost::make_shared< fostlib::jsondb>(
                        fostlib::coerce<fostlib::string>(which["filepath"]),
                        tplate);
                return cptr;
            };
        auto predicate = [&cptr](boost::weak_ptr<fostlib::jsondb> p) {
                cptr = p.lock();
                if ( cptr ) {
                    ++p_found_alive;
                    return false;
                } else {
                    ++p_found_dead;
                    return true;
                }
            };
        g_databases.insert_or_assign_if(name, predicate, make);
        const auto size = g_databases.size();
        if ( size > p_bound.value() ) {
            const auto left = g_databases.remove_if(
                [](const auto &, const auto &p) {
                    if ( p.expired() ) {
                        ++p_expired;
                        return true;
                    } else
                        return false;
                });
            if ( left > p_bound.value() ) {
                const auto old = p_bound.value();
                const auto now = (p_bound += 16);
                fostlib::log::warning(c_beanbag)
                    ("", "Clearing out old beanbags")
                    ("bound", "old", old)
                    ("bound", "new", now)
                    ("size", "old", size)
                    ("size", "new", left);
            } else {
                fostlib::log::info(c_beanbag)
                    ("", "Clearing out old beanbags")
                    ("size", "old", size)
                    ("size", "new", left);
            }
        }
        return cptr;
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


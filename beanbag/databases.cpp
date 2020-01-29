/**
    Copyright 2012-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
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
    /**
        The beanbag cache is comprised of three layers.

        1. The LRU is the 'least recently used' ones. These are the latest
            that have been asked for by application code.
        2. The cold store are beanbags that have previously been in the LRU
        3. We also need to track every beanbag that is loaded in case we have
            to revive one. This is the `g_databases`

        When a beanbag is loaded it goes into the LRU if there is room. If there
        is no room then the LRU is demoted to cold and the cold is emptied and
        used as the new LRU.

        When a beanbag is loaded the it is only necessary to search `g_databases`
        because all databases must be present in that data structure.

        This algorithm ensures we don't need to sort the LRU by last access to
        push out the least recently used ones, but it will have the same effect
        based on how often databases are loaded.
     */
    using cache_type = std::vector<std::pair<fostlib::string, boost::shared_ptr<fostlib::jsondb>>>;
    cache_type g_lru, g_cold;
    /**
        In order to ensure safey of manipulating the caches we need to have a
        mutex. The mutex is used for both `g_lru` and `g_cold`. A single mutex
        is used becaues the data structures are used and updated together.
     */
    std::mutex g_cache_mutex;

    /// Weak pointer store for all beanbags that have been loaded.
    f5::tsmap<fostlib::string, boost::weak_ptr<fostlib::jsondb>> g_databases;

    /// Various performance indicators.
    fostlib::performance p_held(beanbag::c_beanbag, "database", "live");
    fostlib::performance p_bound(beanbag::c_beanbag, "database", "cache-size");
    fostlib::performance p_loaded(beanbag::c_beanbag, "database", "loaded");
    fostlib::performance
            p_found_alive(beanbag::c_beanbag, "database", "found-alive");
    fostlib::performance
            p_found_dead(beanbag::c_beanbag, "database", "found-dead");
    fostlib::performance p_expired(beanbag::c_beanbag, "database", "expired");

    fostlib::string get_db_name(fostlib::json const &);
    /**
        Search in `g_databases` for the database and load from disk if it can't
        be recovered from memory.
        */
    beanbag::jsondb_ptr search_databases(fostlib::string const &, fostlib::json const &);
    void register_to_lru(fostlib::string const &, beanbag::jsondb_ptr);
}


beanbag::jsondb_ptr beanbag::database(fostlib::json const &which) {
    auto const name = get_db_name(which);
    auto const dbp = search_databases(name, which);
    register_to_lru(name, dbp);
    return dbp;
}


void beanbag::remove(beanbag::jsondb_ptr db) {
    if (db->filename()) { boost::filesystem::remove(db->filename().value()); }
}


beanbag::jsondb_ptr beanbag::test_database(
        const fostlib::string &name, const fostlib::json &blob) {
    jsondb_ptr db(boost::make_shared<fostlib::jsondb>());
    fostlib::jsondb::local transaction(*db);
    transaction.insert(fostlib::jcursor(), blob);
    transaction.commit();
    g_databases.insert_or_assign(name, db);
    return db;
}


/**
 * ## Implementation details
 */
namespace {
    fostlib::string get_db_name(fostlib::json const &which) {
        auto which_name(fostlib::coerce<fostlib::nullable<f5::u8view>>(which));
        if (not which_name) {
            return fostlib::coerce<fostlib::string>(which["name"]);
        } else {
            return which_name.value();
        }
    }
    beanbag::jsondb_ptr search_databases(fostlib::string const &name, fostlib::json const &which) {
        try {
            beanbag::jsondb_ptr cptr;
            auto make = [&which, &cptr]() {
                if (!which.has_key("filepath"))
                    throw fostlib::exceptions::null(
                            "Beanbag configurations must specify a file path in "
                            "the 'filepath' member");
                fostlib::json tplate;
                if (which.has_key("template")) {
                    tplate = fostlib::json::parse(fostlib::utf::load_file(
                            fostlib::coerce<boost::filesystem::path>(
                                    which["template"])));
                } else if (which.has_key("initial")) {
                    tplate = which["initial"];
                } else {
                    // Without some sort of template we insist the file exists
                    ++p_loaded;
                    ++p_held;
                    cptr = boost::make_shared<fostlib::jsondb>(
                            fostlib::coerce<fostlib::string>(which["filepath"]));
                    return cptr;
                }
                // With a template of some sort we can create a new disk file
                ++p_loaded;
                ++p_held;
                cptr = boost::make_shared<fostlib::jsondb>(
                        fostlib::coerce<fostlib::string>(which["filepath"]),
                        tplate);
                return cptr;
            };
            auto predicate = [&cptr](boost::weak_ptr<fostlib::jsondb> &p) {
                cptr = p.lock();
                if (cptr) {
                    ++p_found_alive;
                    return false;
                } else {
                    ++p_found_dead;
                    --p_held;
                    return true;
                }
            };
            g_databases.insert_or_assign_if(name, predicate, make);
            const auto size = g_databases.size();
            if (fostlib::coerce<decltype(p_bound.value())>(size)
                > p_bound.value()) {
                const auto left =
                        g_databases.remove_if([](const auto &, const auto &p) {
                            if (p.expired()) {
                                ++p_expired;
                                --p_held;
                                return true;
                            } else
                                return false;
                        });
                if (fostlib::coerce<decltype(p_bound.value())>(left)
                    > p_bound.value()) {
                    const auto old = p_bound.value();
                    const auto now = (p_bound += 16);
                    fostlib::log::debug(beanbag::c_beanbag)(
                            "", "Clearing out old beanbags -- increasing bound")(
                            "bound", "old", old)("bound", "new", now)(
                            "size", "old", size)("size", "new", left);
                } else {
                    fostlib::log::debug(beanbag::c_beanbag)(
                            "", "Clearing out old beanbags -- keeping old bound")(
                            "size", "old", size)("size", "new", left);
                }
            }
            return cptr;
        } catch (fostlib::exceptions::exception &e) {
            fostlib::insert(e.data(), "opening-beanbag", which);
            throw;
        }
    }
    void register_to_lru(fostlib::string const &name, beanbag::jsondb_ptr dbp) {
        std::scoped_lock lock{g_cache_mutex};
        auto logger = fostlib::log::debug(beanbag::c_beanbag);
        logger("", "register_to_lru");
        logger("name", name);
        if (std::find(g_lru.begin(), g_lru.end(), std::pair{name, dbp}) == g_lru.end()) {
            logger("status", "Adding to LRU");
            g_lru.push_back({name, dbp});
            logger("lru", "size", g_lru.size());
            if(g_lru.size() > 10u) {
                std::swap(g_lru, g_cold);
                g_lru.clear();
            }
        } else {
            logger("status", "Already in LRU");
        }
    }
}

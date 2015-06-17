/*
    Copyright 2012-2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "beanbag-views.hpp"
#include <beanbag/databases.hpp>
#include <fost/unicode>


namespace {
    typedef std::map< fostlib::string, beanbag::jsondb_ptr > databases_t;
    boost::mutex g_mutex;
    databases_t g_databases;
}


beanbag::jsondb_ptr beanbag::database(
    const fostlib::json &which
) {
    fostlib::nullable<fostlib::string> which_name(which.get<fostlib::string>());
    fostlib::string name;
    if ( which_name.isnull() )
        name = fostlib::coerce<fostlib::string>(which["name"]);
    else
        name = which_name.value();

    boost::mutex::scoped_lock lock(g_mutex);
    databases_t::const_iterator loc(g_databases.find(name));

    if ( loc == g_databases.end() ) {
        fostlib::json tplate;
        if ( which.has_key("template") )
            tplate = fostlib::json::parse(fostlib::utf::load_file(
                fostlib::coerce<boost::filesystem::wpath>(which["template"])));
        else if ( which.has_key("initial") )
            tplate = which["initial"];
        else {
            boost::shared_ptr< fostlib::jsondb > db(
                new fostlib::jsondb(
                    fostlib::coerce<fostlib::string>(which["filepath"])));
            g_databases[name] = db;
            return db;
        }
        boost::shared_ptr< fostlib::jsondb > db(
            new fostlib::jsondb(
                fostlib::coerce<fostlib::string>(which["filepath"]),
                tplate) );
        g_databases[name] = db;
        return db;
    } else
        return loc->second;
}


void beanbag::alias(const fostlib::string &name, beanbag::jsondb_ptr db) {
    boost::mutex::scoped_lock lock(g_mutex);
    g_databases[name] = db;
}


void beanbag::remove(beanbag::jsondb_ptr db) {
    { // Don't hold the lock while we hit the file system
        boost::mutex::scoped_lock lock(g_mutex);
        for ( databases_t::iterator iter(g_databases.begin()); iter != g_databases.end(); ) {
            if ( iter->second == db ) {
                g_databases.erase(iter++); // Note post increment
            } else {
                ++iter;
            }
        }
    }
    if ( not db->filename().isnull() ) {
        boost::filesystem::remove(db->filename().value());
    }
}


void beanbag::test_database(const fostlib::string &name, const fostlib::json &blob) {
    jsondb_ptr db(new fostlib::jsondb);
    fostlib::jsondb::local transaction(*db);
    transaction.insert(fostlib::jcursor(), blob);
    transaction.commit();
    alias(name, db);
}


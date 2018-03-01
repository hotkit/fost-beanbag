/*
    Copyright 2005-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/string>
#include <fost/unicode>
#include <fost/main>
#include <fost/test>

#include <fost/db>
#include <fost/jsondb>


using namespace fostlib;


// This function is in database.cpp
void do_insert_test( dbconnection &dbc );


FSL_MAIN(
    L"fost-schema-test-jsondb-file",
    L"fost-schema-test-jsondb-file\nTest the file handling for Fost 4 JSON databases\nCopyright (C) 2008-2017, Felspar Co. Ltd."
)( fostlib::ostream &out, fostlib::arguments &args ) {
    if ( not args[1] ) {
        out << L"Supply a JSON file which contains the database configuration as the first parameter" << std::endl;
        return 1;
    }

    // Override the pretty print setting
    setting<bool> no_pretty_print("", c_jsondb_pretty_print, false);

    /*
        Load the configuration and set a root if one isn't already set
    */
    json configuration(json::parse(utf::load_file(
        coerce<boost::filesystem::path>(args[1].value()))));
    if ( !configuration.has_key( L"root" ) )
        jcursor("root")(configuration) = "../fost-orm/Cpp/fost-schema-test/jsondb-file";

    /*
        Connect to the master and then create the new database
    */
    dbconnection master(configuration);
    string dbname(guid());
    master.create_database(dbname);

    /*
        Create a new configuration and then connect to the database
    */
    json new_config;
    jcursor( L"database" )( new_config ) = dbname;
    jcursor( L"root" )( new_config ) = configuration[ L"root" ];
    jcursor( L"filename" )( new_config ) = dbname + L".json";
    dbconnection dbc( new_config );

    /*
        Repeat one of the database tests, but this time it should save to disk
    */
    do_insert_test( dbc );

    /*
        Now check that the file on disk matches the first test file
    */
    auto fnn = coerce<boost::filesystem::path>(
        concat(coerce<string>(new_config["root"]),
            "/", coerce<string>(new_config["filename"])));
    auto fno = coerce<boost::filesystem::path>(
        concat(coerce<string>(new_config["root"]),
            "/", "../first-test.json"));
    out << "Comparing " << fnn << " and " << fno << std::endl;
    FSL_CHECK_EQ(
        fostlib::trim(utf::load_file(fnn)).value(),
        fostlib::trim(utf::load_file(fno)).value());

    return 0;
}


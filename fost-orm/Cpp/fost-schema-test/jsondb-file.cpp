/*
    Copyright 2005-2013, Felspar Co Ltd. http://support.felspar.com/
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
    L"fost-schema-test-jsondb-file\nTest the file handling for Fost 4 JSON databases\nCopyright (c) 2008-2010, Felspar Co. Ltd."
)( fostlib::ostream &out, fostlib::arguments &args ) {
    if ( args[ 1 ].isnull() ) {
        out << L"Supply a JSON file which contains the database configuration as the first parameter" << std::endl;
        return 1;
    }

    // Override the pretty print setting
    setting<bool> no_pretty_print("", c_jsondb_pretty_print, false);

    /*
        Load the configuration and set a root if one isn't already set
    */
    json configuration( json::parse( utf::load_file( coerce< std::wstring >( args[ 1 ].value() ).c_str() ) ) );
    if ( !configuration.has_key( L"root" ) )
        jcursor( L"root" )( configuration ) = L"../fost-orm/Cpp/fost-schema-test/jsondb-file";

    /*
        Connect to the master and then create the new database
    */
    dbconnection master( configuration );
    string dbname( guid() );
    master.create_database( dbname );

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
    FSL_CHECK_EQ(
        fostlib::trim( utf::load_file( coerce< std::wstring >( concat(
            new_config[ L"root" ].get< string >().value(), L"/", new_config[ L"filename" ].get< string >().value()
        ).value() ).c_str() ) ).value(),
        fostlib::trim( utf::load_file( coerce< std::wstring >( concat(
            new_config[ L"root" ].get< string >().value(), L"/../", L"first-test.json"
        ).value() ).c_str() ) ).value()
    );

    return 0;
}


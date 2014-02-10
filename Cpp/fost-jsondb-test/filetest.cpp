/*
    Copyright 2005-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb-test.hpp"
#include <fost/string>
#include <fost/main>
#include <fost/test>
#include <fost/jsondb>
#include <fost/unicode>

#include <boost/filesystem.hpp>


using namespace fostlib;


namespace {
    const setting< bool > c_keep_file(
        "fost-jsondb-test-file", "fost-jsondb-test-file", "Keep database file",
        false, true);
}


FSL_MAIN(
    L"fost-jsondb-test-file",
    L"fost-jsondb-test-file\n"
    L"Test the file handling for Fost 4 JSON blobs\n"
    L"Copyright (c) 2008-2010, Felspar Co. Ltd."
)( fostlib::ostream &out, fostlib::arguments &args ) {
    boost::filesystem::wpath dbname( coerce<boost::filesystem::wpath>(guid()) );

    // Put an empty JSON blob in the database
    {
        jsondb database( dbname, json() );
        jsondb::local loc1( database );
        loc1
            .insert( jcursor(10), json(true) )
            .commit()
        ;
    }

    // We can now reload that database and remove the item from the end
    {
        jsondb database( dbname );
        jsondb::local loc1( database );
        loc1
            .remove( jcursor(10) )
            .commit()
        ;
    }

    if ( !c_keep_file.value() )
        boost::filesystem::remove(dbname);

    return 0;
}

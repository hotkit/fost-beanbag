/*
    Copyright 2009-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/core>
#include <fost/main>
#include <fost/test>

#include <fost/db>


using namespace fostlib;


FSL_MAIN(
    L"fost-schema-test-dyndriver",
    L"fost-schema-test-dyndriver\nTest the dynamic loading of database driver files\nCopyright (c) 2009, Felspar Co. Ltd."
)( fostlib::ostream &out, fostlib::arguments &args ) {
    if ( args.size() < 3 ) {
        out << L"Usage\n    fost-schema-test-dyndriver driver-name driver-file [-d dsn]" << std::endl;
        return 1;
    }
    setting< string > configuration( L"fost-schema-test-dyndriver", L"Database drivers", args[ 1 ].value(), args[ 2 ].value(), false );
    nullable< string > dsn = args.commandSwitch("d");
    out << configuration.name() << ": " << configuration.value()
        << ": " << dsn.value_or("No DSN") << std::endl;
    dbconnection dbc(concat(args[ 1 ].value() + "/", dsn.value_or(string())).value());
    return 0;
}


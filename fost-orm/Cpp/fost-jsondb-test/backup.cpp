/*
    Copyright 2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb-test.hpp"
#include <fost/main>
#include <fost/test>
#include <fost/unicode>


using namespace fostlib;
namespace bfs = boost::filesystem;


FSL_MAIN(
    L"fost-jsondb-test-backup",
    L"fost-jsondb-test-backup\n"
    L"Test the backup process for Fost 4 JSON blobs\n"
    L"Copyright (c) 2008-2010, Felspar Co. Ltd."
)( fostlib::ostream &out, fostlib::arguments &args ) {
    const bfs::wpath backup(L"filename.backup");
    const bfs::wpath temp(L"filename.tmp");
    const bfs::wpath filename(L"filename.txt");

    const string old_backup("old backup\n");
    const string original("original contents\n");
    const string updated("updated contents\n");

    if ( bfs::exists(backup) )
        bfs::remove(backup);
    if ( bfs::exists(temp) )
        bfs::remove(temp);
    if ( bfs::exists(filename) )
        bfs::remove(filename);

    FSL_CHECK_NOTHROW(utf::save_file(backup, old_backup));

    FSL_CHECK_NOTHROW(utf::save_file(filename, original));
    string read1(utf::load_file(filename));
    FSL_CHECK_EQ(read1, original);

    FSL_CHECK_NOTHROW(
        if ( bfs::exists(backup) )
            bfs::remove(backup);
        bfs::create_hard_link(filename, backup));
    string read2(utf::load_file(backup));
    FSL_CHECK_EQ(read2, original);

    FSL_CHECK_NOTHROW(utf::save_file(temp, updated));
    string read3(utf::load_file(temp));
    FSL_CHECK_EQ(read3, updated);
    string read4(utf::load_file(filename));
    FSL_CHECK_EQ(read4, original);
    string read5(utf::load_file(backup));
    FSL_CHECK_EQ(read5, original);

#if ( BOOST_VERSION_MAJOR < 46 )
    if ( bfs::exists(filename) )
        bfs::remove(filename);
#endif

    FSL_CHECK_NOTHROW(bfs::rename(temp, filename));
    string read6(utf::load_file(filename));
    FSL_CHECK_EQ(read6, updated);
    string read7(utf::load_file(backup));
    FSL_CHECK_EQ(read7, original);

    return 0;
}

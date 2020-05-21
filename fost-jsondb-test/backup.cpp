/**
    Copyright 2012-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-jsondb-test.hpp"
#include <fost/jsondb>
#include <fost/main>
#include <fost/test>
#include <fost/unicode>


FSL_MAIN(
        "fost-jsondb-test-backup",
        "fost-jsondb-test-backup\n"
        "Test the backup process for Fost 4 JSON blobs\n"
        "Copyright (c) 2008-2020 Red Anchor Trading Co. Ltd.")
(fostlib::ostream &out, fostlib::arguments &args) {
    fostlib::fs::path const directory(fostlib::unique_filename());
    fostlib::fs::create_directories(directory);

    const fostlib::fs::path backup(directory / "filename.backup");
    const fostlib::fs::path temp(directory / "filename.tmp");
    const fostlib::fs::path filename(directory / "filename.txt");

    const fostlib::string old_backup("old backup\n");
    const fostlib::string original("original contents\n");
    const fostlib::string updated("updated contents\n");

    if (fostlib::fs::exists(backup)) fostlib::fs::remove(backup);
    if (fostlib::fs::exists(temp)) fostlib::fs::remove(temp);
    if (fostlib::fs::exists(filename)) fostlib::fs::remove(filename);

    FSL_CHECK_NOTHROW(fostlib::utf::save_file(backup, old_backup));

    FSL_CHECK_NOTHROW(fostlib::utf::save_file(filename, original));
    fostlib::string read1(fostlib::utf::load_file(filename));
    FSL_CHECK_EQ(read1, original);

    FSL_CHECK_NOTHROW(if (fostlib::fs::exists(backup))
                              fostlib::fs::remove(backup);
                      fostlib::fs::create_hard_link(filename, backup));
    fostlib::string read2(fostlib::utf::load_file(backup));
    FSL_CHECK_EQ(read2, original);

    FSL_CHECK_NOTHROW(fostlib::utf::save_file(temp, updated));
    fostlib::string read3(fostlib::utf::load_file(temp));
    FSL_CHECK_EQ(read3, updated);
    fostlib::string read4(fostlib::utf::load_file(filename));
    FSL_CHECK_EQ(read4, original);
    fostlib::string read5(fostlib::utf::load_file(backup));
    FSL_CHECK_EQ(read5, original);

    FSL_CHECK_NOTHROW(fostlib::fs::rename(temp, filename));
    fostlib::string read6(fostlib::utf::load_file(filename));
    FSL_CHECK_EQ(read6, updated);
    fostlib::string read7(fostlib::utf::load_file(backup));
    FSL_CHECK_EQ(read7, original);

    return 0;
}

/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "patch.hpp"
#include <fost/test>


FSL_TEST_SUITE(patch);


FSL_TEST_FUNCTION(empty_json) {
    fostlib::json empty;
    auto ops = beanbag::patch::operations(empty);
    FSL_CHECK_EQ(ops.size(), 0u);
}


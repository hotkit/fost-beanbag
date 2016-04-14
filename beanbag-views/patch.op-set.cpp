/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "patch.hpp"


namespace {


    const struct set : protected beanbag::patch::transform {
        set()
        : transform("op:set") {
        }

        transform_fn operator () (const fostlib::json &js) const {
            return transform_fn();
        }
    } c_set;


}


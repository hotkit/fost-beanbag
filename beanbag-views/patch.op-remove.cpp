/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "patch.hpp"


namespace {


    const struct remove : protected beanbag::patch::transform {
        remove() : transform("op:remove") {}

        transform_fn operator()(const fostlib::json &js) const {
            return [pos = fostlib::coerce<fostlib::jcursor>(js["path"])](
                           fostlib::jsondb::local &trans) {
                trans.remove(pos);
            };
        }
    } c_set;


}

/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "patch.hpp"
#include <fost/datetime>


namespace {


    const struct add : protected beanbag::patch::transform {
        add() : transform("op:add") {}

        transform_fn operator()(const fostlib::json &js) const {
            return [pos = fostlib::coerce<fostlib::jcursor>(js["path"]),
                    amount = fostlib::coerce<int64_t>(js["amount"])](
                           fostlib::jsondb::local &trans) {
                if (trans.has_key(pos)) {
                    const auto current = fostlib::coerce<int64_t>(trans[pos]);
                    trans.update(pos, current + amount);
                } else {
                    trans.insert(pos, amount);
                }
            };
        }
    } c_operation;


}

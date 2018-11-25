/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
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

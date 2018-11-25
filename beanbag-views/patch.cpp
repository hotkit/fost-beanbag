/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "patch.hpp"
#include "beanbag-views.hpp"
#include <f5/threading/map.hpp>
#include <fost/log>


namespace {
    auto &g_transformers() {
        static f5::tsmap<fostlib::string, beanbag::patch::transform *>
                c_transformers;
        return c_transformers;
    }

    beanbag::patch::transform_fn operation(const fostlib::json &op) {
        const auto opname = fostlib::coerce<fostlib::string>(op["!"]);
        const auto transformer = g_transformers().find(opname, nullptr);
        if (transformer) {
            return (*transformer)(op);
        } else {
            throw fostlib::exceptions::not_implemented(
                    __FUNCTION__, "Could not find operation", opname);
        }
    }
}


beanbag::patch::transforms beanbag::patch::operations(const fostlib::json &ops) {
    transforms lambdas;
    if (ops.isnull()) {
        // This is OK, there's just nothing to do
    } else if (ops.isobject()) {
        lambdas.push_back(operation(ops));
    } else if (ops.isarray()) {
        for (const auto &op : ops) { lambdas.push_back(operation(op)); }
    } else {
        throw fostlib::exceptions::not_implemented(
                __FUNCTION__, "Cannot deal with this type of PATCH opeation");
    }
    return lambdas;
}


/*
    beanbag::patch::transform
*/


beanbag::patch::transform::transform(fostlib::nliteral n) : name(n) {
    g_transformers().emplace_if_not_found(name, this);
}

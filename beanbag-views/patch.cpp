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
    f5::tsmap<f5::lstring, beanbag::patch::transform *> c_transformers;

    beanbag::patch::transform_fn operation(const fostlib::json &op) {
        auto transformer = c_transformers.find(fostlib::coerce<fostlib::string>(op["!"]));
        if ( transformer ) {
            return (*transformer)(op);
        } else {
            return beanbag::patch::transform_fn();
        }
    }
}


beanbag::patch::transforms beanbag::patch::operations(const fostlib::json &ops) {
    transforms lambdas;
    if ( ops.isobject() ) {
        lambdas.push_back(operation(ops));
    }
    return lambdas;
}


/*
    beanbag::patch::transform
*/


beanbag::patch::transform::transform(f5::lstring name) {
    c_transformers.insert_or_assign(name, this);
}


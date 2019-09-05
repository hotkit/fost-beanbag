/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once
#ifndef BEANBAG_PATH_HPP
#define BEANBAG_PATH_HPP


#include <fost/core>


namespace beanbag {


    /// Return a jcursor for the directory structure passed in
    fostlib::jcursor path_to_jcursor(const fostlib::string &pathname);


}


#endif // BEANBAG_PATH_HPP

/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
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

/*
    Copyright 2015 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once
#ifndef BEANBAG_JSONDB_PTR_HPP
#define BEANBAG_JSONDB_PTR_HPP


#include <fost/jsondb>


namespace beanbag {


    /// Shorter name for the jsondb pointer type we use
    typedef boost::shared_ptr<fostlib::jsondb> jsondb_ptr;


}


#endif // BEANBAG_JSONDB_PTR_HPP

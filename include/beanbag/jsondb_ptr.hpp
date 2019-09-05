/**
    Copyright 2015-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
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

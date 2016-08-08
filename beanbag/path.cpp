/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <beanbag/path.hpp>

#include <fost/string>
#include <fost/exception/parse_error.hpp>


fostlib::jcursor beanbag::path_to_jcursor(const fostlib::string &pathname) {
    fostlib::split_type path = fostlib::split(pathname, "/");
    fostlib::jcursor position;
    for ( fostlib::split_type::const_iterator part(path.begin());
            part != path.end(); ++part ) {
        try {
            int index = fostlib::coerce<int>(*part);
            position /= index;
        } catch ( fostlib::exceptions::parse_error& ) {
            position /= *part;
        }
    }
    return position;
}


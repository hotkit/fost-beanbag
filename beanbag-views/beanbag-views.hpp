/*
    Copyright 2014-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifdef FOST_OS_WINDOWS
#define FOST_BEANBAG_DECLSPEC __declspec(dllimport)
#define FOST_BEANBAG_VIEWS_DECLSPEC __declspec(dllexport)
#else
#define FOST_BEANBAG_DECLSPEC
#define FOST_BEANBAG_VIEWS_DECLSPEC
#endif


#include <fost/core>


namespace fostlib {
    extern const module c_fost_beanbag_views;
}

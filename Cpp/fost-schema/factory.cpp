/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/schema/factory.hpp>


using namespace fostlib;


fostlib::detail::factory_base::factory_base( const string &name )
: name( name ) {
}

fostlib::detail::factory_base::~factory_base() {
}

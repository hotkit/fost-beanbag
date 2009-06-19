/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/schema/factory.hpp>
#include <fost/thread.hpp>

#include <fost/exception/out_of_range.hpp>


using namespace fostlib;


namespace {
    typedef library< detail::factory_base * > factory_registry_type;
    factory_registry_type &g_registry() {
        static factory_registry_type registry;
        return registry;
    }
}


fostlib::detail::factory_base::factory_base( const string &name )
: name( name ) {
    g_registry().add( name, this );
}

fostlib::detail::factory_base::~factory_base() {
    g_registry().remove( name(), this );
}


const detail::factory_base &fostlib::find_factory( const string &name ) {
    factory_registry_type::found_t factories( g_registry().find( name ) );
    if ( factories.size() != 1 )
        throw exceptions::out_of_range< std::size_t >( L"Factory could not be found for name", 1, 1, factories.size() );
    return **factories.begin();
}

/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_FACTORY_HPP
#define FOST_FACTORY_HPP
#pragma once


#include <fost/schema.hpp>


namespace fostlib {


    namespace detail {


        class FOST_SCHEMA_DECLSPEC FSL_ABSTRACT factory_base {
        protected:
            factory_base( const string &name );
            virtual ~factory_base();

        public:
            accessors< string > name;
        };


    }


    template< typename I >
    class factory : public detail::factory_base {
    public:
        typedef I instance_type;

        factory()
        : factory_base( string( typeid( instance_type ).name() ) ) {
        }
        factory( const string &name )
        : factory_base( name ) {
        }
    };


    FOST_SCHEMA_DECLSPEC const detail::factory_base &find_factory( const string &name );


}


#endif // FOST_FACTORY_HPP

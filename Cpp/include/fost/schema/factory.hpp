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
#include <typeinfo>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>


namespace fostlib {


    namespace detail {


        class FOST_SCHEMA_DECLSPEC FSL_ABSTRACT factory_base {
        protected:
            factory_base( const std::type_info & );
            factory_base( const std::type_info &, const string &name );
            virtual ~factory_base();

        public:
            string name() const;

        private:
            nullable< string > m_name;
            const std::type_info &m_type;
        };


    }


    template< typename I >
    class factory : public detail::factory_base {
    public:
        typedef I instance_type;

        // This will fail if we try to create a factory for a sub-class of a model type_info
        // which does not itself inherit from model properly.
        BOOST_STATIC_ASSERT( ( boost::is_same<
            instance_type, typename instance_type::instance_type
        >::value ) );

        factory()
        : factory_base( typeid( instance_type ) ) {
        }
        factory( const string &name )
        : factory_base( typeid( instance_type ), name ) {
        }

        std::auto_ptr< instance_type > operator () ( const json &j ) {
            return std::auto_ptr< instance_type >( new instance_type( j ) );
        }
    };


    FOST_SCHEMA_DECLSPEC const detail::factory_base &find_factory( const string &name );


}


#endif // FOST_FACTORY_HPP

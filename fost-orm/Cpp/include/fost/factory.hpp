/*
    Copyright 1999-2010, Felspar Co Ltd. http://fost.3.felspar.com/
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
            factory_base( const enclosure &enc, const std::type_info &, const nullable< string > &name );
            factory_base( const factory_base &enc, const std::type_info &, const nullable< string > &name );

            virtual ~factory_base();

            const std::type_info &m_type;

        public:
            const enclosure &ns() const;
            string name() const;

            boost::shared_ptr< meta_instance > meta() const;

        private:
            typedef boost::variant< const enclosure *, const factory_base * > container_type;
            container_type m_container;
            nullable< string > m_name;
            mutable boost::shared_ptr< meta_instance > m_meta;
        };

        FOST_SCHEMA_DECLSPEC const factory_base &find_factory( const std::type_info &type );
        FOST_SCHEMA_DECLSPEC const factory_base &find_factory( const string &name );


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

        factory( const nullable< string > &name = null )
        : factory_base( enclosure::global, typeid( instance_type ), name ) {
        }
        factory( const enclosure &enc, const nullable< string > &name = null )
        : factory_base( enc, typeid( instance_type ), name ) {
        }
        factory( const factory_base &enc, const nullable< string > &name = null )
        : factory_base( enc, typeid( instance_type ), name ) {
        }

        std::auto_ptr< instance_type > operator () ( const json &j ) const {
            json js( j );
            ( jcursor() / L"_meta" / L"type_info" ).insert( js, string( m_type.name() ) );
            return std::auto_ptr< instance_type >( new instance_type( js ) );
        }
    };

    template< typename T >
    const factory< T > &find_factory() {
        return dynamic_cast< const factory< T > & >( detail::find_factory( typeid( T ) ) );
    }


}


#endif // FOST_FACTORY_HPP

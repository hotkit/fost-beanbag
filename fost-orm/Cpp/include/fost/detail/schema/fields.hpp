/*
    Copyright 1999-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_FIELDS_HPP
#define FOST_SCHEMA_FIELDS_HPP
#pragma once


#include <fost/core>
#include <typeinfo>


namespace fostlib {


    class meta_attribute;

    /// The base class for logical field types
    class FOST_SCHEMA_DECLSPEC field_base : boost::noncopyable {
        protected:
            /// Construct a field type and associate with a name
            field_base( const string &type_name );
            field_base(
                const string &type_name,
                const std::type_info &ti_value,
                const std::type_info &ti_nullable
            );
        public:
            /// Enable correct destruction for sub-classes
            virtual ~field_base();

            accessors< const string > type_name;

            /// Create an attribute for a model of this type
            virtual boost::shared_ptr< meta_attribute > meta_maker(
                const string &name, bool key, bool not_null,
                const nullable< std::size_t > &size, const nullable< std::size_t > &precision
            ) const = 0;

            /// Type describing the columns the field uses
            typedef std::vector<
                boost::shared_ptr< meta_attribute >
            > columns_type;
            /// Iterator type for the columns
            typedef columns_type::const_iterator const_iterator;
            /// Enable access to the start of the columns
            virtual const_iterator begin() const = 0;
            /// Enable access ot the end of the columns
            virtual const_iterator end() const = 0;

            /// Find a field implementation via a logical type name
            static const field_base &fetch( const string &type_name );
            /// Find a field implementation via a concrete type
            template< typename F >
            static const field_base &fetch() {
                return fetch( typeid(F) );
            }

        protected:
            /// An empty field sub structure whose iterators can be used to implement begin() and end()
            const static columns_type s_empty_substructure;

        private:
            const std::type_info * const m_ti_value, * const m_ti_nullable;
            static const field_base &fetch( const std::type_info &ti );
    };


}


#endif // FOST_SCHEMA_FIELDS_HPP

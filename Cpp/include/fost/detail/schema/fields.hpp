/*
    Copyright 1999-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_FIELDS_HPP
#define FOST_SCHEMA_FIELDS_HPP
#pragma once


#include <fost/core>


namespace fostlib {


    class meta_attribute;

    namespace detail {
        typedef std::vector< boost::shared_ptr< meta_attribute > > columns_type;
        FOST_SCHEMA_DECLSPEC extern const columns_type s_empty_substructure;
    }


    class FOST_SCHEMA_DECLSPEC field_base : boost::noncopyable {
        protected:
            typedef detail::columns_type columns_type;
            field_base( const string &type_name );
        public:
            virtual ~field_base();

            accessors< const string > type_name;

            virtual boost::shared_ptr< meta_attribute > meta_maker(
                const string &name, bool key, bool not_null,
                const nullable< std::size_t > &size, const nullable< std::size_t > &precision
            ) const = 0;

            typedef columns_type::const_iterator const_iterator;
            virtual const_iterator begin() const = 0;
            virtual const_iterator end() const = 0;

            static const field_base &fetch( const string &type_name );
    };


}


#endif // FOST_SCHEMA_FIELDS_HPP

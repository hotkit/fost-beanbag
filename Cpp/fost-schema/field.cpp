/*
    Copyright 2008-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/attributes.hpp>
#include <fost/datetime>
#include <fost/insert>
#include <fost/threading>

#include <fost/exception/null.hpp>
#include <fost/exception/out_of_range.hpp>


using namespace fostlib;


const field_wrapper< bool > booleanfield( "boolean" );

const field_wrapper< double > doublefield( L"float" );
const field_wrapper< int64_t > integerfield( L"integer" );

const field_wrapper< string > varchar( L"varchar" ), text( L"text" );

const field_wrapper< date > datefield( L"date" );
//const field_wrapper< time > timefield( L"time" );
const field_wrapper< timestamp > timestampfield( L"timestamp" );


namespace {
    typedef threadsafe_store< field_base* > registry_type;
    registry_type &registry() {
        static registry_type lib;
        return lib;
    }
}


const fostlib::field_base::columns_type fostlib::field_base::s_empty_substructure;


fostlib::field_base::field_base( const string &n )
: type_name( n), m_ti_value( NULL ), m_ti_nullable( NULL ) {
    registry().add( type_name(), this );
}
fostlib::field_base::field_base( const string &n, const std::type_info &ti_value, const std::type_info &ti_nullable )
: type_name( n ), m_ti_value( &ti_value ), m_ti_nullable( &ti_nullable ) {
    registry().add( type_name(), this );
    registry().add( string(m_ti_value->name()), this );
    registry().add( string(m_ti_nullable->name()), this );
}
fostlib::field_base::~field_base() {
    registry().remove( type_name(), this );
    if ( m_ti_value )
        registry().remove( string(m_ti_value->name()), this );
    if ( m_ti_nullable )
        registry().remove( string(m_ti_nullable->name()), this );
}

const field_base &fostlib::field_base::fetch(const string &n) {
    try {
        registry_type::found_t f = registry().find( n );
        if ( f.size() != 1 ) {
            throw exceptions::out_of_range< std::size_t >(
                "There should only be one field of the given type found", 1, 1, f.size() );
        } else {
            return **f.begin();
        }
    } catch ( exceptions::exception &e ) {
        insert(e.data(), "fetching-field_base", "type", n);
        throw;
    }
}

const field_base &fostlib::field_base::fetch( const std::type_info &ti ) {
    return fetch( string(ti.name()) );
}

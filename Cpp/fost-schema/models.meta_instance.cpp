/*
    Copyright 1999-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/schema.hpp>
#include <fost/db.hpp>

#include <fost/exception/null.hpp>
#include <fost/exception/no_attribute.hpp>


using namespace fostlib;


namespace {
    boost::shared_ptr< meta_attribute > make_attribute(
        const string &name, const string &type, bool key, bool not_null,
        const nullable< std::size_t > &size, const nullable< std::size_t > &precision
    ) {
        boost::shared_ptr< meta_attribute > attr(  field_base::fetch( type ).meta_maker(
            name, key, not_null, size, precision
        ) );
        return attr;
    }
    template< typename const_iterator >
    const_iterator find_attr( const_iterator p, const const_iterator end, const string &n ) {
        for( ; p != end; ++p )
            if ( (*p)->name() == n )
                return p;
        return end;
    }
}

fostlib::meta_instance::meta_instance( const string &n )
: enclosure( n ) {
}
fostlib::meta_instance::meta_instance( const enclosure &e, const string &n )
: enclosure( e, n ) {
}


meta_instance &fostlib::meta_instance::primary_key(
    const string &name, const string &type,
    const nullable< std::size_t > &size, const nullable< std::size_t > &precision
) {
    if ( find_attr( m_columns.begin(), m_columns.end(), name ) != m_columns.end() )
        throw exceptions::null( L"Cannot have two attributes with the same name" );
    m_columns.push_back( make_attribute( name, type, true, true, size, precision ) );
    return *this;
}
meta_instance &fostlib::meta_instance::field(
    const string &name, const string &type, bool not_null,
    const nullable< std::size_t > &size, const nullable< std::size_t > &precision
) {
    if ( find_attr( m_columns.begin(), m_columns.end(), name ) != m_columns.end() )
        throw exceptions::null( L"Cannot have two attributes with the same name" );
    m_columns.push_back( make_attribute( name, type, false, not_null, size, precision ) );
    return *this;
}


namespace {
    struct object_reference_attribute : public meta_attribute {
        object_reference_attribute(const string &name, const field_base &type, bool key, bool not_null)
        : meta_attribute( name, type, key, not_null, null, null ) {
        }
        boost::shared_ptr< attribute_base > construct() const {
            throw exceptions::not_implemented("object_reference_attribute::construct() const");
        }
        boost::shared_ptr< attribute_base > construct(const json &) const {
            throw exceptions::not_implemented("object_reference_attribute::construct(const json &) const");
        }
    };
    struct object_reference_field : public field_base {
        columns_type sub_structure;
        object_reference_field(const string &type_name, const meta_instance &mi)
        : field_base(type_name) {
            for ( meta_instance::const_iterator i(mi.begin()); i != mi.end(); ++i )
                if ( (*i)->key() )
                    sub_structure.push_back(*i);
        }
        boost::shared_ptr< meta_attribute > meta_maker(
            const string &name, bool key, bool not_null,
            const nullable< std::size_t > &size, const nullable< std::size_t > &precision
        ) const {
            return boost::shared_ptr< meta_attribute >( new object_reference_attribute(
                name, *this, key, not_null
            ) );
        }
        const_iterator begin() const {
            return sub_structure.begin();
        }
        const_iterator end() const {
            return sub_structure.end();
        }
    };
}
const field_base &fostlib::meta_instance::type() const {
    if ( !m_type.get() )
        m_type.reset( new object_reference_field( fq_name(), *this ) );
    return *m_type;
}
meta_instance &fostlib::meta_instance::primary_key(
    const string &name, const meta_instance &mi
) {
    if ( find_attr( m_columns.begin(), m_columns.end(), name ) != m_columns.end() )
        throw exceptions::null( L"Cannot have two attributes with the same name" );
    m_columns.push_back( mi.type().meta_maker(name, true, true, null, null) );
    return *this;
}
meta_instance &fostlib::meta_instance::field(
    const string &name, const meta_instance &mi, bool not_null
) {
    if ( find_attr( m_columns.begin(), m_columns.end(), name ) != m_columns.end() )
        throw exceptions::null( L"Cannot have two attributes with the same name" );
    m_columns.push_back( mi.type().meta_maker(name, false, not_null, null, null) );
    return *this;
}


const meta_attribute &fostlib::meta_instance::operator[] ( const string &n ) const {
    columns_type::const_iterator p( find_attr( m_columns.begin(), m_columns.end(), n ) );
    if ( p != m_columns.end() )
        return **p;
    else
        throw exceptions::null( L"Could not find attribute definition", n );
}


std::unique_ptr<instance> fostlib::meta_instance::create() const {
    return create( json() );
}
std::unique_ptr<instance> fostlib::meta_instance::create( const json &j ) const {
    return std::make_unique<instance>(*this, j);
}

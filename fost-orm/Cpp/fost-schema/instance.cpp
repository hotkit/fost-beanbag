/*
    Copyright 1999-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/schema.hpp>
#include <fost/db.hpp>

#include <fost/exception/null.hpp>
#include <fost/exception/no_attribute.hpp>

#include <boost/lambda/lambda.hpp>


using namespace fostlib;


/*
    fostlib::instance
*/

fostlib::instance::instance( const meta_instance &meta, const json &v )
: m_in_database( false ), m_to_die( false ), m_meta( meta ) {
    for ( meta_instance::const_iterator col( meta.begin() ); col != meta.end(); ++col )
        if ( v.has_key( (*col)->name() ) )
            attribute( (*col)->construct( v[ (*col)->name() ] ) );
        else
            attribute( (*col)->construct() );
}

void fostlib::instance::attribute( boost::shared_ptr< attribute_base > attr ) {
    m_attributes.insert( std::make_pair( attr->_meta().name(), attr ) );
}

const meta_instance &fostlib::instance::_meta() const {
    return m_meta;
}
attribute_base &fostlib::instance::operator [] ( const string &name ) {
    attributes_type::iterator p( m_attributes.find( name ) );
    if ( p == m_attributes.end() )
        throw exceptions::not_implemented( _meta().name() + L"." + name );
    else
        return *p->second;
}

void fostlib::instance::save( fostlib::dbtransaction &t ) {
    if ( m_in_database )
        throw exceptions::not_implemented(
            "fostlib::instance::save() -- when already in database");
    else
        t.insert( *this, boost::lambda::var( m_in_database ) = true );
}


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
    fostlib::enclosure
*/


const enclosure enclosure::global("");

fostlib::enclosure::enclosure( const string &n )
: name( n ), m_parent( global ) {
}
fostlib::enclosure::enclosure( const enclosure &e, const string &n )
: name( n ), m_parent( e ) {
}

bool fostlib::enclosure::in_global() const {
    return &m_parent == &global;
}

string fostlib::enclosure::fq_name( const string &delim ) const {
    if ( !in_global() )
        return m_parent.fq_name( delim ) + delim + name();
    else
        return name();
}
const enclosure &fostlib::enclosure::parent() const {
    return m_parent;
}

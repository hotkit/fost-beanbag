/*
    Copyright 1999-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/db.hpp>

#include <fost/insert>
#include <fost/push_back>


fostlib::exceptions::data_driver::data_driver( const string&m, const string &d ) throw ()
: exception( m ) {
    fostlib::insert(data(), "driver", d);
}
fostlib::exceptions::data_driver::data_driver(const string &m, const string &d1, const string &d2) throw ()
: exception( m ) {
    fostlib::push_back(data(), "driver", d1);
    fostlib::push_back(data(), "driver", d2);
}
fostlib::wliteral const fostlib::exceptions::data_driver::message() const throw () {
    return L"Problem with database driver.";
}


#include <fost/exception/no_attribute.hpp>
fostlib::exceptions::no_attribute::no_attribute( const string &attribute ) throw ()
: exception( L"Missing attribute" ) {
    try {
        fostlib::insert(data(), "attribute", attribute);
    } catch ( ... ) {
        absorb_exception();
    }
}
fostlib::wliteral const fostlib::exceptions::no_attribute::message() const throw () {
    return L"Attribute could not be found on the instance";
}


#include <fost/exception/query_failure.hpp>
fostlib::exceptions::query_failure::query_failure( const string &m, const fostlib::meta_instance &i ) throw ()
: exception( m ) {
    try {
        fostlib::insert(data(), "meta-instance", i.fq_name());
    } catch ( ... ) {
        absorb_exception();
    }
}
fostlib::wliteral const fostlib::exceptions::query_failure::message() const throw () {
    return L"Problem executing query";
}


#include <fost/exception/transaction_fault.hpp>
fostlib::exceptions::transaction_fault::transaction_fault( const string &error ) throw ()
: exception( error ) {
}
fostlib::wliteral const fostlib::exceptions::transaction_fault::message() const throw () {
    return L"Fault in transaction handling";
}


/*
    Copyright 1999-2018, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb.hpp"
#include <fost/exception/transaction_fault.hpp>


fostlib::exceptions::transaction_fault::transaction_fault(
        const string &error) throw()
: exception(error) {}
fostlib::wliteral const fostlib::exceptions::transaction_fault::message() const
        throw() {
    return L"Fault in transaction handling";
}

/**
    Copyright 1999-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
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

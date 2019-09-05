/**
    Copyright 1999-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#ifndef FOST_EXCEPTION_TRANSACTION_FAULT_HPP
#define FOST_EXCEPTION_TRANSACTION_FAULT_HPP
#pragma once


#include <fost/core>


namespace fostlib {


    namespace exceptions {


        class FOST_JSONDB_DECLSPEC transaction_fault : public exception {
          public:
            transaction_fault(const string &error) throw();

          protected:
            wliteral const message() const throw();
        };


    }


}


#endif // FOST_EXCEPTION_TRANSACTION_FAULT_HPP

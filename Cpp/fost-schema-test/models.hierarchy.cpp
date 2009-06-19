/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/factory>


using namespace fostlib;


FSL_TEST_SUITE( basic_hierarchy );


class BasicModel : public model< BasicModel > {
public:
    FSL_MODEL_CONSTRUCTOR( BasicModel );
};


class BasicSubModel : public model< BasicSubModel, BasicModel > {
public:
    FSL_MODEL_CONSTRUCTOR( BasicSubModel );
};


class HostModel : public model< HostModel > {
public:
    FSL_MODEL_CONSTRUCTOR( HostModel );

    class NestedModel : public model< NestedModel > {
    public:
        FSL_MODEL_CONSTRUCTOR( NestedModel );
    };
};


class SubHostModel : public model< SubHostModel, HostModel > {
public:
    FSL_MODEL_CONSTRUCTOR( SubHostModel );
};

/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/factory>

#include <fost/exception/out_of_range.hpp>


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


const fostlib::factory< BasicModel > s_BasicModel1;
const fostlib::factory< BasicModel > s_BasicModel2( L"BasicModel" );
const fostlib::factory< BasicSubModel > s_BasicSubModel( L"BasicSubModel" );


FSL_TEST_FUNCTION( factories ) {
    FSL_CHECK_EQ( s_BasicSubModel.name(), L"BasicSubModel" );
    FSL_CHECK_EQ( &s_BasicSubModel, &find_factory( L"BasicSubModel" ) );
    FSL_CHECK_EXCEPTION( find_factory( L"No model factory" ), fostlib::exceptions::out_of_range< std::size_t >& );
    FSL_CHECK_EXCEPTION( find_factory( s_BasicModel1.name() ), fostlib::exceptions::out_of_range< std::size_t >& );
}


FSL_TEST_FUNCTION( construction ) {
    std::auto_ptr< BasicModel > bm = s_BasicModel2( fostlib::json() );
    FSL_CHECK( bm.get() );
}

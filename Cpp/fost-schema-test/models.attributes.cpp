/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/factory>


using namespace fostlib;


FSL_TEST_SUITE( basic_attributes );


namespace {
    class SimpleModel : public model< SimpleModel > {
    public:
        SimpleModel( const fostlib::json &j )
        : model_type( j ),
            pk( this, j ),
            display_name( this, j ) {
        }
        FSL_ATTRIBUTE_PK( pk, int64_t );
        FSL_ATTRIBUTE_NULL( display_name, string );
    };
    fostlib::factory< SimpleModel > s_factory_sm;


    class SubModel : public model< SubModel, SimpleModel > {
    public:
        SubModel( const fostlib::json &j )
        : model_type( j ),
            name( this, j ) {
        }
        FSL_ATTRIBUTE_NOT_NULL( name, string );
    };
    fostlib::factory< SubModel > s_factory_sub(L"SubModel");
}


FSL_TEST_FUNCTION( meta ) {
    boost::shared_ptr< meta_instance > simple = s_factory_sm.meta();
    FSL_CHECK( simple->name().length() ); // We don't know what the name will be, but there should be one

    boost::shared_ptr< meta_instance > submodel = s_factory_sub.meta();
    FSL_CHECK_EQ( submodel->name(), "SubModel" );
}


FSL_TEST_FUNCTION( constructors ) {
    std::auto_ptr< SimpleModel > sm = s_factory_sm( fostlib::json() );
    //FSL_CHECK_EQ( sm->pk(), 0 );
}

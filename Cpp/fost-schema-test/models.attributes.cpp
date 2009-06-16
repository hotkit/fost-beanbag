/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/db>


using namespace fostlib;


FSL_TEST_SUITE( basic_attributes );


class SimpleModel : public model< SimpleModel > {
public:
    FSL_CONSTRUCTOR( SimpleModel );
    FSL_ATTRIBUTE_PK( pk, int64_t );
    FSL_ATTRIBUTE_NULL( display_name, string );
};
FSL_MODEL( SimpleModel )( L"SimpleModel" );
FSL_ATTRIBUTE( SimpleModel, pk );
FSL_ATTRIBUTE( SimpleModel, display_name );


class SubModel : public model< SubModel, SimpleModel > {
public:
    FSL_CONSTRUCTOR( SubModel );
    FSL_ATTRIBUTE_NOT_NULL( name, string );
};
FSL_MODEL( SubModel )( L"SubModel" );
FSL_ATTRIBUTE( SubModel, name );


FSL_TEST_FUNCTION( base_attribute ) {
    FSL_CHECK_EQ( ( SimpleModel::attribute< SimpleModel::pk_tag, int64_t, model_base::a_primary >::binding.name() ), L"pk" );
    FSL_CHECK_NOTHROW( SimpleModel::_meta()[ L"pk" ] );

    FSL_CHECK_EQ( ( SimpleModel::attribute< SimpleModel::display_name_tag, string, model_base::a_nullable >::binding.name() ), L"display_name" );
    FSL_CHECK_NOTHROW( SimpleModel::_meta()[ L"display_name" ] );
}

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


namespace {

    class BasicModel : public model< BasicModel > {
    public:
        BasicModel( const fostlib::json &j )
        : model_type( j ) {
        }
    };


    class BasicSubModel : public model< BasicSubModel, BasicModel > {
    public:
        BasicSubModel( const fostlib::json &j )
        : model_type( j ) {
        }
    };


    class HostModel : public model< HostModel > {
    public:
        HostModel( const fostlib::json &j )
        : model_type( j ) {
        }

        class NestedModel : public model< NestedModel > {
        public:
            NestedModel( const fostlib::json &j )
            : model_type( j ) {
            }
        };
    };


    class SubHostModel : public model< SubHostModel, HostModel > {
    public:
        SubHostModel( const fostlib::json &j )
        : model_type( j ) {
        }
    };


    fostlib::enclosure s_anon( L"models_hierarchy_cpp" );

    const fostlib::factory< BasicModel > s_BasicModel1( s_anon );
    const fostlib::factory< BasicModel > s_BasicModel2( s_anon, L"BasicModel" );
    const fostlib::factory< BasicSubModel > s_BasicSubModel( s_anon, L"BasicSubModel" );
    const fostlib::factory< HostModel > s_HostModel( s_anon, L"HostModel" );
    const fostlib::factory< HostModel::NestedModel > s_NestedModel( s_HostModel, L"NestedModel" );
    const fostlib::factory< SubHostModel > s_SubHostModel( s_anon, L"SubHostModel" );

}


FSL_TEST_FUNCTION( factories ) {
    FSL_CHECK_EQ( s_BasicSubModel.name(), L"BasicSubModel" );

    FSL_CHECK_EQ( &s_BasicSubModel, &detail::find_factory( L"BasicSubModel" ) );
    FSL_CHECK_EQ( &s_BasicSubModel, &detail::find_factory( typeid( BasicSubModel ) ) );
    FSL_CHECK_EQ( &s_BasicSubModel, &find_factory< BasicSubModel >() );

    FSL_CHECK_EXCEPTION( detail::find_factory( L"No model factory" ), fostlib::exceptions::out_of_range< std::size_t >& );
    FSL_CHECK_EXCEPTION( detail::find_factory( s_BasicModel1.name() ), fostlib::exceptions::out_of_range< std::size_t >& );

    FSL_CHECK_EQ( s_BasicModel1.ns().name(), s_anon.name() );
}


FSL_TEST_FUNCTION( construction ) {
    std::auto_ptr< BasicSubModel > bm = s_BasicSubModel( fostlib::json() );
    FSL_CHECK( bm.get() );

    FSL_CHECK_EQ( &bm->_instance()._meta(), s_BasicSubModel.meta().get() );
}

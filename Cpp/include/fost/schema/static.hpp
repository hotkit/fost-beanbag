/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_STATIC_HPP
#define FOST_SCHEMA_STATIC_HPP
#pragma once


#include <fost/schema/dynamic.hpp>
#include <fost/thread.hpp>


namespace fostlib {


    /*
        The base class for all model hierarchies
    */
    class FOST_SCHEMA_DECLSPEC model_base {
        boost::shared_ptr< instance > m_instance;
    public:
        // Base type for the attributes
        typedef enum { a_primary, a_nullable, a_required } attribute_meta;
        template< typename tag >
        class attribute_base;

        // Tag types used to build meta data for attributes
        struct tag_base; struct primary_tag; struct nullable_tag; struct required_tag;

        model_base( const json &j );
        virtual ~model_base();

        instance &_instance();
    };


    // Tag types
    struct FSL_ABSTRACT model_base::tag_base {
        virtual model_base::attribute_meta stereotype() const = 0;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::primary_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::nullable_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::required_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
    };


    /*
        This handles the case for model types that inherit from other model type.
        There are two specialisations -- one where there is no superclass and one where
        there is a superclass model.
    */
    template< typename instance_type, typename superclass_type = t_null >
    class model;

    // Where there is no model superclass
    template< typename I >
    class model< I, t_null > : public model_base {
    public:
        typedef I instance_type;
        typedef model< instance_type > model_type;

        model( const json &j )
        : model_base( j ) {
        }

        // This describes an attribute. This ensures that every attribute has its own C++ type
        template< typename tag, typename type, model_base::attribute_meta stereotype >
        class attribute /*: public attribute_base< tag >*/ {
        };
    };

    // Where there is a model superclass
    template< typename I, typename superclass_type >
    class model : public superclass_type {
    public:
        typedef I instance_type;
        typedef model< instance_type, superclass_type > model_type;

        model( const json &j )
        : superclass_type( j ) {
        }
    };


}


#endif // FOST_SCHEMA_STATIC_HPP

/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_STATIC_HPP
#define FOST_SCHEMA_STATIC_HPP
#pragma once


#include "dynamic.hpp"
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

        // Tag types used to build meta data for attributes
        struct tag_base; struct primary_tag; struct nullable_tag; struct required_tag;

        model_base( const json &j );
        virtual ~model_base();

        instance &_instance();
    };


    // Tag types
    struct FSL_ABSTRACT model_base::tag_base {
        virtual model_base::attribute_meta stereotype() const = 0;
        virtual ~tag_base();
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


    /// This handles the case for model types that inherit from other model type. There are two specialisations -- one where there is no superclass and one where there is a superclass model.
    template< typename instance_type, typename superclass_type = t_null >
    class model;

    /// Specialisation where there is no model superclass
    template< typename I >
    class model< I, t_null > : public model_base {
    public:
        typedef I instance_type;
        typedef model< instance_type > model_type;

        model( const json &j )
        : model_base( j ) {
        }

        /// This stores an instance of the underlying data type within a dynamic model
        template< typename actual_type >
        struct _attribute_storage : public attribute_base {
            actual_type m_value;
        public:
            _attribute_storage( model_base *instance, const json &init )
            : attribute_base(), m_value() {
            }

            json to_json() const {
                return coerce< json >( m_value );
            }
            void from_json( const json &j ) {
                m_value = coerce< actual_type >( j );
            }
        };

        /// This works out the correct actual storage type based on the logical type and the stereotype. Both PK and required use the logical type, nullable wraps it in a nullable
        template< typename logical_type, typename stereotype >
        struct _attribute_storage_specifier :
            public _attribute_storage< logical_type >
        {
            _attribute_storage_specifier( model_base *i, const json &j )
            : _attribute_storage< logical_type >( i, j ) {
            }
        };
        template< typename logical_type >
        struct _attribute_storage_specifier< logical_type, model_base::nullable_tag > :
            public _attribute_storage< nullable< logical_type > >
        {
            _attribute_storage_specifier( model_base *i, const json &j )
            : _attribute_storage< nullable< logical_type > >( i, j ) {
            }
        };

        /// This fetches the required meta-data from the attribute's tag type
        template< typename tag >
        struct _attribute : public _attribute_storage_specifier<
            typename tag::logical_attribute_type, typename tag::stereotype_tag
        > {
            _attribute( model_base *instance, const json &init )
            : _attribute_storage_specifier<
                typename tag::logical_attribute_type, typename tag::stereotype_tag
            >( instance, init ) {
            }

            const meta_attribute &_meta() const {
                throw exceptions::not_implemented( "_attribute::_meta() const" );
            }

            typename tag::logical_attribute_type operator () () const {
                throw exceptions::not_implemented( "_attribute::operator () () const" );
            }
        };
    };

    /// Specialisation for where there is a model superclass
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

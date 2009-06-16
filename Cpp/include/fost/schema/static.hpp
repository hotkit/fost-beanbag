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
        // Stores the basic meta data
        struct factory_base;
        struct attribute_binding_base;

        // Base type for the attributes
        typedef enum { a_primary, a_nullable, a_required } attribute_meta;
        template< typename tag >
        class attribute_base;

        // Tag types used to build meta data for attributes
        struct tag_base; struct primary_tag; struct nullable_tag; struct required_tag;

        model_base( const factory_base &factory, dbconnection &dbc, const json &j );
        virtual ~model_base();

        instance &_instance();
    };


    // Stores the binding between an attribute type and it's name
    struct FOST_SCHEMA_DECLSPEC FSL_ABSTRACT model_base::attribute_binding_base {
        attribute_binding_base( const factory_base &factory, const string &name );

        virtual const model_base::tag_base &stereotype() const = 0;
        accessors< string > name;
    };


    // Base types for attributes
    template< typename tag >
    class model_base::attribute_base {
    public:
        static const struct attribute_binding : public model_base::attribute_binding_base {
            attribute_binding( const factory_base &factory, const string &name )
            : attribute_binding_base( factory, name ) {
            }
            const model_base::tag_base &stereotype() const {
                return m_stereotype;
            }
        private:
            typename tag::stereotype_tag m_stereotype;
        } binding;
    };


    // Factories are used to create instances of models
    struct FOST_SCHEMA_DECLSPEC model_base::factory_base {
        factory_base( const string &name );
        factory_base( const enclosure &enc, const string &name );
        factory_base( const factory_base &enc, const string &name );

        const enclosure &ns() const;
        accessors< string > name;

        virtual boost::shared_ptr< meta_instance > _meta() const;

    private:
        typedef boost::variant< const enclosure *, const factory_base * > container_type;
        container_type m_container;
        mutable boost::shared_ptr< meta_instance > m_meta;

        friend struct attribute_binding_base;
        typedef library< const attribute_binding_base* > attributes_type;
        mutable attributes_type m_attributes;
    };


    // Tag types
    struct FSL_ABSTRACT model_base::tag_base {
        virtual model_base::attribute_meta stereotype() const = 0;
        virtual meta_instance &describe(
            meta_instance &meta,
            const model_base::attribute_binding_base &binding
        ) const = 0;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::primary_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
        meta_instance &describe(
            meta_instance &meta,
            const model_base::attribute_binding_base &binding
        ) const;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::nullable_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
        meta_instance &describe(
            meta_instance &meta,
            const model_base::attribute_binding_base &binding
        ) const;
    };
    struct FOST_SCHEMA_DECLSPEC model_base::required_tag : public model_base::tag_base {
        model_base::attribute_meta stereotype() const;
        meta_instance &describe(
            meta_instance &meta,
            const model_base::attribute_binding_base &binding
        ) const;
    };


    /*
        This handles the case for model types that inherit from other model type.
        There are two specialisations -- one where there is no superclass and one where
        there is a superclass model.
    */
    template< typename model_type, typename superclass_type = t_null >
    class model;

    // Where there is no model superclass
    template< typename model_type >
    class model< model_type, t_null > : public model_base {
    public:
        typedef model< model_type > superclass;

        struct factory : public model_base::factory_base {
            factory( const string &name ) : factory_base( name ) {}
            template< typename E >
            factory( const E &enc, const string &name ) : factory_base( enc, name ) {}

            boost::shared_ptr< model_type > operator () ( dbconnection &dbc, const json &j ) const {
                return boost::shared_ptr< model_type >(
                    new model_type( dynamic_cast< const typename model_type::factory& >( *this ), dbc, j )
                );
            }
        };

        static const factory s_factory;
        static const meta_instance &_meta() {
            return *s_factory._meta();
        }

        model( const factory &f, dbconnection &dbc, const json &j )
        : model_base( f, dbc, j ) {
        }

        // This describes an attribute. This ensures that every attribute has its own C++ type
        template< typename tag, typename type, model_base::attribute_meta stereotype >
        class attribute : public attribute_base< tag > {
        };
    };

    // Where there is a model superclass
    template< typename model_type, typename superclass_type >
    class model : public superclass_type {
    public:
        typedef model< model_type, superclass_type > superclass;

        struct factory : public superclass_type::factory {
            factory( const string &name ) : superclass_type::factory( name ) {}
            template< typename E >
            factory( const E &enc, const string &name ) : superclass_type::factory( enc, name ) {}

            boost::shared_ptr< model_type > operator () ( dbconnection &dbc, const json &j ) const {
                return boost::shared_ptr< model_type >(
                    new model_type( dynamic_cast< const typename model_type::factory& >( *this ), dbc, j )
                );
            }

            boost::shared_ptr< meta_instance > _meta() const {
                boost::shared_ptr< meta_instance > m = superclass_type::factory::_meta();
                m->superclasses().push_back( superclass_type::s_factory._meta() );
                return m;
            }
        };

        static const factory s_factory;
        static const meta_instance &_meta() {
            return *s_factory._meta();
        }

        model( const factory &f, dbconnection &dbc, const json &j )
        : superclass_type( f, dbc, j ) {
        }
    };


}


// Used to define the constructors
#define FSL_CONSTRUCTOR( model ) \
    typedef model this_model_type; \
    model( const factory &f, fostlib::dbconnection &d, const fostlib::json &j ) \
    : superclass( f, d, j ) {}

// Used to declare attributes in the models
#define FSL_ATTRIBUTE_PK( name, type ) \
    struct name##_tag { typedef this_model_type model_type; typedef model_base::primary_tag stereotype_tag; }; \
    attribute< name##_tag, type, fostlib::model_base::a_primary > name;
#define FSL_ATTRIBUTE_NOT_NULL( name, type ) \
    struct name##_tag { typedef this_model_type model_type; typedef model_base::required_tag stereotype_tag; }; \
    attribute< name##_tag, type, fostlib::model_base::a_required > name;
#define FSL_ATTRIBUTE_NULL( name, type ) \
    struct name##_tag { typedef this_model_type model_type; typedef model_base::nullable_tag stereotype_tag; }; \
    attribute< name##_tag, type, fostlib::model_base::a_nullable > name;

// Static creation of the model binding
#define FSL_MODEL( name ) template<> const name::factory name::superclass::s_factory
#define FSL_ATTRIBUTE( model, name ) template <> const \
    fostlib::model_base::attribute_base< model::name##_tag >::attribute_binding \
        fostlib::model_base::attribute_base< model::name##_tag >::binding( model::s_factory, fostlib::string( #name ) )


#endif // FOST_SCHEMA_STATIC_HPP

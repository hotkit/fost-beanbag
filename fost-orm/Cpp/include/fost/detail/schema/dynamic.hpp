/*
    Copyright 1999-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_DYNAMIC_HPP
#define FOST_SCHEMA_DYNAMIC_HPP
#pragma once


#include "fields.hpp"


namespace fostlib {


    class attribute_base;


    /// mata_attributes describe the type of a field in a fairly generic way.
    class FOST_SCHEMA_DECLSPEC meta_attribute : boost::noncopyable {
    public:
        /// Constructs a meta_attribute
        meta_attribute(
            const string &name, const field_base &type,
            bool key, bool required,
            const nullable< std::size_t > &size,
            const nullable< std::size_t > &precision
        );
        /// Enable proper sub-classing support
        virtual ~meta_attribute();

        /// The name of the attribute
        accessors< const string > name;
        /// The implementation of the logical type
        const field_base &type() const;
        /// Whether this attribute is part of the primary key
        accessors< const bool > key;
        /// Whether this attribute is required
        accessors< const bool > required;
        /// The size of the value stored in the attribute
        accessors< const nullable< std::size_t > > size;
        /// The precision of the value stored in the attribute
        accessors< const nullable< std::size_t > > precision;

        /// Construct the attribute that actually holds a value
        virtual boost::shared_ptr< attribute_base > construct() const = 0;
        /// Construct the attribute that actually holds a value initialising it with a given JSON value
        virtual boost::shared_ptr< attribute_base > construct( const json & ) const = 0;

    private:
        const field_base &m_type;
    };


    /// An enclosure is a representation of a namespace.
    class FOST_SCHEMA_DECLSPEC enclosure : boost::noncopyable {
    public:
        explicit enclosure( const string &name );
        enclosure( const enclosure &enc, const string &name );
        virtual ~enclosure() {}

        accessors< const string > name;
        string fq_name( const string &delim = L"." ) const;
        const enclosure &parent() const;

        bool in_global() const;

        static const enclosure global;

    private:
        const enclosure &m_parent;
    };


    class instance;
    class dbconnection;
    class dbtransaction;


    /// The meta_instance is a type descriptor which describes what the layout of the instances will look like.
    class FOST_SCHEMA_DECLSPEC meta_instance : public enclosure {
        typedef std::vector< boost::shared_ptr< meta_attribute > > columns_type;
        typedef std::vector< boost::shared_ptr< meta_instance > > typelist_type;
    public:
        explicit meta_instance( const string &name );
        meta_instance( const enclosure &enc, const string &name );

        const meta_attribute &operator[]( const string &name ) const;

        accessors< typelist_type, lvalue > superclasses;
        const field_base &type() const;

        meta_instance &primary_key(
            const string &name, const string &type,
            const nullable< std::size_t > & size = null, const nullable< std::size_t > &precision = null
        );
        meta_instance &primary_key(
            const string &name, const meta_instance &type
        );
        meta_instance &field(
            const string &name, const string &type, bool not_null,
            const nullable< std::size_t > & size = null, const nullable< std::size_t > &precision = null
        );
        meta_instance &field(
            const string &name, const meta_instance &type, bool not_null
        );

        std::auto_ptr< instance > create() const;
        std::auto_ptr< instance > create( const json & ) const;

        typedef columns_type::const_iterator const_iterator;
        const_iterator begin() const { return m_columns.begin(); }
        const_iterator end() const { return m_columns.end(); }

    private:
        columns_type m_columns;
        mutable boost::scoped_ptr< field_base > m_type;
    };


    /// attribute_base is the base class for the attributes that the O/RM knows about
    class FOST_SCHEMA_DECLSPEC attribute_base : boost::noncopyable {
    protected:
        attribute_base();
    public:
        virtual ~attribute_base() {}

        virtual json to_json() const = 0;
        virtual void from_json( const json & ) = 0;

        virtual const meta_attribute &_meta() const = 0;
    };


    /// An instance is the representation of the persistent object in memory
    class FOST_SCHEMA_DECLSPEC instance : boost::noncopyable {
        friend class meta_instance;
    public:
        instance( const meta_instance &, const json &j );

        typedef std::vector< boost::shared_ptr< attribute_base > > key_type;

        void attribute( boost::shared_ptr< attribute_base > attr );
        virtual ~instance() {}

        const meta_instance &_meta() const;

        attribute_base &operator []( const string &name );
        const attribute_base &operator []( const string &name ) const {
            instance *self = const_cast< instance * >( this );
            return (*self)[ name ];
        }

        bool in_database() const { return m_in_database; }
        void save( dbtransaction & );

    private:
        bool m_in_database, m_to_die;
        const meta_instance &m_meta;
        typedef std::map< string, boost::shared_ptr< attribute_base > > attributes_type;
        attributes_type m_attributes;
    };


}


#endif // FOST_SCHEMA_DYNAMIC_HPP

/*
    Copyright 1998-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_CACHE_HPP
#define FOST_CACHE_HPP
#pragma once


#include <fost/db>
#include <boost/thread.hpp>
#include <fost/detail/cache-d.hpp>


namespace fostlib {


    /*
        The object cache provides a protocol that can be used to store object
        instances inside a mastercache or fostcache
    */
    template< typename O, typename K = typename O::key_type >
    class objectcache : public detail::objectcache_base {
    public:
        typedef O instance_type;
        typedef K key_type;

    private:
        std::map< key_type, boost::shared_ptr< instance_type > > m_objects;
    };


    /*
        A mastercache allows for objects to be stored for long periods. A mastercache
        is shared between many threads and there must only be a single mastercache
        per database connection in any given process.
    */
    class FOST_CACHE_DECLSPEC mastercache : boost::noncopyable {
        std::map<
            boost::shared_ptr< meta_instance >,
            boost::shared_ptr< detail::objectcache_base >
        > m_caches;
    public:
        explicit mastercache( dbconnection &dbc );
        virtual ~mastercache();

        mastercache &type( boost::shared_ptr< fostlib::meta_instance > type );
        virtual const fostlib::meta_instance &operator [] ( const string & ) const;

        dbconnection &connection() { return m_dbc; }
        const dbconnection &connection() const { return m_dbc; }

    protected:
        typedef std::map< string, boost::shared_ptr< fostlib::meta_instance > > type_registry_collection;
        type_registry_collection m_types;
        dbconnection &m_dbc;
    };


    /*
        The fostcaches are used in a thread to manage the objects that the thread may change.
    */
    class FOST_CACHE_DECLSPEC fostcache :
        private detail::fostcache_dbc,
        public mastercache
    {
        static boost::thread_specific_ptr< fostcache > s_instance;
    public:
        explicit fostcache( mastercache &master );
        explicit fostcache( dbconnection &dbc );
        ~fostcache();

        // Check the existence of a cache and return the instance associated with the current thread
        static bool exists();
        static fostcache &instance();

        const fostlib::meta_instance &operator [] ( const string & ) const;

    private:
        mastercache *m_master;
    };


    /*
        This smart pointer type will allow objects to be fetched from an underlying
        data store via the fostcache.
    */
    template< typename O, typename K = typename O::key_type >
    class object_ptr {
    public:
        typedef O instance_type;
        typedef K key_type;

        object_ptr();

        bool operator == ( const object_ptr &r ) const;
        bool operator != ( const object_ptr &r ) const;

        bool isnull() const;
        bool operator == ( t_null ) const;
        bool operator != ( t_null ) const;

        const key_type &value() const;

    private:
        nullable< key_type > m_key;
        boost::weak_ptr< instance_type > m_pointer;
    };


}


#include <fost/detail/cache-i.hpp>


#endif // FOST_CACHE_HPP

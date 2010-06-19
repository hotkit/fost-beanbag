/*
    Copyright 1998-2010, Felspar Co Ltd. http://fost.3.felspar.com/
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


    /// The object cache provides a protocol that can be used to store object instances inside a mastercache or fostcache
    template< typename O, typename K = typename O::key_type >
    class objectcache : public detail::objectcache_base {
    public:
        typedef O instance_type;
        typedef K key_type;

    private:
        std::map< key_type, boost::shared_ptr< instance_type > >
            m_objects;
    };


    /// A mastercache allows for objects to be stored for several threads. A mastercache is shared between many threads and there must only be a single mastercache per database connection in any given process.
    class FOST_CACHE_DECLSPEC mastercache : boost::noncopyable {
        std::map<
            boost::shared_ptr< meta_instance >,
            boost::shared_ptr< detail::objectcache_base >
        > m_caches;
    public:
        /// Create a master cache on a database connection
        explicit mastercache( dbconnection &dbc );
        /// Allow master caches to be sub-classed
        virtual ~mastercache();

        /// Register a type as one that will be cached
        mastercache &type(
            boost::shared_ptr< fostlib::meta_instance > type
        );
        virtual const fostlib::meta_instance &operator [] (
            const string &
        ) const;

        /// The database connection being cached
        dbconnection &connection() { return m_dbc; }
        /// The database connection being cached
        const dbconnection &connection() const { return m_dbc; }

    protected:
        typedef std::map<
            string, boost::shared_ptr< fostlib::meta_instance >
        > type_registry_collection;
        type_registry_collection m_types;
        dbconnection &m_dbc;
    };


    /// The fostcaches are used in a thread to manage the objects that the thread may change.
    class FOST_CACHE_DECLSPEC fostcache :
        private detail::fostcache_dbc,
        public mastercache
    {
        static boost::thread_specific_ptr< fostcache > s_instance;
    public:
        /// Build a cache based on a master
        explicit fostcache( mastercache &master );
        /// Build a stand alone cache on a database connection
        explicit fostcache( dbconnection &dbc );
        /// A destructor for the cache
        ~fostcache();

        /// Check the existence of a cache and return the instance associated with the current thread
        static bool exists();
        /// Allow access to the cache instance
        static fostcache &instance();

        const fostlib::meta_instance &operator [] ( const string & ) const;

    private:
        mastercache *m_master;
    };


    /// This smart pointer type will allow objects to be fetched from an underlying data store via the fostcache.
    template< typename O, typename K = typename O::key_type >
    class object_ptr {
    public:
        /// The type of instance pointed to
        typedef O instance_type;
        /// The type of the key used by the instances
        typedef K key_type;

        /// Construct an empty pointer which is null
        object_ptr();

        /// Allow comparison of pointers
        bool operator == ( const object_ptr &r ) const;
        /// Allow comparison of pointers for inequality
        bool operator != ( const object_ptr &r ) const;

        /// Returns true if the pointer is null
        bool isnull() const;
        /// Allow comparison with null
        bool operator == ( t_null ) const;
        /// Check for inequality with null
        bool operator != ( t_null ) const;

        /// Return the key that the pointer wraps
        const key_type &value() const;

        /// Allow underlying value to be fetched
        instance_type *operator -> ();
        /// Allow underlying value to be fetched
        instance_type const *operator -> () const;

    private:
        nullable< key_type > m_key;
        mutable boost::weak_ptr< instance_type > m_pointer;
    };


}


namespace std {
    /// Allow object_ptr instances to be output to a stream
    template< typename O, typename K >
    fostlib::ostream &operator << (
        fostlib::ostream &o,
        const fostlib::object_ptr< O, K > &p
    ) {
        if ( p.isnull() )
            return o << "NULL";
        else
            throw fostlib::exceptions::not_implemented(
                "osream << for fostlib::object_ptr"
            );
    }
}


#include <fost/detail/cache-i.hpp>


#endif // FOST_CACHE_HPP

/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_CACHE_HPP
#define FOST_CACHE_HPP
#pragma once


#include <fost/db>
#include <boost/thread.hpp>


namespace fostlib {


    /*
        The object cache provides a protocol that can be used to store object
        instances inside a mastercache or fostcache
    */
    template< typename object_type >
    class objectcache;

    template<>
    class FOST_CACHE_DECLSPEC objectcache< meta_instance > : boost::noncopyable {
        std::map< instance::key_type, boost::shared_ptr< instance > > m_objects;
    public:
        virtual ~objectcache();
    };

    template< typename object_type >
    class objectcache : objectcache< meta_instance > {
    };


    /*
        A mastercache allows for objects to be stored for long periods. A mastercache
        is shared between many threads and there must only be a single mastercache
        per database connection in any given process.
    */
    class FOST_CACHE_DECLSPEC mastercache : boost::noncopyable {
        std::map<
            boost::shared_ptr< meta_instance >,
            boost::shared_ptr< objectcache< meta_instance > >
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


    namespace detail {
        class FOST_CACHE_DECLSPEC fostcache_dbc : boost::noncopyable {
        protected:
            fostcache_dbc();
            fostcache_dbc( dbconnection &dbc );
            boost::scoped_ptr< dbconnection > m_dbc_ptr;
        };
    }
    /*
        The fostcaches are used in a thread to manage the objects that the thread may change.
    */
    class FOST_CACHE_DECLSPEC fostcache : private detail::fostcache_dbc, public mastercache {
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


}


#endif // FOST_CACHE_HPP

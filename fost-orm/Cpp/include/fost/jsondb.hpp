/*
    Copyright 2007-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_JSONDB_HPP
#define FOST_JSONDB_HPP
#pragma once


#include <fost/core>
#include <fost/json.hpp>
#include <fost/thread.hpp>


namespace fostlib {


    /// A simple transactional JSON based database
    class FOST_JSONDB_DECLSPEC jsondb : boost::noncopyable {
        in_process< json > m_blob;
    public:
        typedef boost::function< void ( json & ) > operation_signature_type;
        typedef std::vector< operation_signature_type > operations_type;

        /// Create an in memory JSON database
        jsondb();
        /// Create a JSON database that is backed to disk
        explicit jsondb(const string &filename,
            const nullable< json > &default_db = null);

        /// The filename of a disk backed database
        accessors< const nullable< string > > filename;

        /// A transaction for accessing the database
        class FOST_JSONDB_DECLSPEC local : boost::noncopyable {
            jsondb &m_db;
            json m_local;
            const jcursor m_position;
            operations_type m_operations;
        public:
            /// Create a transaction
            explicit local( jsondb &db, const jcursor & = jcursor() );

            /// Check to see if the database contains a specified location or not
            template< typename key >
            bool has_key( const key &k ) const {
                return m_local.has_key( k );
            }
            /// Return the data at the specified key in the database
            template< typename key >
            const json &operator [] ( const key &p ) const {
                return m_local[ p ];
            }

            /// Insert a new item at the specified key position
            local &insert( const jcursor &position, const json &item );
            /// Push back a new item on to the array at the specified position
            local &push_back( const jcursor &position, const json &item );
            /// Change the JSON data at the position specified checking to make sure the same value is being replaced
            local &update( const jcursor &position, const json &item );
            /// Remove the JSON data at the position specified checking to make sure that the same value is being deleted
            local &remove( const jcursor &position );

            /// Insert into the transaction
            template< typename V >
            local &insert( const jcursor &position, const V &item ) {
                return insert( position, coerce<json>( item ) );
            }
            /// Push back into the transaction
            template< typename V >
            local &push_back( const jcursor &position, const V &item ) {
                return push_back( position, coerce<json>( item ) );
            }
            /// Update the transaction
            template< typename V >
            local &update( const jcursor &position, const V &item ) {
                return update( position, coerce<json>( item ) );
            }

            /// Commit the transaction
            void commit();
            /// Throw the transaction away
            void rollback();

        private:
            void refresh();
        };
        friend class local;
    };


}


#endif // FOST_JSONDB_HPP

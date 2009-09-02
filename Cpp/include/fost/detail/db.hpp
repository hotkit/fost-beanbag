/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_DB_HPP
#define FOST_DB_HPP
#pragma once


#include <fost/string>
#include <fost/dynlib.hpp>


namespace fostlib {


    namespace exceptions {


        class FOST_SCHEMA_DECLSPEC data_driver : public exception {
        public:
            data_driver( const string &error, const string &driver ) throw ();
            data_driver( const string &error, const string &driver1, const string &driver2 ) throw ();

        protected:
            wliteral const message() const throw ();
        };


    }


    namespace sql {


        struct FOST_SCHEMA_DECLSPEC sql_statement_tag {
            static void do_encode( fostlib::wliteral from, fostlib::string &into );
            static void do_encode( const fostlib::string &from, fostlib::string &into );
            static void check_encoded( const fostlib::string &s );
        };

        struct statement_tag : public sql_statement_tag {};
        typedef tagged_string< statement_tag, string > statement;


    }


    // Forward declarations of classes in schema/dynamic.hpp
    class instance;
    class meta_instance;
    // Forward declarations of classes in this file
    class dbconnection;
    class recordset;


    /*
        Specifies the database interface that all database drivers must implement
    */
    class FOST_SCHEMA_DECLSPEC dbinterface {
    protected:
        explicit dbinterface( const string &driver_name );
    public:
        virtual ~dbinterface();

        accessors< string > name;

        /*
            Every database driver must implement these interfaces
            #include <fost/db-driver>
        */
        class read;
        class recordset;
        class write;

        /*
            Create and destroy databases
        */
        virtual void create_database( dbconnection &dbc, const string &name ) const = 0;
        virtual void drop_database( dbconnection &dbc, const string &name ) const = 0;

        /*
            Make a connection to the configured database
        */
        virtual boost::shared_ptr< fostlib::dbinterface::read > reader( dbconnection &dbc ) const = 0;

        /*
            These members implement ID creation functionality
        */
        virtual int64_t next_id( dbconnection &dbc, const string &counter ) const = 0;
        virtual int64_t current_id( dbconnection &dbc, const string &counter ) const = 0;
        virtual void used_id( dbconnection &dbc, const string &counter, int64_t value ) const = 0;

        /*
            If the connection needs to store any specific data it should derive from this struct
            to store it and return the data from the connect function.
        */
        struct connection_data : boost::noncopyable {
            virtual ~connection_data() {}
        };
        virtual std::auto_ptr< connection_data > connect( dbconnection &dbc ) const;
    };


    class FOST_SCHEMA_DECLSPEC recordset {
    public:
        recordset( boost::shared_ptr< dbinterface::recordset > rs );
        ~recordset();

        const sql::statement &command() const;

        bool eof() const;
        void moveNext();

        std::size_t fields() const;
        const string &name( std::size_t f ) const;
        const json &field( const string &i ) const;
        const json &field( std::size_t i ) const;

        bool isnull( const string & ) const;
        bool isnull( std::size_t ) const;

        json to_json() const;

    private:
        boost::shared_ptr< dbinterface::recordset > m_interface;
    };


    class FOST_SCHEMA_DECLSPEC dbtransaction : boost::noncopyable {
    public:
        dbtransaction( dbconnection &dbc );
        ~dbtransaction();

        dbtransaction &create_table( const meta_instance &meta );
        dbtransaction &drop_table( const meta_instance &meta );
        dbtransaction &drop_table( const string &table );

        dbtransaction &insert( const instance &object, boost::function< void( void ) > oncommit );
        dbtransaction &execute( const sql::statement &cmd );

        void commit();

    private:
        dbconnection &m_connection;
        boost::shared_ptr< dbinterface::write > m_transaction;
        std::vector< boost::function< void( void ) > > m_oncommit, m_onfail;
    };


    class FOST_SCHEMA_DECLSPEC dbconnection : boost::noncopyable {
        friend class dbtransaction;
    public:
        static const setting< bool > c_commitCount;
        static const setting< string > c_commitCountDomain;

        explicit dbconnection( const json &configuration );
        explicit dbconnection( const string &read_dsn );
        dbconnection( const string &read_dsn, const fostlib::string &write_dsn );
        ~dbconnection();

        const dbinterface &driver() const;

        void create_database( const string &name );
        void drop_database( const string &name );

        int64_t next_id( const string &counter );
        int64_t current_id( const string &counter );
        void used_id( const string &counter, int64_t value );

        recordset query( const meta_instance &item, const json &key = json() );
        recordset query( const sql::statement &cmd );

        bool in_transaction() const;
        dbtransaction &transaction();

        accessors< const json > configuration;
        dbinterface::connection_data &connection_data();

    private:
        std::pair< const dbinterface *, boost::shared_ptr< dynlib > > m_interface;
        boost::shared_ptr< dbinterface::read > m_connection;
        boost::scoped_ptr< dbinterface::connection_data > m_cnx_data;
        dbtransaction *m_transaction;
    };


}


#endif // FOST_DB_HPP

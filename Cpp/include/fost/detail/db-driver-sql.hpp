/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_DB_DRIVER_SQL_HPP
#define FOST_DB_DRIVER_SQL_HPP
#pragma once


#include <fost/db-driver>


namespace fostlib {


    namespace sql {


        struct table_name_tag : public sql_statement_tag {};
        typedef tagged_string< table_name_tag, string > table_name;

        struct column_name_tag : public sql_statement_tag {};
        typedef tagged_string< column_name_tag, string > column_name;


    }


    class FOST_SQL_DECLSPEC sql_driver : public dbinterface {
    protected:
        explicit sql_driver( const string &driver_name );

        /*
            These functions are used to provide name mangling functionality
        */
        virtual sql::table_name table_name( const meta_instance & ) const;
        virtual sql::column_name column_name( const meta_attribute & ) const;

        virtual sql::statement mangle( const int64_t field_value ) const;
        virtual sql::statement mangle( const string &field_value ) const;
        virtual sql::statement mangle( const sql::table_name &name ) const = 0;
        virtual sql::statement mangle( const sql::column_name &name ) const = 0;

    public:
        class FOST_SQL_DECLSPEC write : public dbinterface::write {
        protected:
            write( dbinterface::read &reader );

            const sql_driver &driver() const;

        public:
            void create_table( const fostlib::meta_instance & );

        protected:
            /*
                These hooks allow database tables to be built from a set of standard building blocks
            */
            // Given SQL statements for the actual columns that make up a table this
            // executes the code that builds the table in the database. It should not do
            // foreign keys.
            virtual void create_table(
                const sql::table_name &table,
                const std::list< std::pair< sql::column_name, sql::statement > > &key,
                const std::list< std::pair< sql::column_name, sql::statement > > &columns
            );
            // Returns one or more SQL blocks for a logical column
            virtual std::map< sql::column_name, sql::statement > column_type(
                const sql::column_name &name, const meta_attribute &field, bool not_null
            );
        };
    };


}


#endif // FOST_DB_DRIVER_SQL_HPP

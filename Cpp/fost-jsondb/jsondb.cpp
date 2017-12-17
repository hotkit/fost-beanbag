/*
    Copyright 1999-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-jsondb.hpp"
#include <fost/jsondb.hpp>
#include <fost/db-driver>
#include <fost/insert>
#include <fost/unicode>

#include <fost/exception/not_null.hpp>
#include <fost/exception/out_of_range.hpp>
#include <fost/exception/query_failure.hpp>
#include <fost/exception/transaction_fault.hpp>
#include <fost/exception/unexpected_eof.hpp>


using namespace fostlib;


namespace {


    string dbname( const json &config ) {
        const auto db = coerce<nullable<f5::u8view>>(config["database"]);
        if ( db ) return db.value();
        if ( config.has_key("write") && config["read"] != config["write"] ) {
            throw exceptions::data_driver("JSON database must have the same "
                "read/write connections", "json");
        }
        const auto read = coerce<nullable<f5::u8view>>(config["read"]);
        if ( read ) return read.value();
        throw exceptions::data_driver(
            "You must specify a database name or read/write database names",
            "json");
    }
    nullable< string > dbpath( const json &config ) {
        const auto fn = coerce<nullable<f5::u8view>>(config["filename"]);
        const auto root = coerce<nullable<f5::u8view>>(config["root"]);
        return concat( root, L"/", fn );
    }
    nullable<string> dbpath( const json &config, const string &name ) {
        const auto root = coerce<nullable<f5::u8view>>(config["root"]);
        if ( not root ) return null;
        else return concat(root, "/", name + ".json");
    }
    bool allows_write( const dbconnection &dbc ) {
        return
            (coerce<nullable<f5::u8view>>(dbc.configuration()["database"]) &&
                dbc.configuration()[ L"write" ].get<bool>().value_or(false))
            || coerce<nullable<f5::u8view>>(dbc.configuration()["write"]);
    }

    jsondb &g_database( const string &dbname, const nullable< string > &file, bool create ) {
        static boost::mutex mx;
        static std::map< string, boost::shared_ptr< jsondb > > databases;
        boost::mutex::scoped_lock lock( mx );
        std::map< string, boost::shared_ptr< jsondb > >::iterator p( databases.find( dbname ) );
        if ( p == databases.end() ) {
            boost::shared_ptr< jsondb > db;
             try {
                if ( not file ) db.reset(new jsondb);
                else {
                    try {
                        if ( create || dbname == L"master" ) // We always allow master database to be created
                            db.reset(
                                new jsondb(coerce<boost::filesystem::wpath>(file.value()), json()));
                        else
                            db.reset(
                                new jsondb(coerce<boost::filesystem::wpath>(file.value())));
                    } catch ( exceptions::exception &e ) {
                        fostlib::insert(e.data(), "database", "filename", file.value());
                        throw;
                    }
                }
                jsondb::local loc( *db );
                if ( dbname == L"master" && !loc.has_key( L"database" ) )
                    loc.insert( jcursor( L"database" ), json() ).commit();
                p = databases.insert( std::make_pair( dbname, db ) ).first;
            } catch ( exceptions::exception &e ) {
                fostlib::insert(e.data(), "database", "name", dbname);
                throw;
            }
        }
        return *p->second;
    }
    jsondb &g_database( const json &config, bool create ) {
        return g_database( dbname( config ), dbpath( config ), create );
    }


    const class jsonInterface : public dbinterface {
    public:
        jsonInterface();

        void create_database( dbconnection &dbc, const string &name ) const;
        void drop_database( dbconnection &dbc, const string &name ) const;

        boost::shared_ptr< dbinterface::read > reader( dbconnection &dbc ) const;

        int64_t next_id( dbconnection &dbc, const string &counter ) const;
        int64_t current_id( dbconnection &dbc, const string &counter ) const;
        void used_id( dbconnection &dbc, const string &counter, int64_t value ) const;
    } c_driver;


    class jsonreader : public dbinterface::read {
    public:
        jsonreader( dbconnection &d );

        boost::shared_ptr< dbinterface::recordset > query( const meta_instance &item, const json &key ) const;
        boost::shared_ptr< dbinterface::recordset > query( const sql::statement &cmd ) const;

        boost::shared_ptr< dbinterface::write > writer();

        boost::scoped_ptr< jsondb::local > database;
    };


    class jsonwriter : public dbinterface::write {
    public:
        jsonwriter( jsonreader &reader );

        void create_table( const meta_instance &meta );
        void drop_table( const meta_instance &meta );
        void drop_table( const string &table );

        void insert( const instance &object );
        void execute( const sql::statement &cmd );

        void commit();
        void rollback();

        jsondb::local &database;
    };


    class jsonrecordset : public dbinterface::recordset {
    public:
        jsonrecordset( const meta_instance &item, const json &rs );

        bool eof() const;
        void moveNext();
        std::size_t fields() const;
        const string &name( std::size_t f ) const;
        const json &field( std::size_t i ) const;
        const json &field( const string &name ) const;

        json to_json() const { return m_rs; }

    private:
        std::set< string > m_fieldnames;
        json::const_iterator m_position, m_end;
        json m_rs;
    };


}


/*
    jsonInterface
*/

namespace {
    struct master {
        master()
        : database( L"database" ) {
            database.primary_key( L"name", L"text" );
        }
        meta_instance database;
    };
    boost::scoped_ptr< master > master_schema;

    void check_master_write( const dbconnection &dbc ) {
        if ( !master_schema ) master_schema.reset( new master );
        if ( dbname( dbc.configuration() ) != L"master" )
            throw exceptions::data_driver( L"Can only manage databases when connected to the 'master' database", L"json" );
        if ( !allows_write( dbc ) )
            throw exceptions::transaction_fault( L"Cannot manage databases without a write connection" );
    }
}

jsonInterface::jsonInterface()
: dbinterface( L"json" ) {
}

void jsonInterface::create_database( dbconnection &dbc, const string &name ) const {
    try {
        check_master_write( dbc );
        fostlib::recordset rs( dbc.query( master_schema->database, json( name ) ) );
        if ( rs.eof() ) {
            g_database( name, dbpath( dbc.configuration(), name ), true );
            json init;
            jcursor( L"name" )( init ) = name;
            boost::shared_ptr< instance > dbrep(
                master_schema->database.create(init).release());
            dbtransaction trans( dbc );
            dbrep->save( trans );
            trans.commit();
        } else
            throw exceptions::query_failure(
                L"The requested database already exists", master_schema->database
            );
    } catch ( exceptions::exception &e ) {
        fostlib::insert(e.data(), "database", "name", name);
        throw;
    }
}

void jsonInterface::drop_database( dbconnection &dbc, const string &/*name*/ ) const {
    check_master_write( dbc );
    throw exceptions::not_implemented( L"void jsonInterface::drop_database( dbconnection &dbc, const string &name ) const" );
}

boost::shared_ptr< dbinterface::read > jsonInterface::reader( dbconnection &dbc ) const {
    if ( !master_schema ) master_schema.reset( new master );
    return boost::shared_ptr< dbinterface::read >( new jsonreader( dbc ) );
}

int64_t jsonInterface::next_id( dbconnection &dbc, const string &counter ) const {
    throw fostlib::exceptions::not_implemented( L"jsonInterface::next_id( const string &counter ) const" );
}
int64_t jsonInterface::current_id( dbconnection &dbc, const string &counter ) const {
    throw fostlib::exceptions::not_implemented( L"jsonInterface::current_id( const string &counter ) const" );
}
void jsonInterface::used_id( dbconnection &dbc, const string &counter, int64_t value ) const {
    throw fostlib::exceptions::not_implemented( L"jsonInterface::used_id( const string &counter, int64_t value ) const" );
}


/*
    jsonreader
*/

jsonreader::jsonreader( dbconnection &dbc )
: read( dbc ), database( new jsondb::local( g_database( dbc.configuration(), false ) ) ) {
}

boost::shared_ptr< dbinterface::recordset > jsonreader::query( const meta_instance &item, const json &key ) const {
    if ( m_connection.in_transaction() )
        throw exceptions::transaction_fault(
            L"Cannot query the JSON database whilst there is a write transaction"
        );
    if ( database->has_key( item.fq_name() ) ) {
        if ( key.isnull() )
            return boost::shared_ptr< dbinterface::recordset >(
                new jsonrecordset( item, (*database)[ item.fq_name() ] )
            );
        jcursor position = jcursor( item.fq_name() ) / key;
        if ( database->has_key( position ) )
            return boost::shared_ptr< dbinterface::recordset >( new jsonrecordset( item, (*database)[ position ] ) );
        else
            return boost::shared_ptr< dbinterface::recordset >( new jsonrecordset( item, json() ) );
    } else
        throw exceptions::query_failure( L"Database table not found", item );
}
boost::shared_ptr< dbinterface::recordset > jsonreader::query( const sql::statement &cmd ) const {
    throw exceptions::not_implemented( L"jsonreader::query( const sql::statement &cmd ) const" );
}

boost::shared_ptr< dbinterface::write > jsonreader::writer() {
    return boost::shared_ptr< dbinterface::write >( new jsonwriter( *this ) );
}


/*
    jsonwriter
*/


jsonwriter::jsonwriter( jsonreader &reader )
: dbinterface::write( reader ), database( *reader.database ) {
}


void jsonwriter::create_table( const meta_instance &meta ) {
    database.insert( jcursor( meta.fq_name() ), json() );
}

void jsonwriter::drop_table( const meta_instance &/*meta*/ ) {
    throw exceptions::not_implemented( L"void jsonwriter::drop_table( const meta_instance &meta ) const" );
}
void jsonwriter::drop_table( const string &/*table*/ ) {
    throw exceptions::not_implemented( L"void jsonwriter::drop_table( const string &table ) const" );
}

void jsonwriter::insert( const instance &object ) {
    jcursor key( object._meta().fq_name() );
    json repr = json::object_t();
    for ( meta_instance::const_iterator col( object._meta().begin() ); col != object._meta().end(); ++col ) {
        if ( (*col)->key() )
            key /= object[ (*col)->name() ].to_json();
        jcursor( (*col)->name() ).insert( repr, object[ (*col)->name() ].to_json() );
    }
    database.insert( key, repr );
}


void jsonwriter::execute( const sql::statement &cmd ) {
    throw exceptions::not_implemented( L"jsonwriter::execute( const sql::statement &cmd )" );
}

void jsonwriter::commit() {
    database.commit();
}


void jsonwriter::rollback() {
    database.rollback();
}


/*
    jsonrecordset
*/


jsonrecordset::jsonrecordset( const meta_instance &item, const json &rs )
: dbinterface::recordset( sql::statement( L"JSON query" ) ),  m_position( rs.begin() ), m_end( rs.end() ), m_rs( rs ) {
    for ( meta_instance::const_iterator i( item.begin() ); i != item.end(); ++i )
        m_fieldnames.insert( (*i)->name() );
}


bool jsonrecordset::eof() const {
    return m_position == m_end;
}


void jsonrecordset::moveNext() {
    if ( eof() )
        throw exceptions::unexpected_eof( L"The recordset is already at the end" );
    ++m_position;
}


std::size_t jsonrecordset::fields() const {
    return m_fieldnames.size();
}


const string &jsonrecordset::name( std::size_t f ) const {
    for ( std::set< string >::const_iterator i( m_fieldnames.begin() ); i != m_fieldnames.end(); ++i, --f )
        if ( f == 0 )
            return *i;
    throw exceptions::out_of_range< std::size_t >( L"There aren't enough fields in the recordset", 0, m_fieldnames.size(), f );
}


const json &jsonrecordset::field( std::size_t i ) const {
    if ( eof() )
        throw exceptions::unexpected_eof( L"Cannot fetch a field from a recordset after reaching the end of the recordset" );
    return field( name( i ) );
}


const json &jsonrecordset::field( const string &name ) const {
    if ( eof() )
        throw exceptions::unexpected_eof( L"Cannot fetch a field from a recordset after reaching the end of the recordset" );
    return (*m_position)[ name ];
}


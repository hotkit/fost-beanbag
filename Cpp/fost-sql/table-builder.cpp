/*
    Copyright 2009-2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-sql.hpp"
#include <fost/db-driver-sql.hpp>


namespace {
    void set_definitions(
        std::list< std::pair< fostlib::sql::column_name, fostlib::sql::statement > > &to,
        const std::map< fostlib::sql::column_name, fostlib::sql::statement > &def
    ) {
        for ( std::map< fostlib::sql::column_name, fostlib::sql::statement >::const_iterator i( def.begin() ); i != def.end(); ++i )
            to.push_back(*i);
    }
}
void fostlib::sql_driver::write::create_table( const fostlib::meta_instance &mi ) {
    sql::table_name table = driver().table_name( mi );
    std::list< std::pair< sql::column_name, sql::statement > > keys, cols;
    for ( meta_instance::const_iterator attribute( mi.begin() ); attribute != mi.end(); ++attribute ) {
        std::map< sql::column_name, sql::statement > definition =
            column_type(
                driver().column_name( **attribute ),
                **attribute,
                (*attribute)->required()
            );
        if ( (*attribute)->key() )
            set_definitions(keys, definition);
        else
            set_definitions(cols, definition);
    }
    create_table( table, keys, cols );
}


std::map< fostlib::sql::column_name, fostlib::sql::statement > fostlib::sql_driver::write::column_type(
    const sql::column_name &name, const meta_attribute &ma, bool not_null
) {
    field_base::const_iterator begin( ma.type().begin() ), end( ma.type().end() );
    std::map< sql::column_name, sql::statement > ret;
    if ( begin != end )
        for ( field_base::const_iterator field( begin ); field != end; ++field ) {
            std::map< sql::column_name, sql::statement > part = column_type(
                name + sql::column_name(L"_") + driver().column_name( **field ), **field, not_null
            );
            for ( std::map< sql::column_name, sql::statement >::const_iterator sub( part.begin() ); sub != part.end(); ++sub )
                ret[ sub->first ] = sub->second;
        }
    else {
        fostlib::sql::statement column( ma.type().type_name() );
        if ( ma.size() ) {
            column += fostlib::sql::statement( L" (" )
                + fostlib::sql::statement( fostlib::coerce< fostlib::string >( ma.size().value() ) )
                + fostlib::sql::statement( L")" );
        }
        ret[ name ] = column + fostlib::sql::statement( not_null ? L" NOT NULL" : L" NULL" );
    }
    return ret;
}

void fostlib::sql_driver::write::create_table(
    const fostlib::sql::table_name &table,
    const std::list< std::pair< sql::column_name, sql::statement > > &key,
    const std::list< std::pair< sql::column_name, sql::statement > > &columns
) {
    fostlib::nullable< sql::statement > cols, pk;
    for ( std::list< std::pair< sql::column_name, sql::statement > >::const_iterator it( key.begin() ); it != key.end(); ++it ) {
        pk = concat( pk, sql::statement( L", " ), driver().mangle( it->first ) );
        cols = concat( cols, sql::statement( L", " ), driver().mangle( it->first ) + sql::statement( L" " ) + it->second );
    }
    for ( std::list< std::pair< sql::column_name, sql::statement > >::const_iterator it( columns.begin() ); it != columns.end(); ++it )
        cols = concat( cols, sql::statement( L", " ), driver().mangle( it->first ) + sql::statement( L" " ) + it->second );
    execute(
        fostlib::sql::statement( L"CREATE TABLE " ) + driver().mangle( table ) +
        fostlib::sql::statement( L" ( " ) + cols.value() +
        fostlib::sql::statement( L", CONSTRAINT " ) + driver().mangle( fostlib::sql::table_name( L"PK_" ) + table ) +
        fostlib::sql::statement( L" PRIMARY KEY (" ) + pk.value() +
        fostlib::sql::statement( L"));" )
    );
}

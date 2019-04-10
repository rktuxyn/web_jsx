/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
class pgSql {
    constructor( npgsql/*[Native]*/, db_conn/*Database connection string*/, pgsql_ctx/*[User Context]*/ ) {
        if ( !npgsql || typeof ( npgsql ) !== "object" )
            throw new TypeError( "npgsql native wraper required!!!" );
        if ( npgsql.execute_io.toString().indexOf( "[native code]" ) < 0 )
            throw new TypeError( "npgsql native wraper required!!!" );
        if ( !pgsql_ctx || pgsql_ctx === null || typeof ( pgsql_ctx ) === "undefined" )
            throw new TypeError( "User Context required!!!" );
        if ( !db_conn || typeof ( db_conn ) !== "string" )
            throw new TypeError( "Database connection string required!!!" );
        this.npgsql = npgsql;
        this.pgsql_ctx = typeof ( pgsql_ctx ) === 'object' ? JSON.stringify( pgsql_ctx ) : pgsql_ctx;
        this.db_conn = db_conn;
    }
    dispose() {
        delete this.npgsql;
        delete this.pgsql_ctx;
        delete this.db_conn;
    }
    static quote_literal( value ) {
        try {
            if ( value === undefined || value === null ) return "";
            if ( typeof ( value ) !== "string" )
                value = String( value );
            value = value.replace( '\'', "''" );
            return "''" + value + "''";
        } catch ( e ) {
            throw new Error( e.message + " " + value );
        }
    }
    create_query( obj, def, nowhere ) {
        if ( !obj || obj === null || typeof ( obj ) !== "object" )
            throw new Error( "Plain object required!!!" );
        let query = "";
        if ( Object.keys( obj ).length <= 0 ) return "";
        if ( def === void 0 || def === null || typeof ( def ) !== "object" ) def = {};
        let isFirst = true;
        for ( let p in obj ) {
            let pv = obj[p];
            if ( def[p] ) {
                pv = def[p].call( pgSql, pv, obj );
                if ( isFirst ) {
                    isFirst = false;
                    query += pv; continue;
                }
                query += "and " + pv; continue;
            }
            if ( isFirst ) {
                query += p + "=" + pgSql.quote_literal( pv ); isFirst = false;
            } else {
                query += "and " + p + "=" + pgSql.quote_literal( pv );
            }
        }
        nowhere = typeof ( nowhere ) !== "boolean" ? false : nowhere;
        return query.length > 0 ? ( nowhere === true ? query : "where " + query ) : query;
    }
    parse_param( param, sql ) {
        if ( !sql ) throw new Error( "SQL required!!!" );
        sql = sql.replace( /\r\n/gi, "" ).replace( /\n/gi, "" ).replace( /\t/gi, " " ).replace( /\s+/g, " " );//normalize
        if ( typeof ( param ) !== "object" )
            return sql;
        let len = param.length;
        return sql.replace( /{(\d+)}/g, function ( match, number ) {
            let index = parseInt( number );
            if ( isNaN( index ) )
                throw new Error( "Invalid param index!!!" );

            if ( index > len )
                throw new Error( "Index should not greater than " + len );

            return typeof ( param[index] ) !== 'undefined'
                ? pgSql.quote_literal( param[index] )
                : /*match || ""*/pgSql.quote_literal( "" )
                ;
        } );
    }
    execute_io( sp, form_object ) {
        if ( !sp || !form_object || form_object === null )
            throw new TypeError( "Invalid argument defined!!!" );
        try {
            let resp = this.npgsql.execute_io(
                this.db_conn, sp, this.pgsql_ctx,
                typeof ( form_object ) === 'object' ? JSON.stringify( form_object ) : form_object
            );
            form_object = undefined;
            let ret_val = parseInt( resp._ret_val );
            return {
                "ret_val": isNaN( ret_val ) ? 0 : ret_val,
                "ret_msg": resp._ret_msg,
                "ret_data_table": resp._ret_data_table
            };
        } catch ( e ) {
            form_object = undefined;
            return {
                "ret_val": -1,
                "ret_msg": e.message,
                "ret_data_table": "{}"
            };
        }

    }
    async execute_promise( sp, form_object ) {
        return new Promise( ( resolve, reject ) => {
            resolve.call( this, this.execute_io( sp, form_object ) );
        } );
    }
    async execute_async( sp, form_object, cb ) {
        await __async_t( () => {
            cb.call( this, this.execute_io( sp, form_object ) );
        } );
    }
    force_execute( sp, obj, max_try ) {
        if ( !sp || !obj || obj === null )
            throw new TypeError( "Invalid argument defined!!!" );
        let try_count = 0;
        typeof ( max_try ) !== 'number' ? max_try = 5 : undefined;
        let pg_resp = {};
        typeof ( obj ) === 'object' ? obj = JSON.stringify( obj ) : undefined;
        do {
            pg_resp = this.execute_io( sp, obj );
            if ( pg_resp.ret_val < 0 ) {
                try_count++;
                if ( try_count > max_try ) {
                    return { ret_msg: pg_resp.ret_msg, ret_val: -1 };
                }
                continue;
            }
            break;
        } while ( true );
        return pg_resp;
    }
}
module.exports = {
    pgSql: pgSql
};

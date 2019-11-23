/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
const npgsql_db_type = {
    COMMON: -1, NULL: -2, Array: 1,
    Bigint: 1, Boolean: 2, Box: 3, Bytea: 4,
    Circle: 5, Char: 6, Date: 7, Double: 8,
    Integer: 9, Line: 10, LSeg: 11, Money: 12,
    Numeric: 13, Path: 14, Point: 15, Real: 17,
    Smallint: 18, Text: 19, Time: 20, Timestamp: 21,
    Varchar: 22, Refcursor: 23, Inet: 24, Bit: 25,
    TimestampTZ: 26, Uuid: 27, Xml: 28, Oidvector: 29,
    Interval: 30, TimeTZ: 31, Name: 32, Abstime: 33,
    MacAddr: 34, Json: 35, Jsonb: 36, Hstore: 37,
    InternalChar: 38, Varbit: 39, Xid: 42, Cid: 43, Cidr: 44,
    TsVector: 45, TsQuery: 46, Enum: 47, Composite: 48,
    Regtype: 49, Geometry: 50, Citext: 51, Int2Vector: 52, Tid: 53, Range: 1073741824
};
const parameter_direction = {
    Input: 1,
    Output: 2, InputOutput: 3,
    ReturnValue: 6
};
class pgSql {
    constructor( db_conn/*Database connection string*/, pgsql_ctx/*[User Context]*/) {
        if ( !pgsql_ctx || pgsql_ctx === null || typeof ( pgsql_ctx ) === "undefined" )
            throw new TypeError( "User Context required!!!" );
        if ( !db_conn || typeof ( db_conn ) !== "string" )
            throw new TypeError( "Database connection string required!!!" );
        //this.npgsql = npgsql;
        this.pgsql_ctx = typeof ( pgsql_ctx ) === 'object' ? JSON.stringify( pgsql_ctx ) : pgsql_ctx;
        this.db_conn = db_conn;
    }
    dispose() {
        //delete this.npgsql;
        delete this.pgsql_ctx;
        delete this.db_conn;
    }
    static quote_literal( value ) {
        try {
            if ( value === undefined || value === null ) return "";
            if ( typeof ( value ) === "object" ) {
                value = JSON.stringify( value );
            } else {
                if ( typeof ( value ) !== "string" )
                    value = String( value );
            }
            value = value.replace( '\'', "''" );
            return "''" + value + "''";
        } catch ( e ) {
            throw new Error( e.message + " " + value );
        }
    }
    static quote_literalq( value ) {
        try {
            if ( value === undefined || value === null ) return "";
            if ( typeof ( value ) === "object" ) {
                value = JSON.stringify( value );
            } else {
                if ( typeof ( value ) !== "string" )
                    value = String( value );
            }
            value = value.replace( '\'', "'" );
            return "'" + value + "'";
        } catch ( e ) {
            throw new Error( e.message + " " + value );
        }
    }
    parse_paramq( param, sql ) {
        if ( !sql ) throw new Error( "SQL required!!!" );
        sql = sql.replace( /\r\n/gi, "" ).replace( /\n/gi, "" ).replace( /\t/gi, " " ).replace( /\s+/g, " " );//normalize
        if ( !param || typeof ( param ) !== "object" || !Array.isArray( param ) ) return sql;
        let len = param.length;
        return sql.replace( /{(\d+)}/g, function ( match, number ) {
            let index = parseInt( number );
            if ( isNaN( index ) )
                throw new Error( "Invalid param index!!!" );

            if ( index > len )
                throw new Error( "Index should not greater than " + len );

            return typeof ( param[index] ) !== 'undefined'
                ? pgSql.quote_literalq( param[index] )
                : /*match || ""*/"null"
                ;
        } );
    }
    create_query( obj, def, nowhere ) {
        if ( !obj || obj === null || typeof ( obj ) !== "object" )
            throw new Error( "Plain object required!!!" );
        let query = "";
        if ( Object.keys( obj ).length <= 0 ) return query;
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
        if ( !param || typeof ( param ) !== "object" || !Array.isArray( param ) ) return sql;
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
            let resp = npgsql.execute_io(
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
    data_reader( query, param, callback ) {
        if ( typeof ( callback ) !== "function" )
            throw new Error( "Callback required." );
        return npgsql.data_reader( this.db_conn, this.parse_paramq( param, query ), callback );
    }
    
    execute_scalar( sp, params ) {
        try {
            let res = npgsql.execute_scalar( this.db_conn, sp, params );
            let outres = {};
            params.forEach( ( param ) => {
                if ( param.parameter_direction === parameter_direction.Input ) return;
                if ( param.npgsql_db_type === npgsql_db_type.Json ||
                    param.npgsql_db_type === npgsql_db_type.Jsonb ) {
                    return outres[param.name] = res[param.parameter_name] ? JSON.parse( res[param.parameter_name] ) : null, delete res[param.parameter_name], void 0;
                }
                if ( param.npgsql_db_type === npgsql_db_type.Integer ||
                    param.npgsql_db_type === npgsql_db_type.Numeric ||
                    param.npgsql_db_type === npgsql_db_type.Bigint ||
                    param.npgsql_db_type === npgsql_db_type.Smallint ||
                    param.npgsql_db_type === npgsql_db_type.Real ) {
                    let val = parseFloat( res[param.parameter_name] );
                    return outres[param.name] = isNaN( val ) ? 0 : val, delete res[param.parameter_name], void 0;
                }
                return outres[param.name] = res[param.parameter_name], delete res[param.parameter_name], void 0;
            } );
            return outres;
        } catch ( e ) {
            return {
                "ret_val": -1,
                "ret_msg": e.message,
                "ret_data_table": "{}"
            };
        }
        
    }
    execute_query( query, param ) {
        return npgsql.execute_query( this.db_conn, this.parse_paramq( param, query ) )
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
    pgSql: pgSql,
    npgsql_db_type: npgsql_db_type,
    npgsql_parameter_direction: parameter_direction,
    npgsql_createParam: function ( name, db_type, direction, value ) {
        let out = {
            parameter_name: void 0,
            data: void 0,
            npgsql_db_type: db_type,
            parameter_direction: direction,
            name: name
        };
        out.parameter_name = `_${name}`;
        if ( direction === parameter_direction.Input || direction === parameter_direction.InputOutput ) {
            if ( !value || value === null ) {
                out.data = "null";
            } else {
                if ( db_type === npgsql_db_type.Varchar ) {
                    out.data = pgSql.quote_literalq( value );
                } else if ( db_type === npgsql_db_type.Json || db_type === npgsql_db_type.Jsonb ) {
                    out.data = pgSql.quote_literalq( JSON.stringify( value ) );
                } else {
                    out.data = pgSql.quote_literalq( value );
                }
            }
            return out;
        }
        if ( direction === parameter_direction.Output
            || direction === parameter_direction.ReturnValue ) {
            return out;
        }
        throw new Error( `Invalid parameter_direction ${direction}` );
    }
};

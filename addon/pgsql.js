//3:22 AM 1/21/2019
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
class pgSql {
    constructor( db_conn, ctx, npgsql ) {
        this.npgsql = npgsql;
        this.db_conn = db_conn;
        this.ctx = typeof ( ctx ) === 'object' ? JSON.stringify( ctx ) : ctx;
    }
    quote_Literal( value ) {
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
                ? SQL.quote_Literal( param[index] )
                : /*match || ""*/SQL.quote_Literal( "" )
                ;
        } );
    }
    execute_io( sp, form_object) {
        try {
            let resp = this.npgsql.execute_io(
                this.db_conn, sp, this.ctx,
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
    dispose() {
        delete this.npgsql;
        delete this.db_conn;
        delete this.ctx;
    }
}
module.exports = {
    pgSql: pgSql
};
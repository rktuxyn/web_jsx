/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//8:23 PM 12/2/2018
/** [Object Extend]*/
( typeof ( Object.extend ) === 'function' ? undefined : ( Object.extend = function ( destination, source ) {
    for ( let property in source )
        destination[property] = source[property];
    return destination;
} ) );
/** [/Object Extend]*/
Date.prototype.addHours = function ( h ) {
    this.setHours( this.getHours() + h );
    return this;
};
function http_init( body, follow_location ) {
    let req_object = {
        url: this.url,
        method: this.method,
        is_debug: this.is_debug,
        is_verify_ssl: this.is_verify_ssl,
        is_verify_ssl_host: this.is_verify_ssl_host,
        follow_location: typeof ( follow_location ) !== "boolean" ? true : follow_location
    };
    if ( this.method === "POST" ) {
        if ( body === undefined || body === null )
            throw new Error( "Request body required for POST request!!!" );
        if ( typeof ( body ) !== 'string' )
            throw new Error( "POST paylod data typeof(string) required !!!" );
        req_object.body = body; body = undefined;
    }
    if ( Object.keys( this.header ).length > 0 ) {
        req_object.header = [];
        for ( let key in this.header )
            req_object.header.push( `${key}:${this.header[key]}` );
    }
    if ( this.cookie && this.cookie.length > 0 ) {
        req_object.cookie = this.cookie.join( ";" );
    }
    return create_http_request( req_object );
};
function parse_response( resp ) {
    this.response = {
        http_status_code: 0,
        cookie: [],
        header: {},
        body: {},
        is_error: false,
        error: undefined
    };
    if ( resp.ret_val < 0 ) {
        this.response.is_error = true;
        this.response.error = resp.ret_msg;
        return;
    }
    this.response.body = resp.response_body; delete resp.response_body;
    if ( !resp.response_header && "string" !== typeof ( resp.response_header ) ) {
        delete resp.response_header;
        return;
    }
    let arr = resp.response_header.split( "\r\n" );
    delete resp.response_header;
    if ( null === arr ) return;
    for ( let row of arr ) {
        if ( !row ) continue;
        if ( row.indexOf( 'HTTP' ) > -1 ) {
            this.response.http_status_code = parseInt( row.split( " " )[1] );
            continue;
        }
        if ( row.indexOf( 'Set-Cookie' ) > -1 ) {
            let cok = row.split( ":" )[1];
            if ( !cok ) continue;
            cok = cok.split( ";" )[0];
            this.response.cookie.push( cok ); continue;
        }
        let harr = row.split( ":" );
        let key = harr[0].replace( /-/g, "_" ).toLowerCase();
        this.response.header[key] = harr[1];
    }
    return;
};
function prepare_post_data( body ) {
    if ( 'object' !== typeof ( body ) || typeof ( body ) === 'string' ) {
        this.set_header( "Content-Length", String( body.length ) )
        return body;
    }
    if ( null === body && 'object' !== typeof ( body ) )
        throw new Error( "Request body required for POST request!!!" );

    let str = [];
    for ( let p in body ) {
        str.push( encodeURIComponent( p ) + "=" + encodeURIComponent( body[p] ) );
    }
    let data = str.join( "&" );
    this.set_header( "Content-Length", String( data.length ) );
    return data;

};
function clean_resp( rs ) {
    for ( let p in rs )
        delete rs[p];
};
function resolve( deferred, cb ) {
    if ( typeof ( cb ) !== "function" ) return deferred;
    deferred.catch( ( reason ) => {
        cb( -1, reason );
    } ).then( ( s ) => {
        cb( 1, s );
    } ); deferred = undefined;
    return;
};
//8:23 PM 12/2/2018
//1:02 PM 11/24/2019
//With Rajib & Ovi
class http_request {
    constructor( _url, opt ) {
        this.url = _url;
        if ( !opt || opt === null || typeof ( opt ) !== "object" ) {
            opt = {
                is_debug: false,
                is_verify_ssl: true,
                is_verify_ssl_host: true
            };
        }
        Object.extend( this, opt );
        this.response = {};
        this.method = void 0;
        this.cookie = [];
        this.header = {};
    }
    get_time_stamp( day ) {
        return new Date().addHours( typeof ( day ) === "number" ? day : 1 ).toString().split( "(" )[0].trim();
    }
    exists_cookie( cook ) {
        return this.cookie.indexOf( cook ) >= 0;
    }
    set_cookie( key, value ) {
        return this.set_raw_cookie( `${key}=${value}` );
    }
    set_raw_cookie( cook ) {
        if ( this.exists_cookie( cook ) ) return this;
        this.cookie.push( cook );
        return this;
    }
    remove_header( key ) {
        if ( this.header[key] )
            delete this.header[key];
        return this;
    }
    set_header( key, value ) {
        this.remove_header( key );
        this.header[key] = value;
        return this;
    }
    getAsync( cb, follow_location ) {
        return resolve( new Promise( ( resolve, reject ) => {
            this.get( follow_location );
            resolve();
        } ), cb );
    }
    postAsync( body, cb, follow_location ) {
        return resolve( new Promise( ( resolve, reject ) => {
            this.post( body, follow_location ); body = undefined;
            resolve();
        } ), cb );
    }
    /**
     * @type {{body: JSON, cb: Function, follow_location:boolean|void 0}}
     */
    sendAsync( body, cb, follow_location ) {
        return resolve( new Promise( ( resolve, reject ) => {
            this.post( body, follow_location );
            return body = undefined, resolve();
        } ), cb );
    }
    /**
     * @type {{follow_location:boolean|void 0}}
     * @returns {{http_request}}
     */
    get( follow_location ) {
        this.method = "GET";
        let resp = http_init.call( this, void 0, follow_location );
        parse_response.call( this, resp );
        clean_resp( resp );
        return this;
    }
    post( body, follow_location ) {
        this.method = "POST";
        let resp = http_init.call( this, prepare_post_data.call( this, body ), follow_location ); body = void 0;
        parse_response.call( this, resp );
        clean_resp( resp );
        return this;
    }
    send( body, follow_location ) {
        this.method = "POST";
        let resp = http_init.call( this, prepare_post_data.call( this, body ), follow_location ); body = void 0;
        parse_response.call( this, resp );
        clean_resp( resp );
        return this;
    }
    move_to_request( with_header ) {
        if ( 'number' === typeof ( this.response.http_status_code ) && this.response.http_status_code > 0 ) {
            this.cookie = [];
            for ( let part; this.response.cookie.length && ( part = this.response.cookie.shift() ); ) {
                this.cookie.push( part );
            }
        }
        if ( !with_header )
            this.header = {};
        return this;
    }
    clear_response() {
        clean_resp( this.response );
        delete this.response;
        this.url = undefined;
        this.method = undefined;
        this.header = {};
        this.cookie = [];
        return this;
    }
};
module.exports = http_request;

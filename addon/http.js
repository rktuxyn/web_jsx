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
var http_request = function ( _url ) {
    this.url = _url;
};
function http_init( body ) {
    let req_object = {
        url: this.url,
        method: this.method
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
	/*if( this.header && this.header.length > 0)
		req_object.header = this.header;*/
    if ( this.cookie && this.cookie.length > 0 ) {
        req_object.cookie = "";
        for ( let i = 0, l = this.cookie.length; i < l; i++ ) {
            if ( i === 0 ) {
                req_object.cookie += this.cookie[i]; continue;
            }
            req_object.cookie += "; " + this.cookie[i];
        }

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
    //this.form_data = data;
    return data;

};

Object.extend( http_request.prototype, {
    response: {},
    url: undefined,
    method: undefined,
    cookie: [],
    header: {},
    get_time_stamp: function ( day ) {
        return new Date().addHours( typeof ( day ) === "number" ? day : 1 ).toString().split( "(" )[0].trim();
    },
    set_cookie: function ( key, value ) {
        this.cookie.push( `${key}=${value}` );
        return this;
    },
    set_raw_cookie: function ( cook ) {
        this.cookie.push( cook );
        return this;
    },
    remove_header: function ( key ) {
        if ( this.header[key] )
            delete this.header[key];
        /*let index = this.header.indexOf( key );
        throw new Error( `${key} ==> ${index}==>${JSON.stringify( this.header)}` );
        if ( index < 0 ) return;
        this.header.splice( index, 1 );*/
        return this;
    },
    set_header: function ( key, value ) {
        this.header[key] = value;
        /**this.header.push( `${key}:${value}` );*/
        return this;
    },
    get: function () {
        this.method = "GET";
        let resp = http_init.call( this );
        parse_response.call( this, resp );
        delete resp;
    },
    post: function ( body ) {
        this.method = "POST";
        let resp = http_init.call( this, prepare_post_data.call( this, body ) ); delete body;
        parse_response.call( this, resp );
        delete resp;
    },
    send: function ( body ) {
        this.method = "POST";
        let resp = http_init.call( this, prepare_post_data.call( this, body ) ); delete body;
        parse_response.call( this, resp );
        delete resp;
    },
    move_to_request: function ( with_header ) {
        if ( 'number' === typeof ( this.response.http_status_code ) && this.response.http_status_code > 0 ) {
            this.cookie = [];
            for ( let part; this.response.cookie.length && ( part = this.response.cookie.shift() ); ) {
                this.cookie.push( part );
            }
        }
        if ( !with_header )
            this.header = {};
        return this;
    },
    clear_response: function () {
        delete this.response;
        this.url = undefined;
        this.method = undefined;
        this.header = {};
        this.cookie = [];
        return this;
    }
} );
module.exports = http_request;
//8:23 PM 12/2/2018
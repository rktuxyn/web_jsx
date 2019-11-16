//3:25 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
( typeof ( Object.extend ) === 'function' ? undefined : ( Object.extend = function ( destination, source ) {
	for ( var property in source )
		destination[property] = source[property];
	return destination;
} ) );
function getAuthHeader( header ) {
    if ( !header ) throw "Invalid request!!!";
    let map = {
        "HTTP_ORIGIN": "origin"
    };
    let out = {};
    header.split( "~" ).find( ( a ) => {
        let ar = a.split( "=" ), val;
        if ( !map[ar[0]] ) return;
        val = ar[1];
        out[map[ar[0]]] = val === void 0 || !val ? null : ar[1].trim();
    } );
    return out;
};
function writeCrossHeader( ctx, origin ) {
    ctx.response.header( "Access-Control-Allow-Origin", origin || "*" );
    ctx.response.header( "Access-Control-Allow-Credentials", "true" );
    ctx.response.header( "Access-Control-Allow-Methods", "GET" );
    //caps at 2 hours (7200 seconds)
    //https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Access-Control-Max-Age
    ctx.response.header( "Access-Control-Max-Age", "7200" );
	//https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS#Access-Control-Expose-Headers
	ctx.response.header( "Access-Control-Expose-Headers", "x-access-token, x-auth-token, x-http-status-code, Content-Type, Pragma, Content-Transfer-Encoding" );
    ctx.response.header( "Access-Control-Allow-Headers", "x-mac-addr, x-access-token, x-auth-token, x-device-info, x-http-status-code, Content-Type, Accept, X-Requested-With, remember-me" );
    //NotAcceptable
    return true;
};
function getCookie( httpCookie, name ) {
    if ( !httpCookie || !name ) throw "Invalid request!!!";
    let arr = httpCookie.split( ";" );
    if ( arr === null || !arr ) return undefined;
    for ( let row of arr ) {
        let rarr = row.split( "=" );
        let key = rarr[0];
        if ( !key ) continue;
        if ( key.trim() !== name ) continue;
        return String( rarr[1] ).trim();
    }
    return undefined;
};
let { CryptoJS } = require( "/addon/aes.js" );
module.exports = ( function () {
	var _user_info = { is_authenticated: false, move_next: true };
    return {
        init: function ( ctx, config ) {
            if ( !ctx || 'object' !== typeof ( ctx ) )
                throw "context required!!!";
            if ( !config || 'object' !== typeof ( config ) )
                throw "config required!!!";
            if ( !ctx.request.cookie ) return;
            if ( !config.auth_cookie ) config.auth_cookie = "web_jsx_session";
            let cook = getCookie( ctx.request.cookie, config.auth_cookie );

            if ( !cook || cook == null ) return;
            _user_info.remote_address = ctx.remote_addr;
			_user_info.login_id = cook;
			Object.extend(_user_info, getAuthHeader( ctx.request.header ) );
            try {
                if ( _user_info.user_data ) {
                    _user_info.session = JSON.parse( _user_info.user_data );
                    delete _user_info.user_data;
                    _user_info.session.session_id = getCookie( ctx.request.cookie, "_sess_act" );
                }
            } catch ( e ) { }
			
			if (_user_info.origin.indexOf( ctx.host ) > -1 ) return this;
			
            return writeCrossHeader( _user_info.origin ), this;
		},
		get move_next() {
			if ( !_user_info ) return false;
			return _user_info.move_next;
		},
        isAuthenticated: function () {
            if ( !_user_info ) return false;
            return _user_info.is_authenticated;
        },
        getUserInfo: function () {
            return _user_info || {};
        },
        clear: function () {
            _user_info = { is_authenticated: false };
            return this;
        },
        authenticate: function ( ctx, config, loginid ) {
            if ( !ctx || 'object' !== typeof ( ctx ) )
                throw "context required!!!";
            if ( !config || 'object' !== typeof ( config ) )
                throw "config required!!!";
            if ( !config.auth_cookie ) config.auth_cookie = "web_jsx_session";
            if ( ctx.https ) {
                ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${loginid};` );
                return this;
            }
            ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${loginid};` );
            return this;
        }
    };
}() );
//8:08 AM 11/30/2018
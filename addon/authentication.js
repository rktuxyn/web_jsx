//3:25 AM 11/30/2018
/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
    ctx.response.header( "Access-Control-Max-Age", "7200" );
    ctx.response.header( "Access-Control-Expose-Headers", "x-access-token, x-auth-token, x-http-status-code, Content-Type, Pragma, Content-Transfer-Encoding" );
    ctx.response.header( "Access-Control-Allow-Headers", "x-mac-addr, x-access-token, x-auth-token, x-device-info, x-http-status-code, Content-Type, Accept, X-Requested-With, remember-me" );
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
//let { CryptoJS } = require( "/addon/aes.js" );
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
            _user_info = { is_authenticated: false, move_next: true };
            _user_info.remote_address = ctx.remote_addr;
            let arr = cook.split( "~" );
            Object.extend( _user_info, getAuthHeader( ctx.request.header ) );
            if ( arr !== null ) {
                _user_info.is_authenticated = true;
                let keys = ["login_id", "user_data"];
                for ( let i = 0, l = arr.length; i < l; i++ ) {
                    _user_info[keys[i]] = arr[i];
                }
                try {
                    if ( _user_info.user_data ) {
                        _user_info.session = JSON.parse( _user_info.user_data );
                        delete _user_info.user_data;
                        _user_info.session.session_id = getCookie( ctx.request.cookie, "_sess_act" );
                    }
                } catch ( e ) { }
            }
           
            if ( _user_info.origin.indexOf( ctx.host ) > -1 ) return this;
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
        authenticate: function ( ctx, config, loginid, userData ) {
            if ( !ctx || 'object' !== typeof ( ctx ) )
                throw "context required!!!";
            if ( !config || 'object' !== typeof ( config ) )
                throw "config required!!!";
            if ( !config.auth_cookie ) config.auth_cookie = "web_jsx_session";
            if ( ctx.https ) {
                ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${loginid}~${userData};` );
                return this;
            }
            ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${loginid}~${userData};` );
            return this;
        }
    };
}() );
//8:08 AM 11/30/2018

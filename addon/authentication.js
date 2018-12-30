//3:25 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
function getCookie( httpCookie, name ) {
    if ( !httpCookie || !name ) throw "Invalid request!!!";
    let arr = httpCookie.split( ";" );
	if( arr === null || !arr ) return undefined;
    for ( let row of arr ) {
        let rarr = row.split( "=" );
		let key = rarr[0];
		if( !key )continue;
        if ( key.trim() !== name ) continue;
        return String( rarr[1] ).trim();
    }
    return undefined;
};
/**Date.prototype.addHours = function ( h ) {
    this.setHours( this.getHours() + h );
    return this;
};*/
module.exports = ( function () {
    var _user_info = undefined;
    return {
        init: function ( ctx, config ) {
			if( !ctx || 'object' !== typeof( ctx ) )
				throw "context required!!!";
			if( !config || 'object' !== typeof( config ) )
				throw "config required!!!";
            if ( !ctx.http_cookie ) return;
			if( !config.auth_cookie ) config.auth_cookie = "web_jsx_session";
            let cook = getCookie( ctx.http_cookie, config.auth_cookie );
            if ( !cook || cook == null ) return;
            let dec = crypto.decrypt( cook, config.crypto.key, config.crypto.iv );
            let arr = dec.split( "~" );
            if ( arr === null ) {
                _user_info = { is_authenticated: false };
                return;
            }
            let keys = ["login_id", "role_id", "user_data"];
            _user_info = { is_authenticated: true };
            for ( let i = 0, l = arr.length; i < l; i++ ) {
                _user_info[keys[i]] = arr[i];
            }
            _user_info.remote_address = ctx.remote_addr;
            return this;
        },
        isAuthenticated: function () {
            if ( !_user_info ) return false;
            return _user_info.is_authenticated;
        },
        getUserInfo: function () {
            return _user_info;
        },
        clear: function () {
            _user_info = undefined;
            return this;
        },
        authenticate: function ( ctx, config, user_id, role, user_data ) {
			if( !ctx || 'object' !== typeof( ctx ) )
				throw "context required!!!";
			if( !config || 'object' !== typeof( config ) )
				throw "config required!!!";
            let enc = crypto.encrypt( `${user_id}~${role}~${user_data}`, config.crypto.key, config.crypto.iv );
            //let dt = new Date().addHours( 24 ).toString().split( "(" )[0].trim();
            if ( !config.auth_cookie ) config.auth_cookie = "web_jsx_session";
            if ( ctx.https ) {
                ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${enc};` );
                return;
            }
            ctx.response.cookie( `Set-Cookie: ${config.auth_cookie}=${enc};` );
            return this;
        }
    };
}() );
//8:08 AM 11/30/2018
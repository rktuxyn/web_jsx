//3:25 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
module.exports = (function(){
	return {
		auth_cookie:"web_jsx_session",
		database:{
			module:"pgsql",
            db_conn:"Server=localhost; Port=XXXX; UserId=XXXX;Password=XXXX;Database=sow_sys; keepalive=10; CommandTimeout=100000;"
        },
		crypto:{
            key: "YXV0aGVudGljYXRlX2tleQ==", /* A 256 bit key */
        }
	};
}() );
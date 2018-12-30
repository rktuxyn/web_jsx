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
            db_conn:"Server=localhost; Port=5432; UserId=postgres;Password=xxxx;Database=sow; keepalive=10; CommandTimeout=100000;"
        },
        production: false,
		crypto:{
			key:"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", /* A 256 bit key */
			iv:"XXXXXXXXXXXXXXXX"/* A 128 bit IV */
		}
	};
}() );
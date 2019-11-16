/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
class system {
    static kill_process_by_name( p_name ) {
        try {
            return sys.kill_process_by_name( p_name );
        } catch ( e ) {
            return -1;
        }
    }
    static create_process( settings ) {
        if ( settings === null || typeof ( settings ) !== "object" )
            throw new Error( "Settings not defined!!!" );
        let pid = 0, max = 10, error="";
        while ( true ) {
            if ( max < 0 ) break;
            try {
                pid = sys.create_process( settings );
                if ( pid > 0 ) break;
            } catch ( e ) {
                max--;
				error += e.message +"\n";
                continue;
            }
        }
        return { pid, max, error };
    }
}
module.exports = {
    system: system
};

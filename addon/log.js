/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//--7:52 PM 3/31/2019
var dfo = ( t ) => {
    t = t === 0 ? 1 : t;
    return t <= 9 ? "0" + t : t;
}, dfm = ( t ) => {
    t += 1;
    return t <= 9 ? "0" + t : t;
}, getLocalDateTime = ( offset ) => {
    // create Date object for current location
    let d = new Date();
    // convert to msec
    // subtract local time zone offset
    // get UTC time in msec
    let utc = d.getTime() + ( d.getTimezoneOffset() * 60000 );
    // create new Date object for different city
    // using supplied offset
    let nd = new Date( utc + ( 3600000 * offset ) );
    // return time as a string
    return nd;
}, getTime = ( tz ) => {
    let date = getLocalDateTime( tz );
    return `${date.getFullYear()}-${dfm( date.getMonth() )}-${dfo( date.getDate() )} ${dfo( date.getHours() )}:${dfo( date.getMinutes() )}:${dfo( date.getSeconds() )}`;
};
const MAX_LINE = 100;//Maximum write line in memmory stream
class Log {
    constructor( dir, name, wd, tz ) {
        if ( !dir || "string" !== typeof ( dir ) )
            throw new TypeError( "Invalid argument defined!!!" );
        if ( !name || "string" !== typeof ( name ) )
            throw new TypeError( "Invalid argument defined!!!" );
        if ( !tz ) tz = '+6';
        this.is_dispose = false;
        dir = dir.replace( /\//gi, "\\" );
        name = name.replace( /\//gi, "_" );
        if ( wd === true ) {
            let date = getLocalDateTime( tz );
            name = `${name}_${date.getFullYear()}_${dfm( date.getMonth() )}_${dfo( date.getDate() )}.log`;
        }
        sys.create_directory( dir );
        let absolute = dir + name;
        let is_new = fs.exists_file( absolute );
        this.file = new fs.file( absolute, "a" ); //a+
        if ( is_new ) {
            this.file.write( `Log Genarte On ${getTime( tz )}\r\n-------------------------------------------------------------------\r\n` );
        }
        this.tz = tz;
        this._line_count = 0;
        this._reOpen = function () {
            this.file = new fs.file( absolute, "a" );//a+
        };
        this.dispose = function ( ) {
            delete this.file;
            this.is_dispose = true;
            delete this.dispose;
            delete this.tz;
        };
    }
    newLine() {
        if ( this.is_dispose )
            throw new Error( "Instance already disposed!!!" );
        this.file.write( '-------------------------------------------------------------------\r\n' );
    }
    write( buffer ) {
        if ( this.is_dispose )
            throw new Error( "Instance already disposed!!!" );
        buffer = `${getTime( this.tz )}\t${buffer}`;
        this.file.write( `${buffer}\r\n` );
        this._line_count++;
        if ( this._line_count > MAX_LINE ) {
            this.file.flush();
            delete this.file;
            this._reOpen();
        }
        return buffer;
    }
    close() {
        if ( this.is_dispose )
            throw new Error( "Instance already disposed!!!" );
        this.file.flush();
        this.dispose( );
    }
}
module.exports = Log;

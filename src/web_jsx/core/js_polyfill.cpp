/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "js_polyfill.h"
void add_common_func(std::stringstream& js_stream) {
	js_stream << "String.format = function ( format ) {if ( typeof ( format ) !== 'string' ) throw new Error(\"String required!!!\"); let args = Array.prototype.slice.call( arguments, 1 ); let len = args.length - 1; return format.replace( /{(\\d+)}/g, function ( match, number )  { let index = parseInt( number ); if ( isNaN( index ) ) throw new Error( \"Invalid param index!!!\" ); if ( index > len ) throw new Error( \"Index should not greater than \" + len + format + JSON.stringify( args ) ); return typeof ( args[index] ) !== 'undefined' ? args[number] : \"\"; } ); };\n";
}
void sow_web_jsx::js_write_header(std::stringstream& js_stream) {
	add_common_func(js_stream);
	js_stream << "context.app_path = ( function () { if ( context.app_path === undefined || context.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return context.app_path.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.app_dir = ( function () { if ( context.app_dir === undefined || context.app_dir === null ) { throw new Error(\"App directory not found in current context!!!\"); } return context.app_dir.replace( /\\\\/g,'/' ).replace(/\\/\\//gi, '/'); }());\n";
	js_stream << "context.env_path = ( function () { if ( context.env_path === undefined || context.env_path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return context.env_path.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.root_dir = (function () { if ( context.root_dir === undefined || context.root_dir === null ) { throw \"Root directory not found in current context!!!\"; } return context.root_dir.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.https = context.https===\"on\" || context.https===\"true\" ? true : false;\n";
	js_stream << "context.request.protocol= context.https===true ? \"https:\" : \"http:\";\n";
	js_stream << "context.host_url = context.request.protocol + \"//\" + context.host;\n";
	js_stream << "context.request.query_string = ( function () { try { return JSON.parse( (context.request.query_string) ); } catch ( e ) { return {}; } }() );\n";
	js_stream << "context.request.content_length = (function(){ let len = parseInt(context.request.content_length); return isNaN(len) ? 0 : len; }())\r\n";
	js_stream << "context.request.read_payload = function read_payload( cb ) { return this.method !== 'POST'? -1 : this._read_payload(this.content_length, this.content_type, cb);}\r\n";
	js_stream << "context.request.write_file_from_payload = function write_file_from_payload( dir ) { if ( this.method !== 'POST' ) throw new Error('Allowed only over POST data.'); return this._write_file_from_payload( context.request.content_length, this.content_type, dir ); }\r\n";
	js_stream << "context.response.write = function write( val, nline ) { if ( !val || val === null || val === undefined ) return this;  !1 === nline ? context.response._write( val ) : context.response._write( val + '\\r\\n' ); return this;};\r\n";
	js_stream << "context.response.write_p = function write_p( val ) { if ( !val || val === null || val === undefined ) return this; context.response._write( val ); return this;};\r\n";
	js_stream << "context.server_protocol = context.server_protocol === null || context.server_protocol === '' || !context.server_protocol ? 'HTTP/1.1' : context.server_protocol;\n";
#if defined(FAST_CGI_APP)
	js_stream << "context.is_fcgi = true;\r\n";
	js_stream << "context.response.redirect = function redirect( url, force ) { if ( !url ) { throw \"Redirect location required!!!\"; } context.response._redirect( ( force === true ? ( context.https ? \"https://\" : \"http://\" ) + ( context.host + url.trim() ) : url.trim() ) ); };\r\n";
	js_stream << "context.response.status = function status( status_code, desc ) { context.response._status( status_code, desc ); };\r\n";
#else
	js_stream << "context.is_fcgi = false;\r\n";
	js_stream << "context.response.redirect = function redirect( url, force ) { if ( !url ) { throw \"Redirect location required!!!\"; } context.response._redirect( context.server_protocol, ( force === true ? ( context.https ? \"https://\" : \"http://\" ) + ( context.host + url.trim() ) : url.trim() ) ); };\r\n";
	js_stream << "context.response.status = function status( status_code, desc ) { context.response._status( context.server_protocol, status_code, desc ); };\r\n";
#endif//!FAST_CGI_APP
	js_stream << "context.response.cookie = function ( name, value, data ) {	if ( !name ) throw new TypeError( 'Cookie name required!!!' );	if ( arguments.length === 1 ) return this._cookie( name );	if ( !value ) throw new TypeError( 'Cookie value required!!!' );	if ( data ) {		return this._cookie( `Set-Cookie:${name}=${value} ${data};` );	}	return this._cookie( `Set-Cookie:${name}=${value};` );};\r\n";
	js_stream << "context.response.status(\"200\", \"OK\");\n";
	js_stream << "context.response.as_gzip = function as_gzip() { if ( !context.request.accept_encoding ) return -1; if ( context.request.accept_encoding.indexOf( 'gzip' ) < 0 ) return -1; this._as_gzip(); return 1; };\r\n";
	js_stream << "context.response.header(\"Content-Type\", \"text/html; charset=utf-8\");\n";
	js_stream << "context.response.header(\"Connection\", \"Keep-Alive\");\n";
	js_stream << "sys.load_native_module();\n";//Impliment native module according to module.cfg
}
void sow_web_jsx::js_write_console_header(std::stringstream& js_stream) {
	add_common_func(js_stream);
	js_stream << "this.__print = function ( str ) { if ( env.is_interactive === false ) return; print( String.format( str, Array.prototype.slice.call( arguments, 1 ) ) ); return; };\n";
	js_stream << "env.app_dir = ( function () { if ( env.app_dir === undefined || env.app_dir === null ) { throw new Error(\"App directory not found in current context!!!\"); } return env.app_dir.replace( /\\\\/g,'/' ).replace(/\\/\\//gi, '/'); }());\n";
	js_stream << "env.app_path = ( function () { if ( env.app_path === undefined || env.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return env.app_path.replace( /\\\\/g,'/' ); }());\n";
	js_stream << "env.root_dir = ( function () { if ( env.root_dir === undefined || env.root_dir === null ) { throw new Error(\"Root directory not found in current context!!!\"); } return env.root_dir.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "env.path_translated = ( function () { if ( env.path_translated === undefined || env.path_translated === null ) { throw new Error(\"Translated path not found in current context!!!\"); } return env.path_translated.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "env.path = ( function () { if ( env.path === undefined || env.path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return env.path.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "env.arg = ( function () { try { return JSON.parse( env.arg ); } catch ( e ) { __print( e.message ); return []; } }() );\n";
	js_stream << "sys.load_native_module();\n";//Impliment native module according to module.cfg
}

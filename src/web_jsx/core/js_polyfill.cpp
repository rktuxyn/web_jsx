/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "js_polyfill.h"
#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE
#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE
void sow_web_jsx::create_wj_core_extend_script(std::stringstream& js_stream, int is_cli) {
	js_stream << "var __extend = function( dest, source, is_first ) {\n";
	js_stream << "	if ( typeof ( dest ) !== \"object\" && ( typeof( is_first ) === \"boolean\" && is_first === true ) ) {\n";
	js_stream << "		dest = this;\n";
	js_stream << "	}\n";
	js_stream << "	if ( typeof ( source ) !== \"object\" ) {\n";
	js_stream << "		throw new Error( `__extend::Source should be object... Source type::${typeof( source )}` );\n";
	js_stream << "	}\n";
	js_stream << "	for ( let prop in source ) {\n";
	if (is_cli == FALSE) {
		js_stream << "		if ( prop === \"request\" ) {\n";
		js_stream << "			__extend( dest[prop], source[prop] );\n";
		js_stream << "			continue;\n";
		js_stream << "		}\n";
	}
	js_stream << "		dest[prop] = source[prop];\n";
	js_stream << "	}\n";
	js_stream << "}\n";
	js_stream << "function async_func( func ) {\n";
	js_stream << "	var args = Array.prototype.slice.call( arguments, 1 );\n";
	js_stream << "	return new Promise( ( resolve, reject ) => {\n";
	js_stream << "		resolve( func.apply( this, args ) );\n";
	js_stream << "	} );\n";
	js_stream << "}\n";
	js_stream << "function __reset_object_value(object, prop, value) {\n";
	js_stream << "	Object.defineProperty( object, prop, {\n";
	js_stream << "		enumerable: true,\n";
	js_stream << "		writable: false,\n";
	js_stream << "		configurable: true,\n";
	js_stream << "		value: value\n";
	js_stream << "	} )\n";
	js_stream << "}\n";

	js_stream << "String.format = function ( format ) { if ( typeof ( format ) !== 'string' ) throw new Error(\"String required!!!\"); let args = Array.prototype.slice.call( arguments, 1 ); let len = args.length - 1; return format.replace( /{(\\d+)}/g, function ( match, number )  { let index = parseInt( number ); if ( isNaN( index ) ) throw new Error( \"Invalid param index!!!\" ); if ( index > len ) throw new Error( \"Index should not greater than \" + len + format + JSON.stringify( args ) ); return typeof ( args[index] ) !== 'undefined' ? args[number] : \"\"; } ); };\n";
}
void prepare_native_module(std::stringstream& js_stream, int is_cli = TRUE) {
	js_stream << "__implimant_native_module( ); delete this[\"__implimant_native_module\"];\n";//Impliment native module according to module.cfg
	js_stream << "if ( typeof( this[\"__wj_core\"] ) !== \"object\" ) throw new Error(\"Please add wj_core.dll\");\n";
	sow_web_jsx::create_wj_core_extend_script(js_stream, is_cli);
	if (is_cli == FALSE) {
		js_stream << "__extend( context, __wj_core.context, true );\n";
	}
	else {
		js_stream << "if( typeof( __wj_core.context ) !== \"object\" ) delete __wj_core.context;";
	}
	js_stream << "__extend( this, __wj_core.global, true );\n";
	if (is_cli == FALSE) {
		js_stream << "this[\"__async\"] = async_func;\n";
		js_stream << "this[\"__async_t\"] = async_func;\n";
		js_stream << "sys.async_thread = async ( funcs ) => {\n";
		js_stream << "	if ( !Array.isArray( funcs ) )\n";
		js_stream << "		throw new TypeError( \"Function Array required...\" );\n";
		js_stream << "	for ( let func of funcs ) {\n";
		js_stream << "		await async_func( func );\n";
		js_stream << "	}\n";
		js_stream << "}\n";
		js_stream << "this[\"setTimeout\"] = async ( handler, timeout ) => {\n";
		js_stream << "	if ( typeof ( handler ) !== \"function\" )\n";
		js_stream << "		throw new TypeError( \"handler should be function\" );\n";
		js_stream << "	await async_func( () => {\n";
		js_stream << "		__sleep( timeout );\n";
		js_stream << "		handler();\n";
		js_stream << "	} );\n";
		js_stream << "}\n";

	}
}
void sow_web_jsx::js_write_header(std::stringstream& js_stream) {
	js_stream << "/*__web_jsx_script__*/\n";
	js_stream << "if( typeof( context ) !== \"object\" ) throw new Error(\"context didn't initialized yet.\");\n";
	::prepare_native_module(js_stream, FALSE);
	js_stream << "context.app_path = ( function () { if ( context.app_path === undefined || context.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return context.app_path.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.app_dir = ( function () { if ( context.app_dir === undefined || context.app_dir === null ) { throw new Error(\"App directory not found in current context!!!\"); } return context.app_dir.replace( /\\\\/g,'/' ).replace(/\\/\\//gi, '/'); }());\n";
	js_stream << "context.env_path = ( function () { if ( context.env_path === undefined || context.env_path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return context.env_path.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.root_dir = (function () { if ( context.root_dir === undefined || context.root_dir === null ) { throw \"Root directory not found in current context!!!\"; } return context.root_dir.replace( /\\\\/g,'/' ) }());\n";
	js_stream << "context.https = context.https === \"on\" || context.https === \"true\" ? true : false;\n";
	js_stream << "context.request.protocol = context.https === true ? \"https:\" : \"http:\";\n";
	js_stream << "context.host_url = context.request.protocol + \"//\" + context.host;\n";
	//js_stream << "context.request.header = ( function () { try { return JSON.parse( context.request.header.replace( /\\\\/gi, \"/\" ) ); } catch ( e ) { return []; } }() );\n";
	js_stream << "context.request.query_string = ( function () { try { return JSON.parse( (context.request.query_string) ); } catch ( e ) { return {}; } }() );\n";
	js_stream << "context.request.content_length = (function(){ let len = parseInt(context.request.content_length); return isNaN(len) ? 0 : len; }());\n";
	js_stream << "context.request.read_payload = function read_payload( cb ) { return this.method !== 'POST'? -1 : this._read_payload(this.content_length, this.content_type, cb);}\r\n";
	js_stream << "context.request.write_file_from_payload = function write_file_from_payload( dir ) { if ( this.method !== 'POST' ) throw new Error('Allowed only over POST data.'); return this._write_file_from_payload( context.request.content_length, this.content_type, dir ); }\n";
	js_stream << "context.server_protocol = context.server_protocol === null || context.server_protocol === '' || !context.server_protocol ? 'HTTP/1.1' : context.server_protocol;\n";
	js_stream << "context.is_fcgi = true;\n";
	js_stream << "context.response.write = function write( val, nline ) { if ( !val || val === null || val === undefined ) return this;  !1 === nline ? context.response._write( val ) : context.response._write( val + '\\r\\n' ); return this;};\n";
	js_stream << "context.response.write_p = function write_p( val ) { if ( !val || val === null || val === undefined ) return this; context.response._write( val ); return this;};\r\n";
	js_stream << "context.response.redirect = function redirect( url, force ) { if ( !url ) { throw \"Redirect location required!!!\"; } context.response._redirect( ( force === true ? ( context.https ? \"https://\" : \"http://\" ) + ( context.host + url.trim() ) : url.trim() ) ); };\n";
	js_stream << "context.response.status = function status( status_code, desc ) { context.response._status( status_code, desc ); };\n";
	js_stream << "context.response.cookie = function ( name, value, data ) {	if ( !name ) throw new TypeError( 'Cookie name required!!!' );	if ( arguments.length === 1 ) return this._cookie( name );	if ( !value ) throw new TypeError( 'Cookie value required!!!' );	if ( data ) {		return this._cookie( `Set-Cookie:${name}=${value} ${data};` );	}	return this._cookie( `Set-Cookie:${name}=${value};` );};\n";
	//js_stream << "__implimant_native_module( /*need_context*/ true);\n";//Extend web_jsx global context
	js_stream << "context.response.status(\"200\", \"OK\");\n";
	js_stream << "context.response.as_gzip = function as_gzip() { if ( !context.request.accept_encoding ) return -1; if ( context.request.accept_encoding.indexOf( 'gzip' ) < 0 ) return -1; this._as_gzip(); return 1; };\r\n";
	js_stream << "context.response.header(\"Content-Type\", \"text/html; charset=utf-8\");\n";
	js_stream << "context.response.header(\"Connection\", \"Keep-Alive\");\n";
}
void sow_web_jsx::js_write_console_header(std::stringstream& js_stream) {
	js_stream << "/*__web_jsx_script__*/\n";
	::prepare_native_module(js_stream);
	js_stream << "this.__print = function ( str ) { if ( env.is_interactive === false ) return; print( String.format( str, Array.prototype.slice.call( arguments, 1 ) ) ); return; };\n";
	js_stream << "__reset_object_value( env, \"app_dir\", ( function () { if ( env.app_dir === undefined || env.app_dir === null ) { throw new Error(\"App directory not found in current context!!!\"); } return env.app_dir.replace( /\\\\/g,'/' ).replace(/\\/\\//gi, '/'); }()) );\n";
	js_stream << "__reset_object_value( env, \"app_path\", ( function () { if ( env.app_path === undefined || env.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return env.app_path.replace( /\\\\/g,'/' ); }()) );\n";
	js_stream << "__reset_object_value( env, \"root_dir\", ( function () { if ( env.root_dir === undefined || env.root_dir === null ) { throw new Error(\"Root directory not found in current context!!!\"); } return env.root_dir.replace( /\\\\/g,'/' ) }()) );\n";
	js_stream << "__reset_object_value( env, \"path_translated\", ( function () { if ( env.path_translated === undefined || env.path_translated === null ) { throw new Error(\"Translated path not found in current context!!!\"); } return env.path_translated.replace( /\\\\/g,'/' ) }()) );\n";
	js_stream << "__reset_object_value( env, \"path\", ( function () { if ( env.path === undefined || env.path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return env.path.replace( /\\\\/g,'/' ) }()) );\n";
	js_stream << "__reset_object_value( env, \"arg\", ( function () { try { return JSON.parse( env.arg ); } catch ( e ) { __print( e.message ); return []; } }() ) );\n";
}

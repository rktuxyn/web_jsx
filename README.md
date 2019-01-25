# web_jsx
web_jsx *.jsx, *.jsxh (back-end Javascript) web extension handler (Run with IIS, Apache, Nginx)<br/>
<br/>
Read POST data
```javascript
let payload = "";
context.request.read_payload( function ( buff, count ) {
	payload += buff;
} );
```
Write payload to file<br/>
```javascript
let c = fs.write_file_from_payload( "post_data.txt" );
```
Read data from Postgres SQL<br/>
```javascript
let resp = npgsql.execute_io("Server=localhost; Port=5432; UserId=postgres;Password=1##$1@6Z;Database=sow; keepalive=10; CommandTimeout=100000;", 
	"__sql_execute", JSON.stringify( {} ), JSON.stringify( {
		"sql":"select * from jsx.community"
	} )
);
__print(JSON.stringify(resp));
```
#Global Web Request Context
```json
{
   "context":{
      "host":"sow.pc",
      "http_accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
      "http_accept_encoding":"gzip, deflate, br",
      "http_cookie":"sow=rktuxyn; web_jsx_session=188EAD362DBDCC20C96D99E25CBDDE5A6371E1C073D49C258BEDABD6DCF0141A",
      "http_user_agent":"Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36",
      "remote_addr":"127.0.0.1",
      "root_dir":"D:\\___PROJECT\\__SOW\\",
      "server_protocol":"HTTP/1.1",
      "request":{
         "content_length":"0",
         "content_type":"text/html",
         "method":"GET",
         "page_path":"/context.jsxh",
         "pay_load":"[]",
         "query_string":{

         }
      },
      "response":{
         "_write":"function _write() { [native code] }",
         "write":"function write( str ) { }",
         "header":"function header( key, value ) { [native code] }",
         "write_from_file":"function write_from_file( path ) { [native code] }",
         "body":{
            "flush":"function flush() { [native code] }",
            "write_as_pdf":"function write_as_pdf() { [native code] }"
         },
         "clear":"function clear() { [native code] }",
         "status":"function status( server_protocol, status_code, msg ) { [native code] }",
         "cookie":"function cookie( cook ) { [native code] }",
         "_redirect":"function redirect( url, force ) { [native code] }",
         "redirect":"function redirect( url, force ) {  }"
      }
   },
   "fs":{
      "read_file":"function read_file( path ) { [native code] }",
      "write_file":"function write_file( path ) { [native code] }",
      "read_directory":"function read_directory( dir, pattern ) { [native code] }",
      "read_directory_regx":"function read_directory_regx( dir, regx_pattern ) { [native code] }"
   },
   "npgsql":{
      "execute_io":"function execute_io( con_str, sp_str, context_json, form_json ) { [native code] }",
      "execute_scalar":"function execute_scalar() { [native code] }",
      "execute_non_query":"function execute_non_query() { [native code] }",
      "data_reader":"function data_reader( con_str, stmt_str, callback ) { [native code] }"
   },
   "native_pdf":{
      "generate_pdf":"function generate_pdf( config, body ) { [native code] }"
   },
   "crypto":{
      "encrypt":"function encrypt( plain_text, key_str, iv_str ) { [native code] }",
      "decrypt":"function decrypt( encrypted_text, key_str, iv_str ) { [native code] }",
      "encrypt_source":"function encrypt_source( plain_text ) { [native code] }",
      "decrypt_source":"function decrypt_source( encrypted_text ) { [native code] }"
   },
   "create_http_request":"function create_http_request( req_object ) { [native code] }",
   "require":"function require( source_path, is_encrypted, encrypt_key ) { [native code] }",
   "__get_response_body":"function __get_response_body() { [native code] }",
   "__create_directory":"function __create_directory( path_str ) { [native code] }",
   "__async":"function __async( callback ) { [native code] }"
}
```

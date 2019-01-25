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
#HTTP Context
```json
{
   "context":{
      "host":"sow.pc",
      "remote_addr":"127.0.0.1",
      "root_dir":"D:\\___PROJECT\\__SOW\\",
      "server_protocol":"HTTP/1.1",
      "request":{
         "accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
         "accept_encoding":"gzip, deflate, br",
         "content_type":"text/html",
         "cookie":"web_jsx_session=188EAD362DBDCC20C96D99E25CBDDE5A6371E1C073D49C258BEDABD6DCF0141A;",
         "method":"GET",
         "page_path":"/context.jsxh",
         "pay_load":"[]",
         "query_string":{

         },
         "user_agent":"Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36",
         "_read_payload":"function _read_payload() { [native code] }",
         "protocol":"https:",
         "read_payload":"function read_payload( cb ) { }"
      },
      "user":{
         "is_authenticated":"false",
         "name":"",
         "role":""
      },
      "response":{
         "_write":"function _write() { [native code] }",
         "header":"function header() { [native code] }",
         "write_from_file":"function write_from_file() { [native code] }",
         "body":{
            "flush":"function flush() { [native code] }",
            "write_as_pdf":"function write_as_pdf() { [native code] }"
         },
         "clear":"function clear() { [native code] }",
         "status":"function status() { [native code] }",
         "cookie":"function cookie() { [native code] }",
         "_redirect":"function _redirect() { [native code] }",
         "_as_gzip":"function _as_gzip() { [native code] }",
         "write":"function write( val, nline ) { }",
         "write_p":"function write_p( val ) { }",
         "redirect":"function redirect( url, force ) {  }",
         "as_gzip":"function as_gzip() {  }"
      },
      "host_url":"https://sow.pc"
   },
   "fs":{
      "read_file":"function read_file() { [native code] }",
      "write_file":"function write_file() { [native code] }",
      "_write_file_from_payload":"function _write_file_from_payload() { [native code] }",
      "read_directory":"function read_directory() { [native code] }",
      "read_directory_regx":"function read_directory_regx() { [native code] }",
      "write_file_from_payload":"function write_file_from_payload( path ) {  }"
   },
   "npgsql":{
      "execute_io":"function execute_io() { [native code] }",
      "execute_scalar":"function execute_scalar() { [native code] }",
      "execute_non_query":"function execute_non_query() { [native code] }",
      "data_reader":"function data_reader() { [native code] }"
   },
   "native_pdf":{
      "generate_pdf":"function generate_pdf() { [native code] }"
   },
   "crypto":{
      "encrypt":"function encrypt() { [native code] }",
      "decrypt":"function decrypt() { [native code] }",
      "encrypt_source":"function encrypt_source() { [native code] }",
      "decrypt_source":"function decrypt_source() { [native code] }"
   },
   "create_http_request":"function create_http_request() { [native code] }",
   "require":"function require() { [native code] }",
   "__get_response_body":"function __get_response_body() { [native code] }",
   "__create_directory":"function __create_directory() { [native code] }",
   "__async":"function __async() { [native code] }",
   "__sleep":"function __sleep() { [native code] }"
}
```

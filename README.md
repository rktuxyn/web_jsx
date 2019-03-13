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
#Content-Encoding Gzip response 
```javascript
context.response.as_gzip()
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
  "context": {
    "app_path": "C:/web_jsx/web_jsx_cgi.exe",
    "env_path": "C:/web_jsx/;",
    "host": "www.safeonline.world",
    "remote_addr": "127.0.0.1",
    "root_dir": "D:/__SOW/",
    "server_protocol": "HTTP/2.2",
    "request": {
      "accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
      "accept_encoding": "gzip, deflate, br",
      "content_type": "text/html",
      "cookie": "web_jsx_session=U2FsdGVkX1+6UnnLIs3cASo6tHlVr3cqXbHIuLlz4r/mPyj6klFg+L9HJfwJY0aB;",
      "method": "GET",
      "page_path": "/context.jsxh",
      "pay_load": "[]",
      "query_string": {

      },
      "user_agent": "Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36",
      "_read_payload": "function _read_payload() { [native code] }",
      "protocol": "https:",
      "read_payload": "function read_payload( cb ) { }"
    },
    "user": {
      "is_authenticated": "false",
      "name": "",
      "role": ""
    },
    "response": {
      "_write": "function _write() { [native code] }",
      "header": "function header() { [native code] }",
      "write_from_file": "function write_from_file() { [native code] }",
      "body": {
        "flush": "function flush() { [native code] }",
        "write_as_pdf": "function write_as_pdf() { [native code] }"
      },
      "clear": "function clear() { [native code] }",
      "status": "function status() { [native code] }",
      "cookie": "function cookie() { [native code] }",
      "_redirect": "function _redirect() { [native code] }",
      "_as_gzip": "function _as_gzip() { [native code] }",
      "write": "function write( val, nline ) { }",
      "write_p": "function write_p( val ) { }",
      "redirect": "function redirect( url, force ) {  }",
      "as_gzip": "function as_gzip() {  }"
    },
    "host_url": "https://www.safeonline.world/"
  },
  "fs": {
    "read_file": "function read_file() { [native code] }",
    "write_file": "function write_file() { [native code] }",
    "_write_file_from_payload": "function _write_file_from_payload() { [native code] }",
    "write_file_from_payload": "function write_file_from_payload( path ) {  }"
  },
  "sys": {
    "read_directory": "function read_directory() { [native code] }",
    "read_directory_regx": "function read_directory_regx() { [native code] }",
    "create_directory": "function create_directory() { [native code] }",
    "delete_directory": "function delete_directory() { [native code] }",
    "create_process": "function create_process() { [native code] }",
    "terminate_process": "function terminate_process() { [native code] }",
    "current_process_id": "function current_process_id() { [native code] }",
    "process_is_running": "function process_is_running() { [native code] }",
    "create_child_process": "function create_child_process() { [native code] }",
    "open_process": "function open_process() { [native code] }",
    "kill_process_by_name": "function kill_process_by_name() { [native code] }"
  },
  "npgsql": {
    "execute_io": "function execute_io() { [native code] }",
    "execute_scalar": "function execute_scalar() { [native code] }",
    "execute_non_query": "function execute_non_query() { [native code] }",
    "data_reader": "function data_reader() { [native code] }"
  },
  "native_pdf": {
    "generate_pdf": "function generate_pdf() { [native code] }"
  },
  "crypto": {
    "encrypt": "function encrypt() { [native code] }",
    "decrypt": "function decrypt() { [native code] }"
  },
  "create_http_request": "function create_http_request() { [native code] }",
  "require": "function require() { [native code] }",
  "__get_response_body": "function __get_response_body() { [native code] }",
  "hex_to_string": "function hex_to_string() { [native code] }",
  "string_to_hex": "function string_to_hex() { [native code] }",
  "__async_t": "function __async_t() { [native code] }",
  "__async": "function __async() { [native code] }",
  "setTimeout": "function setTimeout() { [native code] }",
  "__sleep": "function __sleep() { [native code] }"
}
```

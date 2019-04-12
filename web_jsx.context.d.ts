/*
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/

/**HTTP current request method*/
enum RequestMethod {
    /**HTTP Request method `GET`*/
    GET,
    /**HTTP Request method `HEAD`*/
    HEAD,
    /**HTTP Request method `POST`*/
    POST,
    /**HTTP Request method `PUT`*/
    PUT,
    /**HTTP Request method `DELETE`*/
    DELETE,
    /**HTTP Request method `CONNECT`*/
    CONNECT,
    /**HTTP Request method `OPTIONS`*/
    OPTIONS,
    /**HTTP Request method `TRACE`*/
    TRACE
}
//index.wjsx
//handler.wjsxh
interface ContextStatic {
    /**Executor path shown here*/
    app_path: string;
    /**Containing Server environment Path variable(s)*/
    env_path: string;
    /**Containing host name here without protocol*/
    host: string;
    /**Request remote address*/
    remote_addr: string;
    /**Application root directory*/
    root_dir: string;
    server_protocol: string
    /**Is secure connection request*/
    https: boolean;
    /**Host URL with protocol*/
    host_url: string;
    request: {
        /**HTTP request accept type*/
        accept: string;
        /**HTTP request accept encoding type*/
        accept_encoding: string;
         /**HTTP request content type*/
        content_type: string;
        /**Current HTTP Request all cookie collection with ; seperator*/
        cookie: string;
        /**HTTP Request Mehtod GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE*/
        method: RequestMethod;
        /**Current HTTP request page path*/
        page_path: string;
         /**Current HTTP request content*/
        pay_load: [];
        /**Current HTTP request query string*/
        query_string: Object;
         /**Current HTTP request user agent*/
        user_agent: string;
        /**Current HTTP request protocol*/
        protocol: string;
        /**
        *Read from current HTTP Payload stream chunk
        *~@param func A function with @param buff as chunk reading buffer and @param count chunk buffer length
        *~@throws if Http requset method not POST
        */
        read_payload( func: ( buff: string, count: number ) => any ): any;
    };
    response: {
        /**
        *Write string to HTTP Response body stream
        *~@param data A string containing chracter
        */
        write( data: string ): any;
        /**
        *Set HTTP Response header collection
        *~@param name A string containing header key e.g. Content-Type
        *~@param description A string containing header description e.g. application/json
        */
        header( name: string, description: string ): any;
        /**
        *Write HTTP Response body stream from server map file
        *~@param path A string containing file path
        *~@throws File not found exception
        */
        write_from_file( path: string ): any;
        /**HTTP response body*/
        body: {
            /**HTTP response flush, make sure this method execute end of response*/
            flush(): any;
            /**It'll write HTTP response body as pdf with default settings*/
            write_as_pdf(): any;
        };
        /**HTTP response body stram will be clear*/
        clear(): any;
        /**
         * Set HTTP response status code here and will be override privious given status code
         * ~@param http_status should be HTTP status code. we will not verify given status code
         * ~@param description HTTP status description
         */
        status( http_status: string, description: string ): any;
         /**
         * Set HTTP cookie collection and if match, than it will be override privious given cookie
         * ~@param cook should be like this `Set-Cookie:name=value;`
         * ~@param description HTTP status description
         */
        cookie( cook: string ): any;
         /**
         * Set HTTP status code 303 See Other
         *~@param url server url
         *~@param force if you set true than we'll set your url with http protocol
         */
        redirect( url: string, force: boolean ): any;
        /**HTTP response apply as zlib GZip stream compression.*/
        as_gzip(): any;
    };
}

interface IJsxFile {
    /**
    * File constructor.
    * ~@param path server file path
    * ~@param mood define read/write mood e.g. r|r+|w|w+|a|a+
    * @throws Director Not Found|File Not Found|Permission denied
    * ~Read more https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s
    */
    new( path: string, mood: string ): IJsxFile;
    /**
    * Read file byte chunk if you define file mood read when create constructor
    * ~@param count define the read length
    * ~@returns Object {read character, character length}
    */
    read( count: number ): {
        /**Read character*/
        data: string;
        /**Read character length*/
        read_length: number
    };
    /**
    * Write chracter in File Stream which opened yet as read mood
    * ~@param data define write character
    * ~@throws The stream is not readable
    */
    write( data: string ): any;
    /**
    * Release File Stream which opened yet as read or write mood
    * ~If you open is write mood, than all buffer will be write to disk.
    */
    flush(): any;
}
interface IFsStatic {
    /**
    * Read server file
    *~@param path server file path
    *~@throws Director Not Found|File Not Found|Permission denied
    * ~@returns {staus_code, message, data}
    */
    read_file( path: string ): {
        /**Execution status code*/
        staus_code: number;
        /**Execution status message*/
        message: string;
        /**File character*/
        data: string;
    };
    /**
    * Write file to server
    *~@param path server file path
    *~@param data define write character
    *~@throws Director Not Found|Permission denied
    * ~@returns {staus_code, message}
    */
    write_file( path: string, data: string ): {
        /**Execution status code*/
        staus_code: number;
        /**Execution status message*/
        message: string;
    };
    /**
    * Check file exists in server
    *~@param path server file path
    * ~@returns true|false
    */
    exists_file( path: string ): boolean;
    /**
     * Create new file constructor.
     * e.g. var file = new fs.file(your_path, your_open_format);
     */
    file: IJsxFile;
    /**
    *Create a file from HTTP post data collection.
    * if http request method POST nor @throws Allowed only over POST data.
    *~@param path A string containing server path
    *~@throws Stram Write not readable|Directory not found exception|Permission denied
    */
    write_file_from_payload( path: string ): any;
}
interface ISpwanOption {
    /**Process start directory default current location; not required*/
    start_in: string;
    /**Process name default null; not required*/
    process_name: string;
    /**
     * Process full location required
     * ~e.g. C:/web_jsx/web_jsx_cgi.exe
    */
    process_path: string;
    /**Process title not required*/
    title: string;
    /**Process argument not required*/
    arg: string;
    /**If you need to wait untill Process exit, than set true default false*/
    wait_for_exit: boolean;
}
interface ISysStatic {
    /**
    * Read server directory
    *~@param dir server dir e.g. / root dir
    *~@throws Director Not Found | Permission denied
    * ~@returns {staus_code, message, dir}
    */
    read_directory( dir: string ): {
        /**Execution status code*/
        staus_code: number;
        /**Execution status message*/
        message: string;
        /**Directory collection */
        dir: [];
    };
    /**
    * Read server directory with filter
    *~@param dir server dir e.g. / root dir
    *~@param ext server dir e.g. (html|aspx|jsx|php) or C Regular Expression
    *~@throws Director Not Found|Permission denied
    * ~@returns {staus_code, message, dir}
    */
    read_directory_regx( dir: string, ext: string ): {
        /**Execution status code*/
        staus_code: number;
        /**Execution status message*/
        message: string;
        /**Directory collection */
        dir: [];
    };
    /**
    * Create directory into server
    *~@param path server dir e.g. /log/
    *~@throws Permission denied
    * ~@returns {message}
    */
    create_directory( path: string ): string;
    /**
    * Delete directory from server
    *~@param path server dir e.g. /log/
    *~@throws Director Not Found|Permission denied
    * ~@returns {message}
    */
    delete_directory( path: string ): string;
    /**
     * Spwan new process|child process
     * ~@param option Containing {start_in:string, process_name:string, process_path:string, title:string, arg:string, wait_for_exit:boolean}
     * ~param start_in -> Process start directory default current location; not required
     * ~param process_name -> Process name default null; not required
     * ~param process_path -> Process full location required e.g. C:/web_jsx/web_jsx_cgi.exe
     * ~param title -> Process title not required
     * ~param arg -> Process argument not required
     * ~param wait_for_exit -> If you need to wait untill Process exit, than set true default false
     */
    create_process( option: ISpwanOption ): any;
    /**
    * Terminate open process by process id
    * ~@param pid process id
    * ~@throws Permission denied
    * ~@returns {if process found 1 or 0}
    */
    terminate_process( pid: number ): number;
    /**
    * Current process id
    * ~@returns {Current Processs Id}
    */
    current_process_id(): number;
    /**
    * Check given process id is running
    * ~@param pid define Process Id
    * ~@returns {true|false}
    */
    process_is_running( pid: number ): boolean;
    /**
    * Create new child process
    * ~@param process_path process full path
    * ~@param arg process argument
    * ~@throws Process not found
    * ~@returns {1}
    */
    create_child_process( process_path: string, arg: string ): number;
    /**
    * Open new process and forget
    * ~@param process_path process full path
    * ~@param arg process argument
    * ~@throws Process not found
    * ~@returns {process_id}
    */
    open_process( process_path: string, arg: string ): number;
    /**
    * Kill any open process by name e.g. web_jsx_cgi.exe
    * ~@param process_name e.g. web_jsx_cgi.exe
    * ~@throws Permission denied
    * ~@returns {-1|0}
    */
    kill_process_by_name( process_name: string ): number;
}
interface INpgSqlStatic {
    /**
     * Executes the stored procedure @param sp_str and returns the first row as json string
     *~Opens a database connection with the connection settings specified by the @param con_str.
     *~@param con_str define the npgsql connection string
     *~@param sp_str define the specific stored procedure name
     *~@param ctx_str define the json string as stored procedure first param
     *~@param form_data_str define the json string as stored procedure second param
     *~@returns The first column of the first row as json string. e.g. {_ret_val number; _ret_msg string; _ret_data_table json string}
     */
    execute_io( con_str: string, sp_str: string, ctx_str: JSON, form_data_str: JSON ): {
        /**
        * define the excution status (In-case error occured in middle or back then return -1)
        */
        _ret_val: number;
        /**
         * define the excution status message
        */
        _ret_msg: string;
        /**
         * In-case any json data return from database
        */
        _ret_data_table: string;
    };
    /**
     * NotImplementedException
     * ~@returns Error
     */
    execute_scalar(): Error;
    /**
     * NotImplementedException
     * ~@returns Error
     */
    execute_non_query(): Error;
    /**
     * Executes the query @param query_str and give each row in @param func
     *~Opens a database connection with the connection settings specified by the @param con_str.
     *~@param con_str define the npgsql connection string
     *~@param query_str define the PgSQL query string
     *~@param func define the Function callback with param @param index is the number row and @param row is the data row array [columns]
     */
    data_reader( con_str: string, query_str: string, func: ( index: number, row: string ) => any ): any;
}
interface IPdfOption {
    /**Pdf file output path*/
    path: string;
    /**Create pdf from http url*/
    url: string;
    /**If create pdf from given chracter set true*/
    from_body: boolean;
    /**Containing page settings*/
    global_settings: Object;
    /**Containing page design settings*/
    object_settings: Object;
}
interface INativePdfStatic {
    /**
     * Generate pdf with wkhtmltopdf Api
     * ~@param option Containing {path: string, url: string, from_body: boolean, global_settings: Object, object_settings: Object}
     * ~@param body if you like to create pdf from given chracter. Can be null | void 0
     * ~option=>
     * ~path->Pdf file output path
     * ~url->Create pdf from http url
     * ~from_body->If create pdf from given chracter set true
     * ~global_settings->Containing page settings
     * ~object_settings->Containing page design settings
     * ~@throws Director Not Found|Permission denied
     * ~@returns {ret_val:number, ret_msg:string}
     */
    generate_pdf( option: IPdfOption, body: string ): {
        /**define the excution status (In-case error occured in middle or back then return -1) */
        ret_val: number;
        /**define the excution status message*/
        ret_msg: string;
    };
}
interface ICryptoStatic {
    /**
    * Encrypt data with openssl EVP algorithm
    * ~Initialise the encryption operation. IMPORTANT - ensure you use a key
    * ~and IV size appropriate for your cipher
    * ~In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * ~IV size for *most* modes is the same as the block size. For AES this
    * ~is 128 bits
    * ~@param data define plain string
    * ~@param data define plain string
    * ~@param key define Decrypt key
    * ~@param iv define Decrypt iv
    * ~@returns encrypted string
    */
    encrypt( data: string, key: string, iv: string ): string;
    /**
    * Decrypt data with openssl EVP algorithm
    * ~Initialise the decryption operation. IMPORTANT - ensure you use a key
    * ~and IV size appropriate for your cipher
    * ~In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * ~IV size for *most* modes is the same as the block size. For AES this
    * ~is 128 bits
    * ~@param data define plain string
    * ~@param key define Decrypt key
    * ~@param iv define Decrypt iv
    * ~@returns decypted string
    */
    decrypt( data: string, key: string, iv: string ): string;
}
interface IHttpOption {
    /**HTTP request full uri */
    url: string;
    /**HTTP request method GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE*/
    method: string;
    /**Follow server redirect location default true*/
    follow_location: boolean;
    /**Request header collection*/
    header: [];
    /**Request cookie collection*/
    cookie: string;
}
/**
 * Create simple HTTP request with curl
 * ~@param option Containing {url:string, method:string, follow_location:boolean, header:[], cookie:string}
 * ~url -> HTTP request full uri
 * ~method -> HTTP request method GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE
 * ~follow_location -> Follow server redirect location default true
 * ~header -> Request header collection
 * ~cookie -> Request cookie collection
 * @returns Object 
 */
declare function create_http_request( option: IHttpOption ): {
    /**define the excution status (In-case error occured in middle or back then return -1) */
    ret_val: number;
    /**define the excution status message*/
    ret_msg: string;
    /**HTTP response header with ; seperator*/
    response_header: string;
    /**HTTP response body chracter*/
    response_body: string;
};
/**
 * Load your Javascript module e.g. require("/addon/http.js");
 * ~@param path define the Javascript module location
 * ~@throws Module not found | unable to compile module
 * ~@returns Object or what you've set into module.exports in your module.js
 */
declare function require( path: string ): Object;
/**
 * Convert Hex string to plain string
 * @param data hex string
 */
declare function hex_to_string( data: string ): string;
/**
 * Convert plain string to Hex string
 * @param data plain string
 */
declare function string_to_hex( data: string ): string;
/**
 * Create a async thread with uvlib event_loop algorithm (Non-blocking thread)
 * ~@param func Execute non-blocking thread here
 */
declare function __async_t( func: Function ): any;
/**
 * Create async thread by std::async method and execute with another thread (Blocking thread)
 * ~@param func here execute 
 */
declare function __async( func: Function ): any;
/**
 * Create a set time out timer with uvlib event_loop algorithm (Non-blocking thread)
 * ~@param func Execute non-blocking thread here
 * ~@param milliseconds delay time
 */
declare function setTimeout( func: Function, milliseconds: number ): any;
/**
 * Block current thread untill timeout
 * ~@param milliseconds sleep time
 */
declare function __sleep( milliseconds: number ): any;

declare var context: ContextStatic;
declare var fs: IFsStatic;
declare var sys: ISysStatic;
declare var npgsql: INpgSqlStatic;
declare var native_pdf: INativePdfStatic;
declare var crypto: ICryptoStatic;

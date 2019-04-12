/*
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/

/**HTTP current request method*/
declare enum RequestMethod {
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
    /**
    * Executor path shown here
    * @type {string}
    */
    app_path: string;
    /**
    * Containing Server environment Path variable(s)
    * @type {string}
    */
    env_path: string;
    /**
    * Containing host name here without protocol
    * @type {string}
    */
    host: string;
    /**
    * Request remote address
    * @type {string}
    */
    remote_addr: string;
    /**
    * Application root directory
    * @type {string}
    */
    root_dir: string;
    server_protocol: string
    /**
    * Is secure connection request
    * @type {boolean}
    */
    https: boolean;
    /**
    * Host URL with protocol
    * @type {string}
    */
    host_url: string;
    request: {
        /**
        * HTTP request accept type
        * @type {string}
        */
        accept: string;
        /**
        * HTTP request accept encoding type
        * @type {string}
        */
        accept_encoding: string;
        /**
        * HTTP request content type
        * @type {string}
        */
        content_type: string;
        /**
        * Current HTTP Request all cookie collection with ; seperator
        * @type {string}
        */
        cookie: string;
        /**
        * HTTP Request Mehtod GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE
        * @type {RequestMethod}
        */
        method: RequestMethod;
        /**
        * Current HTTP request page path
        * @type {string}
        */
        page_path: string;
        /**
        * Current HTTP request content
        * @type {Array}
        */
        pay_load: [];
        /**
        * Current HTTP request query string
        * @type {Object}
        */
        query_string: Object;
        /**
        * Current HTTP request user agent
        * @type {string}
        */
        user_agent: string;
        /**
        * Current HTTP request protocol
        * @type {string}
        */
        protocol: string;
        /**
        * Read from current HTTP Payload stream chunk
        * ~@param func A function with @param buff as chunk reading buffer and @param count chunk buffer length
        * ~@throws if Http requset method not POST
        */
        read_payload( func: ( buff: string, count: number ) => any ): any;
    };
    response: {
        /**
        * Write string to HTTP Response body stream
        * ~@param data A string containing chracter
        */
        write( data: string ): any;
        /**
        * Set HTTP Response header collection
        * ~@param name A string containing header key e.g. Content-Type
        * ~@param description A string containing header description e.g. application/json
        */
        header( name: string, description: string ): any;
        /**
        * Write HTTP Response body stream from server map file
        * ~@param path A string containing file path
        * ~@throws File not found exception
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
         * ~@param url server url
         * ~@param force if you set true than we'll set your url with http protocol
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
    * ~@throws Director Not Found|File Not Found|Permission denied
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
declare var context: ContextStatic;
//npm install -g yo generator-code
//npm install -g yo
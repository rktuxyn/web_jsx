/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
enum RequestMethod {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
}

interface ContextStatic {
    /**
    *Executor path shown here
    */
    app_path: string;
    /**
    *Containing Server environment Path variable(s)
    */
    env_path: string;
    /**
    *Containing host name here without protocol
    */
    host: string;
    /**
    *Request remote address
    */
    remote_addr: string;
    /**
    *Application root directory
    */
    root_dir: string;
    server_protocol: string
    https: boolean;
    host_url: string;
    request: {
        accept: string;
        accept_encoding: string;
        content_type: string;
        cookie: string;
        /**
        * Http Request Mehtod GET|HEAD|POST|PUT|DELETE|CONNECT|OPTIONS|TRACE
        */
        method: RequestMethod;
        page_path: string;
        pay_load: [];
        query_string: Object;
        user_agent: string;
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
        body: {
            flush(): any;
            write_as_pdf(): any;
        };
        clear(): any;
        status( http_status: string, description: string ): any;
        cookie( cook: string ): any;
        redirect( url: string, force: boolean ): any;
        /**
        *HTTP response apply as zlib GZip stream compression.
        */
        as_gzip(): any;
    };
}
interface IJsxFile {
    new( path: string, mood: string ): IJsxFile;
    read(): string;
    write( data: string ): any;
}
interface IFsStatic {
    read_file( path: string ): {
        staus_code: number;
        message: string;
        data: string;
    };
    write_file( path: string, data: string ): {
        staus_code: number;
        message: string;
    };
    exists_file( path: string ): boolean;
    file: IJsxFile;
    /**
    *Create a file from HTTP post data collection.
    * if http request method POST nor @throws Allowed only over POST data.
    *~@param path A string containing server path
    *~@throws Stram Write or Directory not found exception
    */
    write_file_from_payload( path: string ): any;
}
interface ISpwanOption {
    start_in: string;
    process_name: string;
    process_path: string;
    title: string;
    arg: string;
    wait_for_exit: boolean;
}
interface ISysStatic {
    read_directory( dir: string ): {
        staus_code: number;
        message: string;
        dir: [];
    };
    read_directory_regx( dir: string, ext: string ): {
        staus_code: number;
        message: string;
        dir: [];
    };
    create_directory( path: string ): string;
    delete_directory( path: string ): string;
    create_process( option: ISpwanOption ): any;
    terminate_process( pid: number ): any;
    current_process_id(): number;
    process_is_running( pid: number): boolean;
    create_child_process( process_path: string, arg: string ): number;
    open_process( process_path: string, arg: string ): number;
    kill_process_by_name( process_name: string ): number;
}
interface INpgSqlStatic {
    /**
     * Executes the query and returns the first column of the first row in the result
     *~set returned by the query. All other columns and rows are ignored.
     *~Opens a database connection with the property settings specified by the con_str.
     *~@param con_str define the npgsql connection string
     *~@param sp_str define the specific stored procedure name
     *~@param ctx_str define the json string as stored procedure first param
     *~@param form_data_str define the json string as stored procedure second param
     *~@returns The first column of the first row as json string. e.g. {_ret_val number; _ret_msg string; _ret_data_table json string}
     */
    execute_io( con_str: string, sp_str: string, ctx_str: JSON, form_data_str: JSON ): {
        _ret_val: number;
        _ret_msg: string;
        _ret_data_table: string;
    };
    //NotImplementedException
    execute_scalar(): Error;
    //NotImplementedException
    execute_non_query(): Error;
    data_reader( con_str: string, query_str: string, func: ( index: number, row: string ) => any ): any;
}
interface INativePdfStatic {
    generate_pdf(): any;
}
interface ICryptoStatic {
    encrypt( data: string ): string;
    decrypt( data: string ): string;
}
interface IHttpOption {
    url: string;
    method: string;
    follow_location: boolean;
    header: [];
    cookie: string;
}
declare function create_http_request( option: IHttpOption ): {
    ret_val: number;
    ret_msg: string;
    response_header: string;
    response_body: string;
};
declare function require( path: string ): Object;
declare function hex_to_string( data: string ): string;
declare function string_to_hex( data: string ): string;
declare function __async_t( func: Function): any;
declare function __async( func: Function ): any;
declare function setTimeout( func: Function, milliseconds: number ): any;
declare function __sleep( milliseconds: number ): any;
declare var context: ContextStatic;
declare var fs: IFsStatic;
declare var sys: ISysStatic;
declare var npgsql: INpgSqlStatic;
declare var native_pdf: INativePdfStatic;
declare var crypto: ICryptoStatic;

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:13 PM 2/5/2020
let ports = [
    { port: "9100", pid: void 0 },
    { port: "9101", pid: void 0 },
    { port: "9102", pid: void 0 },
    { port: "9103", pid: void 0 }
];
//web_jsx fcgi "localhost:9100"
ports.forEach( ( row ) => {
    print( `Argument pass=>fcgi localhost:${row.port}\n` );
    row.pid = sys.create_process( {
        start_in: env.root_dir,
        process_name: env.app_path,
        process_path: env.app_path,
        title: "web_jsx",
        arg: `fcgi "localhost:${row.port}"`,
        wait_for_exit: false
    } );
} );
print( `${JSON.stringify( ports )}\n` );
print( "Opening nginx...\n" );
let pid = sys.create_process( {
    start_in: env.root_dir,
    process_name: "nginx",
    process_path: "C:\\nginx\\nginx.exe",
    title: "nginx",
    arg: "",
    wait_for_exit: false
} );
print( `nginx pid=>${pid}\n` );
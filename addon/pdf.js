/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:40 AM 12/5/2018
/** [Object Extend]*/
( typeof ( Object.extend ) === 'function' ? undefined : ( Object.extend = function ( destination, source ) {
    for ( let property in source )
        destination[property] = source[property];
    return destination;
} ) );
/** [/Object Extend]*/
/** [Object clone]*/
( typeof ( Object.clone ) === 'function' ? undefined : ( Object.clone = function ( object ) {
    return this.extend( {}, object );
} ) );
/** [/Object clone]*/
var cfg = {
    path: undefined,
    url: undefined,
    from_body: false,
    global_settings: {
        "documentTitle": "Hello World",
        "size.paperSize": "A4",
        "orientation": "Portrait",
        "colorMode": "Color",
        "dpi": "80",
        "imageDPI": "300",
        "imageQuality": "92",
        "margin.top": "1.27cm",
        "margin.bottom": "1.27cm",
        "margin.left": "1.27cm",
        "margin.right": "1.27cm"
    },
    object_settings: {
        "web.defaultEncoding": "utf-8",
        "web.background": "true",
        "web.loadImages": "true",
        "web.enablePlugins": "false",
        "web.enableJavascript": "false",
        "web.enableIntelligentShrinking": "true",
        "web.minimumFontSize": "12",
        "web.printMediaType": "true",
        "header.fontSize": "8",
        "header.fontName": "Times New Roman",
        "header.left": "[date]",
        "header.line": "false",
        "header.spacing": "0",
        "footer.fontSize": "8",
        "footer.right": "Page [page] of [topage]",
        "footer.line": "false",
        "footer.spacing": "0"
    }
};
function pdf( native_pdf ) {
	//wkhtmltopdf
    this.native_pdf = native_pdf;
};
function prepare_settings( config) {
    let conf = Object.clone( cfg );
    Object.extend( conf, config );
    if ( !conf.path || conf.path === undefined )
        throw new Error("Output path is required!!!");
    if ( 'object' !== typeof ( conf.global_settings ) )
        throw new Error("global_settings required!!!");
    if ( 'object' !== typeof ( conf.object_settings ) )
        throw new Error("global_settings required!!!");
    for ( let p in conf.global_settings ) {
		//Object.hasOwnProperty
        if ( cfg.global_settings[p] === undefined )
            throw new Error(`Invalid key==>${p} defined in global_settings`);
        if ( "string" !== typeof ( conf.global_settings[p] ) )
            conf.global_settings[p] = String( conf.global_settings[p] );
    }
    for ( let p in conf.object_settings ) {
		//Object.hasOwnProperty
        if ( cfg.object_settings[p] === undefined )
            throw new Error(`Invalid key==>${p} defined in object_settings`);
        if ( "string" !== typeof ( conf.object_settings[p] ) )
            conf.object_settings[p] = String( conf.object_settings[p] );
    }
    return conf;
};
Object.extend( pdf.prototype, {
    native_pdf: undefined,
    generate_async: function ( config, body) {
        if ( typeof ( this.native_pdf ) !== 'object' )
            throw new Error("Native pdf required!!!");
        let cnf = prepare_settings( config );
        delete config;
        if ( body && null !== body ) {
            cnf.from_body = true;
            cnf.url = undefined;
        }
        let resp = {};
		//Open new asynchronous thread ( std::async )
        let thread_id = __async( () => {
            resp = this.native_pdf.generate_pdf( config, body );
        } );
        return resp;
    },
    generate: function ( config, body ) {
        if ( 'object' !== typeof ( this.native_pdf )  )
            throw new Error("Native pdf required!!!");
		if ( 'object' !== typeof ( config ) )
            throw new Error("config required!!!");
        let cnf = prepare_settings( config );
        delete config;
        if ( body && null !== body ) {
            cnf.from_body = true;
            cnf.url = undefined;
        }
        return this.native_pdf.generate_pdf( config, body );
    }
} );
module.exports = {
    pdf: pdf,
    config: cfg
};
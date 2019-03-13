/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:33 PM 12/17/2018
module.exports = {
    help: {
        html: {
            normalize: function ( body ) {
                return body
                    .replace( /\r\n/gi, "" ).replace( /\n/gi, "" ).replace( /\s+/g, " " )
                    .replace( /<link rel[^>]*\/>/gi, "" )//Replace style tag
                    .replace( /<link href[^>]*\/>/gi, "" )//Replace style tag
                    .replace( /<link href[^>]*>/gi, "" )//Replace style tag
                    .replace( /<a[^>]*>/gi, "" )
                    .replace( /<script[^>]*>([\s\S]+?)<\/script>/gi, "" )//Replace script tag
                    .replace( /<style[^>]*>([\s\S]+?)<\/style>/gi, "" )
                    .replace( /"\ >/gi, '">' )
                    .replace( /\/\*[\s\S]*?\*\/|([^\\:]|^)\/\/.*$/gm, "" )//Replace comment
                    .replace( /<!--([\s\S]+?)-->/gi, "" )//replace <!-- --> tag
                    ;
            }
        }
    },
};
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:38 PM 1/7/2019
class Multi {
    // Inherit method to create base classes.
    static inherit( ..._bases ) {
        class classes {
            // The base classes
            get base() { return _bases; }
            constructor( ..._args ) {
                var index = 0;
                for ( let b of this.base ) {
                    let obj = new b( _args[index++] );
                    Multi.copy( this, obj );
                }
            }
        }
        // Copy over properties and methods
        for ( let base of _bases ) {
            Multi.copy( classes, base );
            Multi.copy( classes.prototype, base.prototype );
        }
        return classes;
    }
    // Copies the properties from one class to another
    static copy( _target, _source ) {
        for ( let key of Reflect.ownKeys( _source ) ) {
            if ( key !== "constructor" && key !== "prototype" && key !== "name" ) {
                let desc = Object.getOwnPropertyDescriptor( _source, key );
                Object.defineProperty( _target, key, desc );
            }
        }
    }
}
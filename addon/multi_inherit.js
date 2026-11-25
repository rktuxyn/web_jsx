/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
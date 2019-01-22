
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
( typeof ( Array.prototype._reverse ) === "function" ? undefined : ( Array.prototype._reverse = function ( c ) {
    if ( null == this ) throw new TypeError( '"this" is null or not defined' );
    let arr = Object( this ),
        i = arr.length >>> 0;
    if ( 0 === i ) return [];
    let out = [];
    if ( typeof ( c ) == 'function' ) {
        while ( i-- ) {
            out.push( c.call( this, arr[i] ) );
        }
        return out;
    }
    while ( i-- ) {
        out.push( arr[i] );
    }
    return out;
} ) );
( function ( exports ) {
    function Explore() { };
    ( function ( member ) {
        for ( let p in member )
            this[p] = member[p];
    }.call( Explore.prototype, {
        process: {
            wordCollect: [], numArray: [], C: 0,
        },
        word: {
            oneTo10: ['Zero', 'One', 'Two', 'Three', 'Four', 'Five', 'Six', 'Seven', 'Eight', 'Nine'],
            tenTo19: ['Ten', 'Eleven', 'Twelve', 'Thirteen', 'Fourteen', 'Fifteen', 'Sixteen', 'Seventeen', 'Eighteen', 'Nineteen'],
            tenPlus: ['unknown', 'Ten', 'Twenty', 'Thirty', 'Forty', 'Fifty', 'Sixty', 'Seventy', 'Eighty', 'Ninety'],
        },
        _tenPlus: function _tenPlus( i ) {
            return this.process.numArray[i] === 0 ? ( this.process.wordCollect[this.process.C] = '' ) : this.process.numArray[i] === 1
                ? ( this.process.wordCollect[this.process.C] = this.word.tenTo19[this.process.numArray[i - 1]] )
                : ( this.process.wordCollect[this.process.C] = this.word.tenPlus[this.process.numArray[i]] );
        },
        contraption: ( function () {
            var giganticNumeral = {
                bn: function bn( len ) {
                    var fnc, i, C;
                    !len instanceof Number ? ( len = 10 ) : '';
                    for ( i = 0, C = 0, fnc = {}; i < len; i++ ) {
                        C === 1 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                            this._tenPlus( x ); this.process.C++; return;
                        }, C++ ) :

                            C === 2 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                this.hundred( x, _number_ ); this.process.C++; return;
                            }, C++ ) :

                                C === 3 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                    this.thousandOrLakh( x, _number_, 'Thousand' ); this.process.C++; return;
                                }, C++ ) :

                                    C === 4 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                        this._tenPlus( x ); this.process.C++; return;
                                    }, C++ ) :

                                        C === 5 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                            this.thousandOrLakh( x, _number_, 'Lakh' ); this.process.C++; return;
                                        }, C++ ) :

                                            C === 6 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                this._tenPlus( x ); this.process.C++; return;
                                            }, C++ ) :

                                                C === 7 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                    this.croreOrSingle( x, _number_, 'Crore' ); this.process.C++;
                                                }, i >= 7 ? ( C = 1 ) : ( C = 0 ) ) :
                                                    /** Use only 1st time **/
                                                    C === 0 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                        this.croreOrSingle( x, _number_, ( x !== 0 ? '' : ( isDecimal ? 'Paisa Only' : 'Taka' ) ) );
                                                        this.process.C++; return;
                                                    }, C++ ) : 'No Way';
                    }
                    return fnc;
                },
                us: function us( len ) {
                    var fnc, i, C;
                    !len instanceof Number ? ( len = 13 ) : '';
                    for ( i = 0, C = 0, fnc = {}; i < len; i++ ) {
                        C === 1 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                            this._tenPlus( x ); this.process.C++; return;
                        }, C++ ) :

                            C === 2 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                this.hundred( x, _number_ ); this.process.C++; return;
                            }, C++ ) :

                                C === 3 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                    this.thousandOrLakh( x, _number_, 'Thousand' ); this.process.C++; return;
                                }, C++ ) :

                                    C === 4 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                        this._tenPlus( x ); this.process.C++; return;
                                    }, C++ ) :

                                        C === 5 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                            this.thousandOrLakh( x, _number_, 'Million' ); this.process.C++; return;
                                        }, C++ ) :

                                            C === 6 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                this._tenPlus( x ); this.process.C++; return;
                                            }, C++ ) :

                                                C === 7 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                    this.croreOrSingle( x, _number_, 'Billion' ); this.process.C++;
                                                }, C++ ) :

                                                    C === 8 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                        this._tenPlus( x ); this.process.C++; return;
                                                    }, C++ ) :

                                                        C === 9 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                            this.croreOrSingle( x, _number_, 'Trillion' ); this.process.C++;
                                                        }, i >= 9 ? ( C = 1 ) : ( C = 0 ) ) :
                                                            /** Use only 1st time **/
                                                            C === 0 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                                this.croreOrSingle( x, _number_, ( x !== 0 ? '' : ( isDecimal ? 'Paisa Only' : 'Taka' ) ) );
                                                                this.process.C++; return;
                                                            }, C++ ) : 'No Way';
                    }
                    return fnc;
                },
                uk: function uk( len ) {
                    var fnc, i, C;
                    !len instanceof Number ? ( len = 13 ) : '';
                    for ( i = 0, C = 0, fnc = {}; i < len; i++ ) {
                        C === 1 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                            this._tenPlus( x ); this.process.C++; return;
                        }, C++ ) :
                            C === 2 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                this.hundred( x, _number_ ); this.process.C++; return;
                            }, C++ ) :
                                C === 3 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                    this.thousandOrLakh( x, _number_, 'Thousand' ); this.process.C++; return;
                                }, C++ ) :
                                    C === 4 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                        this._tenPlus( x ); this.process.C++; return;
                                    }, C++ ) :
                                        C === 5 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                            this.thousandOrLakh( x, _number_, 'Million' ); this.process.C++; return;
                                        }, C++ ) :
                                            C === 6 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                this._tenPlus( x ); this.process.C++; return;
                                            }, C++ ) :
                                                C === 7 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                    this.croreOrSingle( x, _number_, 'Billion' ); this.process.C++;
                                                }, C++ ) :
                                                    C === 8 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                        this._tenPlus( x ); this.process.C++; return;
                                                    }, C++ ) :
                                                        C === 9 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                            this.croreOrSingle( x, _number_, 'Trillion' ); this.process.C++;
                                                        }, i >= 9 ? ( C = 1 ) : ( C = 0 ) ) :
                                                            /** Use only 1st time **/
                                                            C === 0 ? ( fnc[i] = function ( x, _number_, isDecimal ) {
                                                                this.croreOrSingle( x, _number_, ( x !== 0 ? '' : ( isDecimal ? 'Paisa Only' : 'Taka' ) ) );
                                                                this.process.C++; return;
                                                            }, C++ ) : 'No Way';
                    }
                    return fnc;
                }
            }
            return { bn: giganticNumeral.bn( 8 ), us: giganticNumeral.us( 10 ), uk: giganticNumeral.uk( 10 ) };
        }() ),
        hundred: function hundred( i, _number_ ) {
            return _number_ === 0 ?
                ( this.process.wordCollect[this.process.C] = '' )
                : ( this.process.numArray[i - 1] !== 0 && this.process.numArray[i - 2] !== 0 )
                    ? ( this.process.wordCollect[this.process.C] = this.word.oneTo10[_number_] + ' Hundred and' )
                    : ( this.process.wordCollect[this.process.C] = this.word.oneTo10[_number_] + ' Hundred' );
        },
        thousandOrLakh: function thousandOrLakh( i, _number_, msg ) {
            ( _number_ === 0 || this.process.numArray[i + 1] === 1 ) ?
                ( this.process.wordCollect[this.process.C] = '' ) : ( this.process.wordCollect[this.process.C] = this.word.oneTo10[_number_] );
            return ( this.process.numArray[i + 1] !== 0 || _number_ > 0 )
                ? this.process.wordCollect[this.process.C] = this.process.wordCollect[this.process.C] + ' ' + msg : '';
        },
        croreOrSingle: function croreOrSingle( i, _number_, msg ) {
            ( _number_ === 0 || this.process.numArray[i + 1] === 1 ) ? ( this.process.wordCollect[this.process.C] = '' ) : ( this.process.wordCollect[this.process.C] = this.word.oneTo10[_number_] );
            this.process.wordCollect[this.process.C] = this.process.wordCollect[this.process.C] + ' ' + msg;
            return;
        },
        format: function format( number, isDecimal, type ) {
            var i, ilen, isNumber, mNumber, fn;
            this.process.wordCollect = []; this.process.C = 0;

            if ( !number instanceof "".constructor ) {
                this.dispose();
                return false;
            }
            this.process.numArray = number.split( "" ); number = undefined;
            if ( !this.process.numArray instanceof [].constructor ) {
                this.dispose();
                return false;
            }
            isNumber = true;

            this.process.numArray = this.process.numArray._reverse( function ( c ) {
                return !isNaN( mNumber = parseInt( c ) ) ? mNumber : ( isNumber = false, c + '- is invalid `Number`!' );
            } );

            if ( !isNumber ) { console.warn( this.process.numArray ); this.dispose(); return false; }
            mNumber = '', isNumber = '';
            ilen = this.process.numArray.length;
            for ( i = 0, fn = 0; i < ilen; i++ ) {
                type === 'bn' ? ( ( fn > 7 ) ? ( fn = 1 ) : undefined/** Nothing to  do...*/ )
                    : type === 'us' ? ( ( fn > 9 ) ? ( fn = 1 ) : undefined/** Nothing to  do...*/ )
                        : type === 'uk' ? ( ( fn > 9 ) ? ( fn = 1 ) : undefined/** Nothing to  do...*/ )
                            : undefined/** Undefined Constituency*/;
                this.contraption[type] && typeof this.contraption[type][fn] === 'function' ? this.contraption[type][fn].call( this, i, this.process.numArray[i], isDecimal ) : ( console.warn( 'Invalid function defined. Function name - ' + type + ' and contraption function number -' + fn + '.' ) );
                fn++;
            }
            return this.serilize();
        },
        serilize: function serilize() {
            var i, ilen, out;
            if ( !this.process.wordCollect instanceof [].constructor ) {
                this.dispose();
                return false;
            }
            this.process.wordCollect = this.process.wordCollect._reverse();/** Reverse*/
            for ( i = 0, ilen = this.process.wordCollect.length, out = ''; i < ilen; i++ ) {
                this.process.wordCollect[i] && i >= 0 && i !== ( ilen - 1 ) ? out += this.process.wordCollect[i] + ' ' : out += this.process.wordCollect[i];
            }
            this.dispose();
            return out;
        },
        dispose: function () {
            this.process.wordCollect = [], this.process.C = 0, this.process.numArray = [];
            return this;
        }
    } ) );
    /** Export*/
    exports.inWord = function inWord( number, type ) {
        let out, paisa;
        if ( !( !type ? ( ( type = 'bn' ), true ) : type instanceof Object ? false : ( type = type.toLowerCase() ), type === 'bn' ? true : type === 'us' ? true : type === 'uk' ? true : false ) ) {
            return 'Undefined Constituency found...!  ' + type;
        }
        if ( !number || typeof number === 'object' || number === null ) { return 'Invalid Amount'; }
        number = number.toString(); out = '';
        if ( number.length > 100000 ) {
            return 'Gigantic number...';
        }
        number.indexOf( '-' ) > -1 ? ( out = '(-)', number = number.replace( /-/, '' ) ) : '';
        number = number.split( '.' ); paisa = number[1]; number = number[0]; number = number.replace( /,/g, '' );
        number ? ( number = new Explore().format( number, false/**isDecimal */, type ), number ? ( out += number, number = true ) : number = false ) : ( number = false );
        if ( paisa <= 0 ) {
            out += ' Only';
            return out ? out.trim() : 'Invalid Amount...!';
        }
        paisa ? ( paisa = new Explore().format( paisa, true/**isDecimal */, type ), number && paisa ? ( out += ' And ' + paisa ) : ( number ? ( out += ' Only' ) : ( paisa ? out = paisa : out = false ) ) ) : ( paisa = false );
        return out ? out.trim() : 'Invalid Amount...!';
    };
}( typeof ( module ) === "object" ? module.exports : this ) );

//let currency = require( "/addon/currency_in_word.js" );
//currency.inWord("5845454.50")
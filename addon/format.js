( function ( exports ) {
    var defaultFormat = '0,0', currentLanguage = 'en', languages = {}, zeroFormat = null;
    languages['en'] = {
        delimiters: {
            thousands: ',',
            decimal: '.'
        },
        abbreviations: {
            thousand: 'tis.',
            million: 'mil.',
            billion: 'b',
            trillion: 't'
        },
        ordinal: function () {
            return '.';
        },
        currency: {
            symbol: '$'
        }
    };
    // Node
    //var task, _value;
    function toFixed( value, precision, roundingFunction, optionals ) {
        var power = Math.pow( 10, precision ),
            optionalsRegExp,
            output;

        //roundingFunction = (roundingFunction !== undefined ? roundingFunction : Math.round);
        // Multiply up by precision, round accurately, then divide and use native toFixed():
        output = ( roundingFunction( value * power ) / power ).toFixed( precision );

        if ( optionals ) {
            optionalsRegExp = new RegExp( '0{1,' + optionals + '}$' );
            output = output.replace( optionalsRegExp, '' );
        }

        return output;
    }
    function formatNumeral( _value, format, roundingFunction ) {
        var output;

        // figure out what kind of format we are dealing with
        if ( format.indexOf( '$' ) > -1 ) { // currency!!!!!
            output = formatCurrency( _value, format, roundingFunction );
        } else if ( format.indexOf( '%' ) > -1 ) { // percentage
            output = formatPercentage( _value, format, roundingFunction );
        } else if ( format.indexOf( ':' ) > -1 ) { // time
            output = formatTime( _value, format );
        } else { // plain ol' numbers or bytes
            output = formatNumber( _value, format, roundingFunction );
        }

        // return string
        return output;
    }
    function formatTime( _value ) {
        var hours = Math.floor( _value / 60 / 60 ),
            minutes = Math.floor( ( _value - ( hours * 60 * 60 ) ) / 60 ),
            seconds = Math.round( _value - ( hours * 60 * 60 ) - ( minutes * 60 ) );
        return hours + ':' + ( ( minutes < 10 ) ? '0' + minutes : minutes ) + ':' + ( ( seconds < 10 ) ? '0' + seconds : seconds );
    }
    function formatCurrency( _value, format, roundingFunction ) {
        var symbolIndex = format.indexOf( '$' ),
            openParenIndex = format.indexOf( '(' ),
            minusSignIndex = format.indexOf( '-' ),
            space = '',
            spliceIndex,
            output;

        // check for space before or after currency
        if ( format.indexOf( ' $' ) > -1 ) {
            space = ' ';
            format = format.replace( ' $', '' );
        } else if ( format.indexOf( '$ ' ) > -1 ) {
            space = ' ';
            format = format.replace( '$ ', '' );
        } else {
            format = format.replace( '$', '' );
        }

        // format the number
        output = formatNumber( _value, format, roundingFunction );

        // position the symbol
        if ( symbolIndex <= 1 ) {
            if ( output.indexOf( '(' ) > -1 || output.indexOf( '-' ) > -1 ) {
                output = output.split( '' );
                spliceIndex = 1;
                if ( symbolIndex < openParenIndex || symbolIndex < minusSignIndex ) {
                    // the symbol appears before the "(" or "-"
                    spliceIndex = 0;
                }
                output.splice( spliceIndex, 0, languages[currentLanguage].currency.symbol + space );
                output = output.join( '' );
            } else {
                output = languages[currentLanguage].currency.symbol + space + output;
            }
        } else {
            if ( output.indexOf( ')' ) > -1 ) {
                output = output.split( '' );
                output.splice( -1, 0, space + languages[currentLanguage].currency.symbol );
                output = output.join( '' );
            } else {
                output = output + space + languages[currentLanguage].currency.symbol;
            }
        }

        return output;
    }
    function zeroFormat( format ) {
        zeroFormat = typeof ( format ) === 'string' ? format : null;
    };
    function formatNumber( value, format, roundingFunction ) {
        var negP = false,
            signed = false,
            optDec = false,
            abbr = '',
            abbrK = false, // force abbreviation to thousands
            abbrM = false, // force abbreviation to millions
            abbrB = false, // force abbreviation to billions
            abbrT = false, // force abbreviation to trillions
            abbrForce = false, // force abbreviation
            bytes = '',
            ord = '',
            abs = Math.abs( value ),
            suffixes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'],
            min,
            max,
            power,
            w,
            precision,
            thousands,
            d = '',
            neg = false,
            arr;

        // check if number is zero and a custom zero format has been set
        if ( value === 0 && zeroFormat !== null ) {
            return zeroFormat;
        } else {
            // see if we should use parentheses for negative number or if we should prefix with a sign
            // if both are present we default to parentheses
            if ( format.indexOf( '(' ) > -1 ) {
                negP = true;
                format = format.slice( 1, -1 );
            } else if ( format.indexOf( '+' ) > -1 ) {
                signed = true;
                format = format.replace( /\+/g, '' );
            }

            // see if abbreviation is wanted
            if ( format.indexOf( 'a' ) > -1 ) {
                // check if abbreviation is specified
                abbrK = format.indexOf( 'aK' ) >= 0;
                abbrM = format.indexOf( 'aM' ) >= 0;
                abbrB = format.indexOf( 'aB' ) >= 0;
                abbrT = format.indexOf( 'aT' ) >= 0;
                abbrForce = abbrK || abbrM || abbrB || abbrT;

                // check for space before abbreviation
                if ( format.indexOf( ' a' ) > -1 ) {
                    abbr = ' ';
                    format = format.replace( ' a', '' );
                } else {
                    format = format.replace( 'a', '' );
                }

                if ( abs >= Math.pow( 10, 12 ) && !abbrForce || abbrT ) {
                    // trillion
                    abbr = abbr + languages[currentLanguage].abbreviations.trillion;
                    value = value / Math.pow( 10, 12 );
                } else if ( abs < Math.pow( 10, 12 ) && abs >= Math.pow( 10, 9 ) && !abbrForce || abbrB ) {
                    // billion
                    abbr = abbr + languages[currentLanguage].abbreviations.billion;
                    value = value / Math.pow( 10, 9 );
                } else if ( abs < Math.pow( 10, 9 ) && abs >= Math.pow( 10, 6 ) && !abbrForce || abbrM ) {
                    // million
                    abbr = abbr + languages[currentLanguage].abbreviations.million;
                    value = value / Math.pow( 10, 6 );
                } else if ( abs < Math.pow( 10, 6 ) && abs >= Math.pow( 10, 3 ) && !abbrForce || abbrK ) {
                    // thousand
                    abbr = abbr + languages[currentLanguage].abbreviations.thousand;
                    value = value / Math.pow( 10, 3 );
                }
            }

            // see if we are formatting bytes
            if ( format.indexOf( 'b' ) > -1 ) {
                // check for space before
                if ( format.indexOf( ' b' ) > -1 ) {
                    bytes = ' ';
                    format = format.replace( ' b', '' );
                } else {
                    format = format.replace( 'b', '' );
                }

                for ( power = 0; power <= suffixes.length; power++ ) {
                    min = Math.pow( 1024, power );
                    max = Math.pow( 1024, power + 1 );

                    if ( value >= min && value < max ) {
                        bytes = bytes + suffixes[power];
                        if ( min > 0 ) {
                            value = value / min;
                        }
                        break;
                    }
                }
            }

            // see if ordinal is wanted
            if ( format.indexOf( 'o' ) > -1 ) {
                // check for space before
                if ( format.indexOf( ' o' ) > -1 ) {
                    ord = ' ';
                    format = format.replace( ' o', '' );
                } else {
                    format = format.replace( 'o', '' );
                }

                ord = ord + languages[currentLanguage].ordinal( value );
            }

            if ( format.indexOf( '[.]' ) > -1 ) {
                optDec = true;
                format = format.replace( '[.]', '.' );
            }

            w = value.toString().split( '.' )[0];
            precision = format.split( '.' )[1];
            thousands = format.indexOf( ',' );

            if ( precision ) {
                if ( precision.indexOf( '[' ) > -1 ) {
                    precision = precision.replace( ']', '' );
                    precision = precision.split( '[' );
                    d = toFixed( value, ( precision[0].length + precision[1].length ), roundingFunction, precision[1].length );
                } else {
                    d = toFixed( value, precision.length, roundingFunction );
                }
                arr = d.split( '.' );
                w = arr[0];

                if ( arr[1] && arr[1].length ) {
                    d = languages[currentLanguage].delimiters.decimal + arr[1];
                } else {
                    d = '';
                }

                if ( optDec && Number( d.slice( 1 ) ) === 0 ) {
                    d = '';
                }
            } else {
                w = toFixed( value, null, roundingFunction );
            }

            // format number
            if ( w.indexOf( '-' ) > -1 ) {
                w = w.slice( 1 );
                neg = true;
            }

            if ( thousands > -1 ) {
                w = w.toString().replace( /(\d)(?=(\d{3})+(?!\d))/g, '$1' + languages[currentLanguage].delimiters.thousands );
            }

            if ( format.indexOf( '.' ) === 0 ) {
                w = '';
            }

            return ( ( negP && neg ) ? '(' : '' ) + ( ( !negP && neg ) ? '-' : '' ) + ( ( !neg && signed ) ? '+' : '' ) + w + d + ( ( ord ) ? ord : '' ) + ( ( abbr ) ? abbr : '' ) + ( ( bytes ) ? bytes : '' ) + ( ( negP && neg ) ? ')' : '' );
        }
    };
    exports.format = exports.format || {};
    exports.format.quantity = function ( number ) {
        return this.number.f( number ).format( "0,000" );
    };
    exports.format.amount = function ( number ) {
        number = this.number.d( number );
        if ( number === 0 ) return "0.00";
        return this.number.f( number ).format( "0,000.00" );
    };
    exports.format.number = {
        d: function ( val, prussian ) {
            if ( !val ) return 0;
            val = parseFloat( val );
            if ( isNaN( val ) ) return 0;
            prussian = typeof ( prussian ) !== 'number' ? 2 : prussian;
            return parseFloat( Number( val ).toFixed( prussian ) );
        },
        f: function ( number ) {
            var value, task;
            value = number === undefined || !number || number === '-' ? 0.00 : number;
            task = 'number';
            return {
                format: function ( inputString, roundingFunction ) {
                    return task === 'number' ? ( value === 0 || !value ? '' : formatNumeral( value,
                        inputString ? inputString : defaultFormat,
                        ( roundingFunction !== undefined ) ? roundingFunction : Math.round
                    ) )
                        : task === 'date' ? dateFormat( value, inputString ) : '';
                }
            }
        }
    };
}( typeof ( module ) !== 'undefined' ? module.exports : self || this ) );
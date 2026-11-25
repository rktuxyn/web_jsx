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
class system {
    static kill_process_by_name( p_name ) {
        try {
            return sys.kill_process_by_name( p_name );
        } catch ( e ) {
            return -1;
        }
    }
    static create_process( settings ) {
        if ( settings === null || typeof ( settings ) !== "object" )
            throw new Error( "Settings not defined!!!" );
        let pid = 0, max = 10, error="";
        while ( true ) {
            if ( max < 0 ) break;
            try {
                pid = sys.create_process( settings );
                if ( pid > 0 ) break;
            } catch ( e ) {
                max--;
				error += e.message +"\n";
                continue;
            }
        }
        return { pid, max, error };
    }
}
module.exports = {
    system: system
};

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
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_web_jsx_app_global_h)
#	define _web_jsx_app_global_h
#if !defined(SUCCESS)
#	define SUCCESS 1
#endif//!SUCCESS
#	include <iostream>
#	include "core/web_jsx_exp.h"
#	include "core/js_compiler.h"
#	include <stdio.h>  /* defines FILENAME_MAX */
#	include <list>
#	include <map>
#	include <stdlib.h>
#	include <regex>
#	include <cstdlib>
#	include <sstream>
#	include <future>
#if !(defined(_WIN32)||defined(_WIN64))
#	include <io.h>
#if !defined(SET_BINARY_MODE)
#	define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#if !defined(__file_exists)
#	include <sys/stat.h>
#	define __file_exists(fname)\
access(fname, 0)!=-1
#endif//!__file_exists
#else
#	include <windows.h>
#	include <fcntl.h>
#	include <io.h>
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#	define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#elif defined(_WIN32) || defined(MSDOS) || defined(OS2)
#	define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#	define SET_BINARY_MODE_OUT() _setmode(_fileno(__acrt_iob_func(1)), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#if !defined(__file_exists)
#	define __file_exists(fname)\
_access(fname, 0)!=-1
#endif//!__file_exists
#endif//_WIN32||__unix__
#if !defined(CHUNK)
#	define CHUNK 16384
#endif//!CHUNK
#if defined(FAST_CGI_APP)
#if !defined(H_N_L)
#	define H_N_L "\r\n"
#endif//!H_N_L
#else
#if !defined(H_N_L)
#	define H_N_L "\n"
#endif//!H_N_L
#endif//FAST_CGI_APP
#pragma warning(disable : 6031)
#endif//!_web_jsx_app_global_h

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_web_jsx_app_global_h)
#define _web_jsx_app_global_h
#if !defined(SUCCESS)
#define SUCCESS 1
#endif//!SUCCESS
#if !defined(_IOSTREAM_)
#include <iostream>
#endif//!_IOSTREAM_
//#undef FAST_CGI_APP
#if !defined(_web_jsx_exp_h)
//#include <web_jsx_exp.h>
#include "core/web_jsx_exp.h"
#endif//!_web_jsx_exp_h
#if !defined(_js_compiler_h)
//#include <js_compiler.h>
#include "core/js_compiler.h"
#endif//!_js_compiler_h
#if !defined(_INC_STDIO)
#include <stdio.h>  /* defines FILENAME_MAX */
#endif//!_INC_STDIO
#if !defined(_LIST_)
#include <list>
#endif // !_LIST_
#if !defined(_MAP_)
#include <map>
#endif // !_MAP_
#if !defined(_INC_STDLIB)
#include <stdlib.h>
#endif // !_INC_STDLIB
#if !defined(_REGEX_)
#include <regex>
#endif// !_REGEX_
#if !defined(_CSTDLIB_)
#include <cstdlib>
#endif//_CSTDLIB_
#if !defined(_SSTREAM_)
#include <sstream>
#endif//_SSTREAM_
#if !defined(_FUTURE_)
#include <future>
#endif//!_FUTURE_
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#if !defined(__file_exists)
#include <sys/stat.h>
#define __file_exists(fname)\
access(fname, 0)!=-1
#endif//!__file_exists
#else
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#if !defined(_INC_FCNTL)
#  include <fcntl.h>
#endif//_INC_FCNTL
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#elif defined(_WIN32) || defined(MSDOS) || defined(OS2)
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#  define SET_BINARY_MODE_OUT() _setmode(_fileno(__acrt_iob_func(1)), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#if !defined(__file_exists)
#define __file_exists(fname)\
_access(fname, 0)!=-1
#endif//!__file_exists
#endif//_WIN32||__unix__
#if !defined(_encryption_h)
#include <encryption.h>
#endif//!_encryption_h
#if !defined(CHUNK)
#define CHUNK 16384
#endif//!CHUNK
#if !defined(_npgsql_h)
#include <npgsql.h>
#endif//!_npgsql_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
#if defined(FAST_CGI_APP)
#if !defined(H_N_L)
#define H_N_L "\r\n"
#endif//!H_N_L
#else
#if !defined(H_N_L)
#define H_N_L "\n"
#endif//!H_N_L
#endif//FAST_CGI_APP
#pragma warning(disable : 6031)
//#pragma warning(disable : 6280)
#endif//!_global_h

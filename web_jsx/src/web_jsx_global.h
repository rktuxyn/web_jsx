/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#pragma once
#if !defined(_web_jsx_global_h)
#define _web_jsx_global_h
#if !defined(_IOSTREAM_)
#include <iostream>
#endif // !_IOSTREAM_
#if !defined(_INC_STDLIB)
#include <stdlib.h>
#endif // !_INC_STDLIB
#if !defined(_SSTREAM_)
#include <sstream>
#endif//_SSTREAM_
#if !defined(_FUTURE_)
#include <future>
#endif//!_FUTURE_
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
#if !defined(_REGEX_)
#include <regex>
#endif// !_REGEX_
#if !defined(_LIST_)
#include <list>
#endif // !_LIST_
#if !defined(_MAP_)
#include <map>
#endif // !_MAP_
#if !defined(_VECTOR_)
#include <vector>
#endif//!_VECTOR_
#if !defined(_std_wrapper_h)
#include "std_wrapper.hpp"
#endif // !STD_EXTEND_REGEX
#if defined(_STR_FRMT)
#if !defined(_INC_STDARG)
#include<stdarg.h>
#endif // !_INC_STDARG
#endif // _STR_FRMT
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <sys/stat.h>
#define __file_exists(fname)\
access(fname, 0)!=-1
#else
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#if !defined(_INC_IO)
#include  <io.h> 
#endif//_INC_IO
#define __file_exists(fname)\
_access(fname, 0)!=-1
#endif//_WIN32||__unix__
#if !defined(READ_CHUNK)
#define READ_CHUNK 16384
#endif//!READ_CHUNK
//#define __WIN_API
//#define __EXPORT
#define jsx_export __declspec(dllexport)
#define __client_build
#undef __client_build
#ifdef __cplusplus 
extern "C" {
#endif
	typedef struct {
		std::string t_source;
		std::string err_msg;
		bool is_error;
		bool is_script;
		bool is_script_template;
		bool remove_new_line;
		bool is_strict;
	} template_result;
	typedef struct {
		const char* page_path;
		const char* dir;
		const char* data;
	} parser_settings;
#ifdef __cplusplus
}
#endif
namespace sow_web_jsx {
	void format__path(std::string&str);
	void get_dir_from_path (const std::string& path_str, std::string&dir);
	int dir_exists(const char* dir);
	int create_directory(const char* dir);
	jsx_export size_t read_file(const char*absolute_path, std::stringstream&ssstream, bool check_file_exists);
	size_t read_file(const char*absolute_path, std::string&str, bool check_file_exists);
	char* read_file(const char* absolute_path, bool check_file_exists);
};
#endif //!_web_jsx_global_h

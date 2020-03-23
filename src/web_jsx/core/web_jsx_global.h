/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER

#if !defined(_web_jsx_global_h)
#	define _web_jsx_global_h

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

#	include <string>
#	include <uv.h>
#if !(defined(_WIN32)||defined(_WIN64))
#	include <sys/stat.h>
#	define __file_exists(fname) access(fname, 0)!=-1
#	include <io.h>
#else
#	include <io.h>
#if !defined(__file_exists)
#	define __file_exists(fname) _access(fname, 0)!=-1
#endif//!__file_exists

#endif//_WIN32||__unix__

#if !defined(READ_CHUNK)
#	define READ_CHUNK		16384
#endif//!READ_CHUNK

#if !defined(H_N_L)
#	define H_N_L "\r\n"
#endif//!H_N_L

#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj
#if !defined(_free_char)
#	define _free_char(obj)\
while(obj){\
	delete[] obj; obj = NULL;\
}
#endif//!_free_char

typedef struct {
	std::string start_in;
	int wait_for_exit;
	std::string process_name;
	std::string process_path;
	std::string lp_title;
	std::string arg;
	int dw_creation_flags;
	short show_window;
}process_info;
namespace sow_web_jsx {
	void format__path(std::string&str);
	void get_dir_from_path (const std::string& path_str, std::string&dir);
	
	std::istream& get_line(std::istream& is, std::string& t);
	bool strings_equal(
		const std::string& s1,
		const std::string& s2,
		size_t n
	);
	bool strings_equal(
		const std::string& s1,
		const std::string& s2
	);
	std::string extract_between(
		const std::string& data,
		const std::string& separator1,
		const std::string& separator2
	);
	template<class _input>
	inline int is_error_code(_input ret) {
		return (ret == std::string::npos || ret == FALSE || ret < 0) ? TRUE : FALSE;
	}
	wchar_t* ccr2ws(const char* s);
};
#endif //!_web_jsx_global_h
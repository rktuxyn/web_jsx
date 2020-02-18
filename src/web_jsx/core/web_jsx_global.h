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
#if defined(__WJSX_SHARED)
#if !defined(_export_wjsx)
#	define _export_wjsx __declspec(dllexport)
#endif//!jsx_export
#else
#if !defined(_export_wjsx)
#	define _export_wjsx
#endif//!_export_wjsx
#endif//__WJSX_SHARED
#	include <iostream>
#	include <stdlib.h>
#	include <sstream>
#	include <future>
#	include <string>
#	include <regex>
#	include <list>
#	include <map>
#	include <vector>
#	include "std_wrapper.hpp"
#if defined(_STR_FRMT)
#if !defined(_INC_STDARG)
#	include	<stdarg.h>
#endif // !_INC_STDARG
#endif // _STR_FRMT
#	include <uv.h>
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#	include <sys/stat.h>
#	define __file_exists(fname)\
access(fname, 0)!=-1
#	include <io.h>
#if !defined(SET_BINARY_MODE)
#	define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#else
#	include <assert.h>
#	include <dirent.h>
#	include <direct.h>
#	include <windows.h>
#	include <tchar.h>
#	include <io.h> 
#	include <winbase.h>
#	include <tlhelp32.h>
#	define __file_exists(fname)\
_access(fname, 0)!=-1
#	include <fcntl.h>
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#	define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#else
#	define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#	define SET_BINARY_MODE_OUT() _setmode(_fileno(__acrt_iob_func(1)), _O_BINARY)
#	define SET_BINARY_MODE_IN() _setmode(_fileno(__acrt_iob_func(0)), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#endif//_WIN32||__unix__
#	include "directory_.h"
#	include "glb_r.h"
#if !defined(READ_CHUNK)
#	define READ_CHUNK		16384
#endif//!READ_CHUNK
#if defined(FAST_CGI_APP)
#if !defined(H_N_L)
#	define H_N_L "\r\n"
#endif//!H_N_L
#else
#if !defined(H_N_L)
#	define H_N_L "\n"
#endif//!H_N_L
#endif//FAST_CGI_APP
#	define FORCE_EXIT_PROCESS	9
#pragma warning (disable : 4996)
#	define _CRT_SECURE_NO_WARNINGS

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
//template<class _stream>
//size_t get_sizeof_stream(_stream&strm) {
//	strm.seekg(0, std::ios::end);//Go to end of stream
//	std::streamoff totalSize = strm.tellg();
//	strm.seekg(0, std::ios::beg);//Back to begain of stream
//	return (size_t)totalSize;
//}
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
	int create_process(const process_info*pi);
	long create_child_process(const char*process_path, const char*arg);
	int open_process(const char*process_path, const char*arg);
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
	int fprintf_stdout(const char* msg);
	int fprintf_stderr(const char* msg);
	template<class _input>
	inline int is_error_code(_input ret) {
		return (ret == std::string::npos || ret == FALSE || ret < 0) ? TRUE : FALSE;
	}
#if defined(_WIN32)||defined(_WIN64)
#if !defined(_close_handle)
#	define _close_handle(handle)\
if (CloseHandle(handle)){\
	handle = INVALID_HANDLE_VALUE;\
}
#endif//!_close_handle
	wchar_t* ccr2ws(const char* s);
	const char* get_error_desc(int error_code);
	int kill_process_by_name(const char *process_name);
	int process_is_running(DWORD dwPid);
	int terminate_process(DWORD dwPid);
	DWORD current_process_id();
	//https://stackoverflow.com/questions/22088234/redirect-the-stdout-of-a-child-process-to-the-parent-process-stdin
	enum { ParentRead, ParentWrite, ChildWrite, ChildRead, NumPipeTypes };
	template<class func>
	int read_child_process(const process_info*pi, func fn) {
		if (pi->process_path.empty()) return -10;
		if (__file_exists(pi->process_path.c_str()) == false) return -4;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = nullptr;
		HANDLE pipes[NumPipeTypes];
		if (!CreatePipe(&pipes[ParentWrite], &pipes[ChildRead], &sa, 0)) return -1;
		if (!CreatePipe(&pipes[ParentRead], &pipes[ChildWrite], &sa, 0)) return -1;
		// make sure the handles the parent will use aren't inherited.
		SetHandleInformation(pipes[ParentRead], HANDLE_FLAG_INHERIT, 0);
		SetHandleInformation(pipes[ParentWrite], HANDLE_FLAG_INHERIT, 0);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.wShowWindow = SW_SHOW;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.hStdOutput = pipes[ChildWrite];
		si.hStdError = pipes[ChildWrite];
		si.hStdInput = pipes[ChildRead];
		PROCESS_INFORMATION pinfo;
		ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
		//TCHAR cmd[] = _T(arg);
		std::string* cmd_args= new std::string(pi->process_name);
		cmd_args->append(" "); cmd_args->append(pi->arg);
		PWSTR pszCmdLine = (PWSTR)ccr2ws(cmd_args->c_str());
		_free_obj(cmd_args);
		PWSTR pszApp = (PWSTR)ccr2ws(pi->process_path.c_str());
		LPWSTR current_directory = NULL;
		if (!pi->start_in.empty())
			current_directory = (LPWSTR)ccr2ws(pi->start_in.c_str());
		if (!CreateProcess(pszApp/*lpApplicationName*/,
			pszCmdLine/*lpCommandLine*/,
			NULL/*lpProcessAttributes*/,
			NULL/*lpThreadAttributes*/,
			TRUE/*bInheritHandles*/,
			NULL,//CREATE_NEW_PROCESS_GROUP/*dwCreationFlags*/,
			NULL/*lpEnvironment*/,
			current_directory/*lpCurrentDirectory*/,
			&si/*lpStartupInfo*/,
			&pinfo/*lpProcessInformation*/))
			return -1;
		_close_handle(pinfo.hProcess);
		_close_handle(pinfo.hThread);
		_close_handle(pipes[ChildRead]);
		_close_handle(pipes[ChildWrite]);
		_close_handle(pipes[ParentWrite]);
		unsigned read_len = 4096;
		DWORD read_num;
		for (;;) {
			char* buff = new char[(read_len + 1)];
			buff[read_len] = '\0';
			BOOL success = ReadFile(pipes[ParentRead], buff, sizeof(buff) - 1, &read_num, NULL);
			if (!success || !read_num) {
				delete[]buff;
				break;
			}
			buff[read_num] = 0;
			fn(static_cast<size_t>(read_num), const_cast<const char*>(buff));
			delete[]buff;
		}
		return 1;
	}
#endif//!_WINDOWS_
	size_t read_file(const char* absolute_path, std::stringstream& ssstream, bool check_file_exists);
	size_t read_file(const char*absolute_path, std::string&str, bool check_file_exists);
	char* read_file(const char* absolute_path, bool check_file_exists);
};
#endif //!_web_jsx_global_h
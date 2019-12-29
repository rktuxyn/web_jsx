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
#if !defined(UV_H)
#include <uv.h>
#endif//!UV_H
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <sys/stat.h>
#define __file_exists(fname)\
access(fname, 0)!=-1
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#else
#include <assert.h>
#if !defined(DIRENT_H)
#include <dirent.h>
#endif//!DIRENT_H
#if !defined(_INC_DIRECT)
#include <direct.h>
#endif//!_INC_DIRECT
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#if !defined(_INC_TCHAR)
#include <tchar.h>
#endif//!_INC_TCHAR
#if !defined(_INC_IO)
#include  <io.h> 
#endif//_INC_IO
#if !defined(_WINBASE_)
#include <winbase.h>
#endif//!_WINBASE_
#if !defined(_INC_TOOLHELP32)
#include <tlhelp32.h>
#endif//!_INC_TOOLHELP32
#define __file_exists(fname)\
_access(fname, 0)!=-1
#if !defined(_INC_FCNTL)
#  include <fcntl.h>
#endif//_INC_FCNTL
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#else
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#  define SET_BINARY_MODE_OUT() _setmode(_fileno(__acrt_iob_func(1)), _O_BINARY)
#  define SET_BINARY_MODE_IN() _setmode(_fileno(__acrt_iob_func(0)), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#endif//_WIN32||__unix__
#if !defined(directory__h)
#include "directory_.h"
#endif//!directory__h
#if !defined(_glb_r_h)
#include "glb_r.h"
#endif//!_glb_r_h
#if !defined(READ_CHUNK)
#define READ_CHUNK		16384
#endif//!READ_CHUNK
//#define __WIN_API
//#define __EXPORT
#if defined(FAST_CGI_APP)
#if !defined(H_N_L)
#define H_N_L "\r\n"
#endif//!H_N_L
#else
#if !defined(H_N_L)
#define H_N_L "\n"
#endif//!H_N_L
#endif//FAST_CGI_APP
#if defined(jsx_shared)
#if !defined(jsx_export)
#define jsx_export __declspec(dllexport)
#endif//!jsx_export
#else
#if !defined(jsx_export)
#define jsx_export
#endif//!jsx_export
#endif//jsx_shared
#define __client_build
//#undef __client_build
#define FORCE_EXIT_PROCESS	9
//#define _fgets	 fgets
//#define _stdin stdin
#ifdef __cplusplus
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
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
#ifdef __cplusplus
}
#endif
namespace sow_web_jsx {
	void format__path(std::string&str);
	void get_dir_from_path (const std::string& path_str, std::string&dir);
	/*int dir_exists(const char* dir);
	int create_directory(const char* dir);
	//int delete_directory(const char* dir);*/
	int create_process(const process_info*pi);
	long create_child_process(const char*process_path, const char*arg);
	int open_process(const char*process_path, const char*arg);
	std::istream& get_line(std::istream& is, std::string& t);
#if defined(_WINDOWS_)
	wchar_t* ccr2ws(const char* s);
	int kill_process_by_name(const char *process_name);
	int process_is_running(DWORD dwPid);
	int terminate_process(DWORD dwPid);
	DWORD current_process_id();
	//https://stackoverflow.com/questions/22088234/redirect-the-stdout-of-a-child-process-to-the-parent-process-stdin
	enum { ParentRead, ParentWrite, ChildWrite, ChildRead, NumPipeTypes };
	template<class func>
	int read_child_process(const process_info*pi, func fn) {
		if (pi->process_path.empty())
			return -10;
		if (__file_exists(pi->process_path.c_str()) == false)
			return -4;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = nullptr;

		HANDLE pipes[NumPipeTypes];
		if (!CreatePipe(&pipes[ParentWrite], &pipes[ChildRead], &sa, 0))
			return -1;
		if (!CreatePipe(&pipes[ParentRead], &pipes[ChildWrite], &sa, 0))
			return -1;
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
		std::string cmd_args(pi->process_name);
		cmd_args.append(" ").append(pi->arg);
		PWSTR pszCmdLine = (PWSTR)ccr2ws(cmd_args.c_str());
		std::string().swap(cmd_args);
		PWSTR pszApp = NULL;
		if (!pi->process_path.empty())
			pszApp = (PWSTR)ccr2ws(pi->process_path.c_str());
		LPWSTR current_directory = NULL;
		if (!pi->start_in.empty())
			current_directory = (LPWSTR)ccr2ws(pi->start_in.c_str());
		if (!CreateProcess(pszApp/*lpApplicationName*/,
			pszCmdLine/*lpCommandLine*/,
			NULL/*lpProcessAttributes*/,
			NULL/*lpThreadAttributes*/,
			TRUE/*bInheritHandles*/,
			CREATE_NEW_PROCESS_GROUP/*dwCreationFlags*/,
			NULL/*lpEnvironment*/,
			current_directory/*lpCurrentDirectory*/,
			&si/*lpStartupInfo*/,
			&pinfo/*lpProcessInformation*/))
			return -1;
		/*if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pinfo))
			return -1;*/
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
		CloseHandle(pipes[ChildRead]);
		CloseHandle(pipes[ChildWrite]);
		CloseHandle(pipes[ParentWrite]);
		char ReadBuff[4096 + 1];
		DWORD ReadNum;
		for (;;) {
			BOOL success = ReadFile(pipes[ParentRead], ReadBuff, sizeof(ReadBuff) - 1, &ReadNum, NULL);
			if (!success || !ReadNum)
				break;
			ReadBuff[ReadNum] = 0;
			fn(ReadNum, const_cast<const char*>(ReadBuff));
		}
		return 1;
	}
#endif//!_WINDOWS_
	jsx_export size_t read_file(const char*absolute_path, std::stringstream&ssstream, bool check_file_exists);
	size_t read_file(const char*absolute_path, std::string&str, bool check_file_exists);
	char* read_file(const char* absolute_path, bool check_file_exists);
	const char* get_error_desc(int error_code);
};
#endif //!_web_jsx_global_h

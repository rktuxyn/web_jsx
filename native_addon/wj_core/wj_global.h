/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:25 PM 3/9/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_wj_global_h)
#define _wj_global_h
#	include <uv.h>
#if (defined(_WIN32)||defined(_WIN64))
#	include <dirent.h>
#	include <direct.h>
#	include <windows.h>
#	include <tchar.h>
#	include <io.h> 
#	include <winbase.h>
#	include <tlhelp32.h>
#	include "directory_.h"

#if !defined(__file_exists)
#	define __file_exists(fname) _access(fname, 0)!=-1
#endif//!__file_exists

#if !defined(_close_handle)
#	define _close_handle(handle)\
if (CloseHandle(handle)){\
	handle = INVALID_HANDLE_VALUE;\
}
#endif//!_close_handle

#else

#endif//!_WIN32|_WIN64

#	include <web_jsx/web_jsx.h>
#	include <string>

#	define FORCE_EXIT_PROCESS	9

#if !defined(H_N_L)
#	define H_N_L "\r\n"
#endif//!H_N_L

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
	int create_process(const process_info* pi);
	long create_child_process(const char* process_path, const char* arg);
	int open_process(const char* process_path, const char* arg);
	int kill_process_by_name(const char* process_name);
	int fprintf_stdout(const char* msg);
	int fprintf_stderr(const char* msg);
#if (defined(_WIN32)||defined(_WIN64))
	wchar_t* ccr2ws(const char* mbstr);
	int process_is_running(DWORD dwPid);
	int terminate_process(DWORD dwPid);
	DWORD current_process_id();
	//Read more https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
	enum { ParentRead, ParentWrite, ChildWrite, ChildRead, NumPipeTypes };
	template<class func>
	int read_child_process(const process_info* pi, func fn) {
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
		std::string* cmd_args = new std::string(pi->process_name);
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
		_close_handle(pinfo.hProcess);
		_close_handle(pinfo.hThread);
		_close_handle(pipes[ChildRead]);
		_close_handle(pipes[ChildWrite]);
		_close_handle(pipes[ParentWrite]);
		return 1;
	}
#endif//!_WIN32|_WIN64
	size_t read_file(const char* absolute_path, std::stringstream& ssstream, bool check_file_exists);
	size_t read_file(const char* absolute_path, std::string& str, bool check_file_exists);
}
#endif//!_wj_global_h
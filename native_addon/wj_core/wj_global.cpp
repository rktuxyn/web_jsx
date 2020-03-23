/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:05 PM 3/9/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#	include <assert.h>
#	include "wj_global.h"
#	include <fstream>
#pragma warning(push)
#pragma warning(disable : 4996)
wchar_t* sow_web_jsx::ccr2ws(const char* mbstr) {
	mbstate_t state;
	memset(&state, 0, sizeof state);
	size_t len = sizeof(wchar_t) + mbsrtowcs(NULL, &mbstr, 0, &state);
	wchar_t* buf = new wchar_t[len];
	mbsrtowcs(buf, &mbstr, len, &state);
	return buf;
}
#pragma warning(pop)
int sow_web_jsx::create_process(const process_info* pi) {
	if (pi->process_path.empty())return -10;
	if (__file_exists(pi->process_path.c_str()) == false)return -4;
	PROCESS_INFORMATION pinfo;
	ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	if (pi->dw_creation_flags != CREATE_NEW_PROCESS_GROUP)
		si.dwFlags |= STARTF_USESHOWWINDOW;
	si.cb = sizeof(si);
	si.wShowWindow = pi->show_window || SW_SHOWNORMAL;// SW_HIDE;
	if (!pi->lp_title.empty())
		si.lpTitle = (PWSTR)ccr2ws(pi->lp_title.c_str());
	PWSTR pszApp = NULL;
	if (pi->dw_creation_flags != CREATE_NEW_PROCESS_GROUP) {
		pszApp = (PWSTR)ccr2ws(pi->process_path.c_str());
	}
	//current_directory
	LPWSTR current_directory = NULL;
	if (!pi->start_in.empty()) {
		current_directory = (LPWSTR)ccr2ws(pi->start_in.c_str());
	}
	std::string cmd_args;
	if (pi->dw_creation_flags == CREATE_NEW_PROCESS_GROUP) {
		PCWSTR pszCmd = (PWSTR)ccr2ws(pi->process_path.c_str());
		PWSTR pcl;
		HRESULT hr = SHEvaluateSystemCommandTemplate(pszCmd, &pszApp, &pcl, NULL);
		CoTaskMemFree(pcl);
		if (!SUCCEEDED(hr)) return -9;
		cmd_args = pi->process_path;
		cmd_args.append(" ").append(pi->arg);
	}
	else {
		cmd_args = pi->process_name;
		cmd_args.append(" ").append(pi->arg);
	}
	PWSTR pszCmdLine = (PWSTR)ccr2ws(cmd_args.c_str());
	std::string().swap(cmd_args);
	//CREATE_NO_WINDOW
	//CREATE_NEW_PROCESS_GROUP
	if (CreateProcess(pszApp/*lpApplicationName*/,
		pszCmdLine/*lpCommandLine*/,
		NULL/*lpProcessAttributes*/,
		NULL/*lpThreadAttributes*/,
		FALSE,//pi->dw_creation_flags == CREATE_NEW_PROCESS_GROUP ? FALSE : TRUE/*bInheritHandles*/,
		pi->dw_creation_flags,//CREATE_NO_WINDOW/*dwCreationFlags*/,
		NULL/*lpEnvironment*/,
		current_directory/*lpCurrentDirectory*/,
		&si/*lpStartupInfo*/,
		&pinfo/*lpProcessInformation*/)) {
		//  we are good
		DWORD dwPid = GetProcessId(pinfo.hProcess);
		//HANDLE explorer;
		//explorer = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
		//TerminateProcess(explorer, 1);
		//_close_handle(explorer);
		if (pi->wait_for_exit > 0 || pi->dw_creation_flags == CREATE_NEW_PROCESS_GROUP) {
			//Wait until child process exits.
			WaitForSingleObject(pinfo.hProcess, INFINITE);
			_close_handle(pinfo.hProcess);
			_close_handle(pinfo.hThread);
		}
		CoTaskMemFree(pszApp);
		CoTaskMemFree(pszCmdLine);
		return (int)dwPid;
	}
	//660
	//https://docs.microsoft.com/en-us/windows/desktop/debug/system-error-codes--0-499
	return (int)GetLastError();
}
long sow_web_jsx::create_child_process(const char* process_path, const char* arg) {
	if (__file_exists(process_path) == false)return -1;
	PCWSTR pszCmd = (PCWSTR)ccr2ws(process_path);
	PWSTR pszApp;
	PWSTR pszCmdLine;// = (PWSTR)s2ws(arg);
	HRESULT hr = SHEvaluateSystemCommandTemplate(pszCmd, &pszApp, &pszCmdLine, NULL);
	if (SUCCEEDED(hr)) {
		std::string cmd(process_path);
		cmd.append(" ").append(arg);
		pszCmdLine = (PWSTR)ccr2ws(cmd.c_str());
		std::string().swap(cmd);
		//  if this was a real template, maybe some kind of wnsprintf() first?
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		STARTUPINFO si = { 1 };
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(si);
		si.wShowWindow = SW_HIDE;
		if (CreateProcess(pszApp, pszCmdLine, NULL, NULL, FALSE,
			CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
			//  we are good
			assert(hr == S_OK);
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);
			_close_handle(pi.hProcess);
			_close_handle(pi.hThread);
		}
		else {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
		CoTaskMemFree(pszApp);
		CoTaskMemFree(pszCmdLine);
	}
	return (long)hr;
}
int sow_web_jsx::open_process(const char* process_path, const char* arg) {
	if (__file_exists(process_path) == false)
		return -1;
	std::string str("start ");
	str += process_path;
	str += " ";
	str += arg;
	system(str.c_str());
	str.clear(); std::string().swap(str);
	return 1;
}
int sow_web_jsx::kill_process_by_name(const char* process_name) {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	if (hSnapShot == INVALID_HANDLE_VALUE || hSnapShot == NULL)return -1;
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	//BOOL hRes = Process32First(hSnapShot, &pEntry);
	if (Process32First(hSnapShot, &pEntry)) {
		wchar_t* p_name = ccr2ws(process_name);
		const wchar_t* cp_name = const_cast<const wchar_t*>(p_name);
		do {
			if (wcscmp(pEntry.szExeFile, cp_name) == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
					(DWORD)pEntry.th32ProcessID);
				if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) {
					TerminateProcess(hProcess, FORCE_EXIT_PROCESS);
					_close_handle(hProcess);
				}
			}
		} while (Process32Next(hSnapShot, &pEntry));
		delete[]p_name;
	}
	_close_handle(hSnapShot);
	return 1;
}
int sow_web_jsx::fprintf_stdout(const char* msg) {
	int rec = fprintf(stdout, "%s\n", msg);
	fflush(stdout);
	return rec;
}
int sow_web_jsx::fprintf_stderr(const char* msg) {
	int rec = fprintf_s(stderr, "%s\n", msg);
	fflush(stderr);
	return rec;
}
int sow_web_jsx::process_is_running(DWORD dwPid) {
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hProcess == INVALID_HANDLE_VALUE || hProcess == NULL)return -1;
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(pe);
	int ret = ((int)dwPid) * -1;
	if (Process32First(hProcess, &pe)) {
		do {
			if (pe.th32ProcessID == dwPid) {
				ret = (int)dwPid;
				break;
			}
		} while (Process32Next(hProcess, &pe));
	}
	_close_handle(hProcess);
	return ret;
}
int sow_web_jsx::terminate_process(DWORD dwPid) {
	HANDLE explorer;
	explorer = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
	int ret = FALSE;
	if (!(explorer == INVALID_HANDLE_VALUE || explorer == NULL)) {
		DWORD le = GetLastError();
		if (le == ERROR_ACCESS_DENIED)ret = -501;
		if (le == ERROR_INVALID_PARAMETER)ret = -500;
		if (ret == FALSE) {
			::TerminateProcess(explorer, EXIT_FAILURE);
			ret = 1;
		}
		_close_handle(explorer);
	}
	return ret == FALSE ? -1 : ret;
}
DWORD sow_web_jsx::current_process_id() {
	return ::GetCurrentProcessId();
}

size_t sow_web_jsx::read_file(const char* absolute_path, std::stringstream& ssstream, bool check_file_exists) {
	//size_t r_length = -1;
	std::ifstream* file_stream = new std::ifstream(absolute_path, std::ifstream::binary);
	//std::ifstream file_stream(absolute_path, std::ifstream::binary);
	if (!file_stream->is_open()) {
		delete file_stream;
		ssstream << "File not found in#" << absolute_path;
		return -1;
	}
	file_stream->seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = file_stream->tellg();
	size_t total_len = (size_t)totalSize;
	file_stream->seekg(0, std::ios::beg);//Back to begain of stream
	if (total_len == std::string::npos || total_len == 0)return TRUE;
	do {
		if (!file_stream->good())break;
		char* in;
		size_t read_len = totalSize > READ_CHUNK ? READ_CHUNK : totalSize;
		in = new char[read_len];
		file_stream->read(in, read_len);
		totalSize -= read_len;
		ssstream.write(in, read_len);
		/* Free memory */
		delete[]in;
		if (totalSize <= 0) break;
	} while (true);
	file_stream->close(); delete file_stream;
	return total_len;
}
size_t sow_web_jsx::read_file(const char* absolute_path, std::string& str, bool check_file_exists) {
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(absolute_path, ssstream, check_file_exists);
	str = std::string(ssstream.str());
	std::stringstream().swap(ssstream);
	return ret;
}
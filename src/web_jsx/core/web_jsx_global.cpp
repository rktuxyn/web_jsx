/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#include "web_jsx_global.h"
void sow_web_jsx::format__path(std::string&str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
}
void sow_web_jsx::get_dir_from_path (const std::string& path_str, std::string&dir) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
}
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Not Implemented
#else
wchar_t* sow_web_jsx::ccr2ws(const char* s) {
	size_t len = strlen(s);
	wchar_t * buf = new wchar_t[len + sizeof(wchar_t)]();
	mbsrtowcs(buf, &s, len, NULL);
	return buf;
}
int sow_web_jsx::open_process(const char*process_path, const char*arg) {
	if (__file_exists(process_path) == false)
		return -1;
	std::string str("start ");
	str += process_path;
	str += " ";
	str += arg;
	system(str.c_str());
	std::string().swap(str);
	return 1;
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
	CloseHandle(hProcess);
	return ret;
}
int sow_web_jsx::terminate_process(DWORD dwPid) {
	HANDLE explorer;
	explorer = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
	if (explorer == INVALID_HANDLE_VALUE || explorer == NULL)return -1;
	DWORD le = GetLastError();
	if (le == ERROR_ACCESS_DENIED)return -501;
	if (le == ERROR_INVALID_PARAMETER)return -500;
	TerminateProcess(explorer, 1);
	CloseHandle(explorer);
	return 1;
}
DWORD sow_web_jsx::current_process_id() {
	return ::GetCurrentProcessId();
}
//const char* ws2ccr(WCHAR* wc) {
//	char output[256];
//	wcslen(wc);
//	sprintf(output, "%ws", wc);
//}
//bool isRunning(const char* p_name) {
//	HWND hwnd;
//	LPCWSTR pName = (LPCWSTR)sow_web_jsx::ccr2ws(p_name);
//	hwnd = FindWindow(NULL, pName);
//	return hwnd != 0;
//}
//10:40 PM 3/12/2019
int sow_web_jsx::kill_process_by_name(const char *process_name) {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	if (hSnapShot == INVALID_HANDLE_VALUE || hSnapShot == NULL)return -1;
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof (pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	if (Process32First(hSnapShot, &pEntry)) {
		const wchar_t* p_name = sow_web_jsx::ccr2ws(process_name);
		do {
			if (wcscmp (pEntry.szExeFile, p_name) == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
					(DWORD)pEntry.th32ProcessID);
				if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) {
					TerminateProcess(hProcess, FORCE_EXIT_PROCESS);
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(hSnapShot, &pEntry));
	}
	//while (hRes) {
	//	//wcscmp (pEntry.szExeFile, p_name);
	//	//if (strcmp(ws2ccr(pEntry.szExeFile), process_name) == 0) {
	//	if (wcscmp (pEntry.szExeFile, p_name) == 0) {
	//		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
	//			(DWORD)pEntry.th32ProcessID);
	//		if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) {
	//			TerminateProcess(hProcess, FORCE_EXIT_PROCESS);
	//			CloseHandle(hProcess);
	//		}
	//	}
	//	hRes = Process32Next(hSnapShot, &pEntry);
	//}
	CloseHandle(hSnapShot);
	return 1;
}
int sow_web_jsx::create_process(const process_info*pi) {
	if (pi->process_path.empty())
		return -10;
	if (__file_exists(pi->process_path.c_str()) == false)
		return -4;
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
	std::string cmd_args("");
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
		//CloseHandle(explorer);
		if (pi->wait_for_exit > 0 || pi->dw_creation_flags == CREATE_NEW_PROCESS_GROUP) {
			//Wait until child process exits.
			WaitForSingleObject(pinfo.hProcess, INFINITE);
			CloseHandle(pinfo.hProcess);
			CloseHandle(pinfo.hThread);
		}
		CoTaskMemFree(pszApp);
		CoTaskMemFree(pszCmdLine);
		return (int)dwPid;
	};
	//660
	//https://docs.microsoft.com/en-us/windows/desktop/debug/system-error-codes--0-499
	return (int)GetLastError();
}
long sow_web_jsx::create_child_process(const char*process_path, const char*arg) {
	if (__file_exists(process_path) == false)
		return -1;
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
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
		CoTaskMemFree(pszApp);
		CoTaskMemFree(pszCmdLine);
	}
	return (long)hr;
}

#endif
const char * sow_web_jsx::get_error_desc(int error_code) {
	switch (error_code) {
	case ERROR_FILE_NOT_FOUND: return "The system cannot find the file specified!!!";
	case ERROR_PATH_NOT_FOUND: return "The system cannot find the path specified!!!";
	case ERROR_DIRECTORY: return "The directory name is invalid!!!";
	case ERROR_ACCESS_DENIED:return "Access is denied!!!";
	case ERROR_TOO_MANY_OPEN_FILES:return "The system cannot open the file!!!";
	case ERROR_NOT_ENOUGH_MEMORY: return "Not enough memory resources are available to process this command!!!";
	case ERROR_INVALID_NAME: return "The filename, directory name, or volume label syntax is incorrect!!!";
	case ERROR_INVALID_HANDLE:return "The handle is invalid!!!";
	case ERROR_INVALID_ACCESS:return "ERROR_INVALID_ACCESS!!!";
	case ERROR_INVALID_DATA:return "The data is invalid!!!";
	case ERROR_OUTOFMEMORY:return "Not enough memory resources are available to complete this operation!!!";
	case ERROR_INVALID_DRIVE:return "The system cannot find the drive specified!!!";
	case ERROR_SEEK:return "The drive cannot locate a specific area or track on the disk!!!";
	case ERROR_READ_FAULT:return "The system cannot read from the specified device!!!";
	default:return "Unknown error code";
	}
}
size_t read_binary() {
	return -1;
}
size_t sow_web_jsx::read_file(const char*absolute_path, std::stringstream&ssstream, bool check_file_exists) {
	size_t r_length = -1;
	if ( check_file_exists == true ) {
		if (__file_exists(absolute_path) == false) { 
			ssstream << "File not found in#" << absolute_path;
			return r_length;
		}
	}
	FILE*fs;
	errno_t err = fopen_s(&fs, absolute_path, "rb");
	if (fs == NULL || err != 0) { 
		ssstream << get_error_desc(err);
		return r_length;
	}
	fseek(fs, 0, SEEK_END);
	size_t t_length = ftell(fs);
	rewind(fs);
	size_t read_length = 0;
	r_length = 0;
	size_t rlen = 0;
	while (true) {
		rlen = t_length > READ_CHUNK ? READ_CHUNK : t_length;
		char* buff = (char*)malloc(rlen + 1);
		buff[rlen] = '\0';
		read_length = fread(buff, 1, rlen, fs);
		if (ferror(fs)) {
			free(buff);
			fclose(fs);
			std::stringstream().swap(ssstream);
			ssstream << "ERROR OCCURED WHILE READING FILE#" << absolute_path;
			return -2;
		}
		r_length += read_length;
		ssstream.write(buff, read_length);
		free(buff);
		t_length -= read_length;
		if (t_length <= 0)break;
	}
	fclose(fs);
	return r_length;
}
size_t sow_web_jsx::read_file(const char*absolute_path, std::string&str, bool check_file_exists) {
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(absolute_path, ssstream, check_file_exists);
	str = std::string(ssstream.str());
	std::stringstream().swap(ssstream);
	return ret;
}

char* sow_web_jsx::read_file(const char* absolute_path, bool check_file_exists = true) {
	if (check_file_exists) {
		if (__file_exists(absolute_path) == false) {
			return new char[8]{ "INVALID" };
		}
	}
	try {
		FILE*stream;
		errno_t err;
		err = fopen_s(&stream, absolute_path, "rb");
		if (stream == NULL) return new char[8]{ "INVALID" };
		if (err != 0)return new char[8]{ "INVALID" };
		fseek(stream, 0, SEEK_END);
		size_t size = ftell(stream);
		rewind(stream);
		char *chars = new char[size + 1];
		chars[size] = '\0';
		for (size_t i = 0; i < size;) {
			i += fread(&chars[i], 1, size - i, stream);
			if (ferror(stream)) {
				fclose(stream);
				return new char[8]{ "INVALID" };
			}
		}
		fclose(stream);
		stream = NULL;
		return chars;
	} catch (std::exception) {
		return new char[8]{ "INVALID" };
	}
}

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#	include "web_jsx_global.h"
void sow_web_jsx::format__path(std::string&str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
}
void sow_web_jsx::get_dir_from_path (const std::string& path_str, std::string&dir) {
	dir.clear();
	size_t found = path_str.find_last_of("/\\");
	if (found == std::string::npos)return;
	dir = path_str.substr(0, found);
}
//\r\n
std::istream& sow_web_jsx::get_line(std::istream& is, std::string& t) {
	t.clear();
	std::streambuf* sb = is.rdbuf();
	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			t += (char)c;
			return is;
		case '\r':
			t += (char)c;
			c = sb->sgetc();
			if (c == '\n') {
				sb->sbumpc();
				t += (char)c;
			}
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty()) {
				is.setstate(std::ios::eofbit);
			}
			return is;
		default:
			t += (char)c;
		}
	}
}
bool sow_web_jsx::strings_equal(
	const std::string& s1,
	const std::string& s2,
	size_t n
) {
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	bool good = (n <= s1.length() && n <= s2.length());
	std::string::const_iterator l1 = good ? (s1.begin() + n) : s1.end();
	std::string::const_iterator l2 = good ? (s2.begin() + n) : s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return good;
}
bool sow_web_jsx::strings_equal(
	const std::string& s1,
	const std::string& s2
) {
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	std::string::const_iterator l1 = s1.end();
	std::string::const_iterator l2 = s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return (s2.size() == s1.size()) ? true : false;
}
std::string sow_web_jsx::extract_between(
	const std::string& data,
	const std::string& separator1,
	const std::string& separator2
) {
	std::string result;
	std::string::size_type start, limit;

	start = data.find(separator1, 0);
	if (std::string::npos != start) {
		start += separator1.length();
		limit = data.find(separator2, start);
		if (std::string::npos != limit)
			result = data.substr(start, limit - start);
	}

	return result;
}
int sow_web_jsx::fprintf_stdout(const char* msg){
	int rec = fprintf(stdout, "%s\n", msg);
	fflush(stdout);
	return rec;
}
int sow_web_jsx::fprintf_stderr(const char* msg){
	int rec = fprintf(stderr, "%s\n", msg);
	fflush(stderr);
	return rec;
}
#	include <fstream>
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
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
wchar_t* sow_web_jsx::ccr2ws(const char* s) {
	size_t len = strlen(s);
	std::unique_ptr<wchar_t[]> tmp(new wchar_t[len + 1]);
	return tmp.release();
}
#error Not Implemented
#else
//wchar_t* sow_web_jsx::ccr2ws(const char* s) {
//	size_t len = strlen(s);
//	wchar_t * buf = new wchar_t[len + sizeof(wchar_t)]();
//	//mbstowcs_s(len, buf, len, s, len);
//	mbsrtowcs(buf, &s, len, NULL);
//	return buf;
//}
wchar_t* sow_web_jsx::ccr2ws(const char* mbstr) {
	mbstate_t state;
	memset(&state, 0, sizeof state);
	size_t len = sizeof(wchar_t) + mbsrtowcs(NULL, &mbstr, 0, &state);
	wchar_t* buf = new wchar_t[len];
	mbsrtowcs(buf, &mbstr, len, &state);
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
	str.clear(); std::string().swap(str);
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
			TerminateProcess(explorer, 1);
			ret = 1;
		}
		_close_handle(explorer);
	}
	return ret == FALSE ? -1 : ret;
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
	//BOOL hRes = Process32First(hSnapShot, &pEntry);
	if (Process32First(hSnapShot, &pEntry)) {
		wchar_t* p_name = sow_web_jsx::ccr2ws(process_name);
		const wchar_t* cp_name = const_cast<const wchar_t*>(p_name);
		do {
			if (wcscmp (pEntry.szExeFile, cp_name) == 0) {
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
int sow_web_jsx::create_process(const process_info*pi) {
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
long sow_web_jsx::create_child_process(const char*process_path, const char*arg) {
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
#endif
const char* sow_web_jsx::get_error_desc(int error_code) {
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
	FILE* stream;
	errno_t err = fopen_s(&stream, absolute_path, "rb");
	if (err != 0)return new char[8]{ "INVALID" };
	fseek(stream, 0, SEEK_END);//Go to end of stream
	size_t size = ftell(stream);
	rewind(stream);//Back to begain of stream
	char* chars = new char[size + 1];
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
}

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "util.h"
//9:11 PM 11/18/2018
#if !(defined(_WIN32) || defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
bool is_user_interactive() {
	return false;
};
void print_info() {
	std::cout << "*******************************************************" << std::endl;
};
#else
BOOL is_user_interactive() {
	BOOL bIsUserInteractive = TRUE;
	HWINSTA hWinStation = GetProcessWindowStation();
	if (hWinStation != NULL) {
		USEROBJECTFLAGS uof = { 0 };
		if (GetUserObjectInformation(hWinStation, UOI_FLAGS, &uof, sizeof(USEROBJECTFLAGS), NULL) && ((uof.dwFlags & WSF_VISIBLE) == 0)) {
			bIsUserInteractive = FALSE;
		}
	}
	return bIsUserInteractive;
};
WORD get_current_console_color(HANDLE hConsole) {
	CONSOLE_SCREEN_BUFFER_INFO *ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
	GetConsoleScreenBufferInfo(hConsole, ConsoleInfo);
	WORD OriginalColors = ConsoleInfo->wAttributes;
	return OriginalColors;
};
void print_info() {
	/*
	1_____Blue				A______Light Green
	2_____Green				B______Light Aqua
	3_____Aqua				C______Light Red
	4_____Red				D______Light Purple
	5_____Purple			E______Light Yellow
	6_____Yellow			F______Bright White
	7_____White
	8_____Gray
	9_____Light Blue
	0_____Black
	*/
	//9:29 AM 11/23/2018
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD orginal_color = get_current_console_color(hConsole);
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "*******************************************************" << std::endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "Web ";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "Extension Handler ";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "*.jsx & *.jsxh;" << "\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_DARK_YELLOW);
	std::cout << "Version: 1.0.0.1" << "\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	std::cout << "Repo:https://github.com/RKTUXYN/web_jsx_cgi" << "\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "Create nested template in your web application.\r\n";
	std::cout << "Run javascript into CLI or Web Back-end\r\n";
	std::cout << "Supported Web Server IIS, Apache & Nginx \r\n";
	std::cout << "Extension-> .jsx (Mixed handler (javascript & html)) \r\n";
	std::cout << "Extension-> .jsxh (Raw javascript handler)\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "*******************************************************";
	fflush(stdout);
	SetConsoleTextAttribute(hConsole, orginal_color);
};
#endif//!_WIN32||_WIN64
void replace_back_slash(std::string&str) {
	std::size_t found = str.find_first_of("\\");
	while (found != std::string::npos) {
		str[found] = '/';
		found = str.find_first_of("\\", found + 1);
	}
};
char *get_env(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	return env_val;
};
const char *get_env_c(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	if (((env_val != NULL) && (env_val[0] == '\0')) || env_val == NULL) {
		return "";
	}
	return const_cast<const char*>(env_val);
};
int get_env_path(std::string&path_str) {
	std::istringstream tokenStream(get_env_c("path"));
	int rec = -1;
	while (std::getline(tokenStream, path_str, ';')) {
		if (path_str.find("web_jsx") != std::string::npos) {
			rec = 1;
			break;
		}
	}
	std::istringstream().swap(tokenStream);
	return rec;
};
const char* get_app_path() {
#if defined(GetModuleFileName)
	wchar_t* buf = (wchar_t*)malloc(_MAX_PATH + 1);
	GetModuleFileName(NULL, buf, _MAX_PATH);
	const char* result = reinterpret_cast<const char*>(buf);
	return result;
#else
#error !TODO
#endif
};
const char* get_current_working_dir(void) {
	static char cwd[FILENAME_MAX];
	if (get_current_dir(cwd, FILENAME_MAX) == NULL)
		*cwd = '\x0';
	return cwd;
};
void get_app_path(std::string&path) {
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(path) < 0) {
		throw std::runtime_error("Please add web_jsx bin path into environment variable Path!!!");
	}
#else
	path = get_env_c("web_jsx_cgi");
#endif
};
void print_envp(char*envp[]) {
	int count = 0;
	std::cout << "<PRE>\n";
	for (++envp; *envp; ++envp) {
		std::cout << *envp << "\n";
		count++;
	}
	std::cout << "Total ENVP##" << count << "\n";
	std::cout << "</PRE>\n";
};
int print_env_var(char* val, const char* env) {
	if (val != nullptr) {
		std::cout << env << "==>" << val << "<br/>";
		return 1;
	}
	std::cout << env << "==>Not Found!!!" << "<br/>";
	return -1;
};
void obj_insert(std::map<std::string, std::string>&from_obj, const char* prop, std::map<std::string, std::map<std::string, std::string>>&to_obj) {
	to_obj[prop] = from_obj;
};
web_extension get_request_extension (const std::string& path_str) {
	if (path_str.size() > MAX_PATH)
		return web_extension::RAW_SCRIPT;
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos)
		return web_extension::RAW_SCRIPT;
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jsx")
		return web_extension::JSX;
	if (str_extension == "jsxh")
		return web_extension::JSXH;
	return web_extension::UNKNOWN;
};
void request_file_info (const std::string& path_str, std::string&dir, std::string&file_name) {
	size_t found;
	found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
	file_name = path_str.substr(found + 1);
};
void server_physical_path(const std::string& path_str, const std::string& path_info, std::string &root_dir) {
	std::string p_i = std::regex_replace(std::string(path_info), std::regex("(?:/)"), "\\");
	root_dir = path_str.substr(0, path_str.find(p_i));
};
req_method determine_req_method(const char* request_method) {
	if (request_method == NULL || request_method == '\0')
		return req_method::UNSUPPORTED;
	if (strcmp(request_method, "GET") == 0)
		return req_method::GET;
	if (strcmp(request_method, "POST") == 0)
		return req_method::POST;
	return req_method::UNSUPPORTED;
};
req_method determine_req_method(void) {
	const char* request_method = get_env_c("REQUEST_METHOD");
	return determine_req_method(request_method);
};

const char* get_content_type(void) {
	const char* CONTENT_TYPE = get_env_c("CONTENT_TYPE");
	if ((CONTENT_TYPE != NULL) && (CONTENT_TYPE[0] == '\0')) {
		return "text/html";
	}
	return CONTENT_TYPE;
};
void write_header(const char* ct) {
	std::cout << "Content-Type:" << ct << "\n";
	std::cout << "Accept-Ranges:bytes\n";
	std::cout << "X-Powered-By:safeonline.world\n";
	std::cout << "X-Process-By:web_jsx_cgi\n";
};
void read_query_string(std::map<std::string, std::string>&data, const char*query_string) {
	if (query_string == NULL)return;
	std::string* query = new std::string(query_string);
	if (query->empty())return;
	std::regex* pattern=new std::regex("([\\w+%]+)=([^&]*)");
	auto words_begin = std::sregex_iterator(query->begin(), query->end(), *pattern);
	auto words_end = std::sregex_iterator();
	for (auto i = words_begin; i != words_end; i++) {
		std::string* key = new std::string((*i)[1].str());
		std::string* value = new std::string((*i)[2].str());
		data["\"" + *key + "\""] = "\"" + (*value) + "\"";
	}
	free(query);
	free(pattern);
	return;
};
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:11 PM 11/18/2018
#	include "core/web_jsx_global.h"
#	include "web_jsx_app_global.h"
#	include "util.h"
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#	include <unistd.h>
#	define get_current_dir getcwd
bool is_user_interactive();
void print_info();
#else
#	include <direct.h>
#define get_current_dir _getcwd
#if !defined(_WINCON_)
#	include <Wincon.h>
#endif//_WINCON_
#define FOREGROUND_BLACK			0x0000 // text color contains black.
#define FOREGROUND_YELLOW			0x0006 // text color contains Yellow.
#define FOREGROUND_DARK_YELLOW		0x0007 // text color contains DarkYellow.
#define FOREGROUND_LIGHT_GREEN		0XA // text color contains LightGreen.
#define FOREGROUND_LIGHT_RED		0XC // text color contains LightGreen.
#endif//!_WIN32
#if !(defined(_WIN32) || defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
bool is_user_interactive() {
	return false;
}
void print_info() {
	std::cout << "*******************************************************" << std::endl;
	std::cout << "Web Jsx: Server-side Javascript run with IIS, Apache & Nginx\r\n";
	std::cout << "Supproted Web extension\r\n";
	std::cout << "*.jsx, *.jsxh, *.wjsx, *.wjsxh\r\n";
	std::cout << "Version: 1.0.0.1\r\n";
	std::cout << "Embedded powerful Javascript Engine V8\r\n";
	std::cout << "Integrated powerful nested template engine.\r\n";
	std::cout << "Embedded powerful database engine e.g. PostgresSQL, MySQL & MSSQL.\r\n";
	std::cout << "Run javascript both CLI, Server-side\r\n";
	std::cout << "Supported Web Server IIS, Apache & Nginx\r\n";
	std::cout << "Extension-> .jsx|.wjsx (Mixed handler (javascript & html))\r\n";
	std::cout << "Extension-> .jsxh|.wjsxh|.js (Raw javascript handler)\r\n";
	std::cout << "git:https://github.com/RKTUXYN/web_jsx\r\n";
	std::cout << "Developed by Rajib Chy #https://github.com/rajibchy\r\n";
	std::cout << "Powered by Safe Online World (SOW)\r\n";
	std::cout << "Copyright @ https://www.safeonline.world 2018\r\n";
	std::cout << "Read more about Web Jsx https://web_jsx.safeonline.world/\r\n";
	std::cout << "*******************************************************";
}
#else
int is_user_interactive() {
	int bIsUserInteractive = TRUE;
	HWINSTA hWinStation = GetProcessWindowStation();
	if (hWinStation != NULL) {
		USEROBJECTFLAGS uof = { 0 };
		if (GetUserObjectInformation(hWinStation, UOI_FLAGS, &uof, sizeof(USEROBJECTFLAGS), NULL) && ((uof.dwFlags & WSF_VISIBLE) == 0)) {
			bIsUserInteractive = FALSE;
		}
	}
	return bIsUserInteractive;
}
WORD get_current_console_color(HANDLE hConsole) {
	CONSOLE_SCREEN_BUFFER_INFO *ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
	GetConsoleScreenBufferInfo(hConsole, ConsoleInfo);
	WORD OriginalColors = ConsoleInfo->wAttributes;
	delete ConsoleInfo;
	return OriginalColors;
}
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
	//https://go.microsoft.com/fwlink/?LinkId=746572
	//9:29 AM 11/23/2018
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD orginal_color = get_current_console_color(hConsole);
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "*******************************************************\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "Web Jsx: Server-side Javascript run with IIS, Apache & Nginx\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "Supproted Web extension:\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "*.jsx, *.jsxh, *.wjsx, *.wjsxh\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_DARK_YELLOW);
	std::cout << "Version: 1.0.0.1\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_GREEN);
	std::cout << "Embedded powerful Javascript Engine V8\r\n";
	std::cout << "Integrated powerful nested template engine.\r\n";
	std::cout << "Embedded powerful database engine e.g. PostgresSQL, MySQL & MSSQL.\r\n";
	std::cout << "Run javascript both of CLI, Server-side\r\n";
	std::cout << "Supported Web Server IIS, Apache & Nginx \r\n";
	std::cout << "Extension-> .jsx|.wjsx (Mixed handler (javascript & html)) \r\n";
	std::cout << "Extension-> .jsxh|.wjsxh|.js (Raw javascript handler)\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
	std::cout << "git:https://github.com/rktuxyn/web_jsx" << "\r\n";
	std::cout << "Developed by Rajib Chy #https://github.com/rajibchy\r\n";
	std::cout << "Powered by Safe Online World Ltd. (SOW)\r\n";
	std::cout << "Copyright @ https://www.safeonline.world 2019\r\n";
	std::cout << "Read more about Web Jsx https://web_jsx.safeonline.world/\r\n";
	SetConsoleTextAttribute(hConsole, FOREGROUND_LIGHT_RED);
	std::cout << "*******************************************************";
	fflush(stdout);
	SetConsoleTextAttribute(hConsole, orginal_color);
}
#endif//!_WIN32||_WIN64
char *string_copy(char *str) {
	size_t strLen = strlen(str);
	char *nstr = new char[strLen + 1];
	nstr[strLen + 1] = '\0';
	memcpy(nstr, str, strLen);
	return nstr;
}
void replace_back_slash(std::string&str) {
	std::size_t found = str.find_first_of("\\");
	while (found != std::string::npos) {
		str[found] = '/';
		found = str.find_first_of("\\", found + 1);
	}
}
char *get_env(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	return env_val;
}
const char *get_env_c(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	if (((env_val != NULL) && (env_val[0] == '\0')) || env_val == NULL) {
		return "";
	}
	return const_cast<const char*>(env_val);
}
int get_env_path(std::string&path_str) {
	path_str.clear();
	std::istringstream tokenStream(get_env_c("path"));
	std::string line;
	int rec = -1;
	while (std::getline(tokenStream, line, ';')) {
		if (line.find("web_jsx") != std::string::npos) {
			rec = 1;
			break;
		}
	}
	tokenStream.clear(); std::istringstream().swap(tokenStream);
	if (rec > 0) {
		line.swap(path_str);
	}
	else {
		line.clear();
	}
	
	return rec;
}
const char* get_app_path() {
#if defined(GetModuleFileName)
	//wchar_t* buf = (wchar_t*)malloc(_MAX_PATH + 1);
	wchar_t* buf = new wchar_t[_MAX_PATH + 1];
	buf[_MAX_PATH + 1] = '\0';
	GetModuleFileName(NULL, buf, _MAX_PATH);
	return reinterpret_cast<const char*>(buf);
#else
#error !TODO
#endif//!GetModuleFileName
}
const char* get_current_working_dir(void) {
	static char cwd[FILENAME_MAX];
	if (get_current_dir(cwd, FILENAME_MAX) == NULL)
		*cwd = '\x0';
	return cwd;
}
void get_app_path(std::string&path) {
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(path) < 0) {
		throw std::runtime_error("Please add web_jsx bin path into 'Path' environment variable!!!");
	}
#else
	path = get_env_c("web_jsx");
#endif//!__WEB_JSX_PUBLISH
}
void print_envp_c(char **envp) {
	int count = 0;
	std::cout << "<PRE>\n";
	for (char* c = *envp; c; c = *++envp) {
		std::cout << c << "\n";
		count++;
	}
	std::cout << "Total ENVP##" << count << "\n";
	std::cout << "</PRE>\n";
}
void print_envp(char*envp[]) {
	int count = 0;
	std::cout << "<PRE>\n";
	for (++envp; *envp; ++envp) {
		std::cout << *envp << "\n";
		count++;
	}
	std::cout << "Total ENVP##" << count << "\n";
	std::cout << "</PRE>\n";
}
int print_env_var(char* val, const char* env) {
	if (val != nullptr) {
		std::cout << env << "==>" << val << "<br/>";
		return 1;
	}
	std::cout << env << "==>Not Found!!!" << "<br/>";
	return -1;
}
void obj_insert(
	std::map<std::string, std::string>&from_obj, 
	const char* prop, 
	std::map<std::string, std::map<std::string, std::string>>&to_obj
) {
	to_obj[prop] = from_obj;
}
web_extension get_request_extension (const std::string& path_str) {
	if (path_str.size() > MAX_PATH)return web_extension::RAW_SCRIPT;
	//LOCALE_NAME_MAX_LENGTH
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return web_extension::RAW_SCRIPT;
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jsx") return web_extension::JSX;
	if (str_extension == "jsxh") return web_extension::JSXH;
	if (str_extension == "js") return web_extension::JS;
	if (str_extension == "wjsx") return web_extension::WJSX;
	if (str_extension == "wjsxh") return web_extension::WJSXH;
	return web_extension::UNKNOWN;
}
void request_file_info(const std::string& path_str, std::string& dir, std::string& file_name) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
	file_name = path_str.substr(found + 1);
}
void server_physical_path(const std::string& path_str, const std::string& path_info, std::string &root_dir) {
	std::string p_i = std::regex_replace(std::string(path_info), std::regex("(?:/)"), "\\");
	root_dir = path_str.substr(0, path_str.find(p_i));
	p_i.clear();
}
req_method determine_req_method(const char* request_method) {
	if (((request_method != NULL) && (request_method[0] == '\0')) || request_method == NULL) return req_method::UNSUPPORTED;
	if (strcmp(request_method, "GET") == 0) return req_method::GET;
	if (strcmp(request_method, "POST") == 0) return req_method::POST;
	if (strcmp(request_method, "HEAD") == 0) return req_method::HEAD;
	if (strcmp(request_method, "PUT") == 0) return req_method::PUT;
	if (strcmp(request_method, "DELETE") == 0) return req_method::DELETE;
	if (strcmp(request_method, "CONNECT") == 0) return req_method::CONNECT;
	if (strcmp(request_method, "OPTIONS") == 0) return req_method::OPTIONS;
	if (strcmp(request_method, "TRACE") == 0) return req_method::TRACE;
	return req_method::UNSUPPORTED;
}
req_method determine_req_method(void) {
	const char* request_method = get_env_c("REQUEST_METHOD");
	return determine_req_method(request_method);
}
void write_headert(const char* ct) {
	std::cout << "Content-Type:" << ct << H_N_L;
	std::cout << "Accept-Ranges:bytes" << H_N_L;
	std::cout << "X-Powered-By:safeonline.world" << H_N_L;
	std::cout << "X-Process-By:web_jsx" << H_N_L;
}
void write_header(const char* ct) {
	write_headert(ct);
	std::cout << "Status:200" << H_N_L;

}
const char* get_content_type(void) {
	const char* content_type = get_env_c("CONTENT_TYPE");
	if (((content_type != NULL) && (content_type[0] == '\0')) || content_type == NULL) {
		return "text/html";
	}
	return content_type;
}
const char* get_server_error(int error_code) {
	if (error_code == 500)return "Internal Server Error";
	if (error_code == 404)return "File Not found";
	return "Server Error";
}
void write_internal_server_error(
	const char* content_type,
	const char* ex_dir,
	int error_code,
	const char* error_msg
) {
	write_headert(content_type);
	std::cout << "Status:" << error_code << " " << get_server_error(error_code) << H_N_L;
	std::cout << "WebJsx-Error-Code:" << error_code << H_N_L;
	std::cout << "\r\n";
	std::string* str = new std::string(ex_dir);
	str->append("error\\");
	str->append(std::to_string(error_code));
	str->append(".html");
	std::string* body = new std::string();
	size_t ret = sow_web_jsx::read_file(str->c_str(), *body, true);
	if (ret != std::string::npos) {
		std::string html_body = REGEX_REPLACE_MATCH(*body, std::regex("(<%(.+?)%>)"), [&error_msg](const std::smatch& m) {
			return error_msg;
		});
		std::cout << html_body;
		std::string().swap(html_body);
	}
	else {
		std::cout << "No Error file found in /error/" << error_code << ".html<br/>";
		std::cout << "Server root:" << ex_dir << "<br/><br/><br/>";
		std::cout << error_msg;
	}
	body->clear(); delete body;
	str->clear(); delete str;
	fflush(stdout);
}

void read_query_string(std::map<std::string, std::string>&data, const char*query_string) {
	if (((query_string != NULL) && (query_string[0] == '\0')) || query_string == NULL)return;
	std::string* query = new std::string(query_string);
	if (query->empty()) {
		_free_obj(query);
		return;
	}
	std::regex* pattern = new std::regex("([\\w+%]+)=([^&]*)");
	std::sregex_iterator words_begin = std::sregex_iterator(query->begin(), query->end(), *pattern);
	std::sregex_iterator words_end = std::sregex_iterator();
	for (auto i = words_begin; i != words_end; i++) {
		data["\"" + (*i)[1].str() + "\""] = "\"" + (*i)[2].str() + "\"";
	}
	_free_obj(query); delete pattern;
	return;
}
void json_obj_stringify(std::map<std::string, std::string>& json_obj, std::string& json_str) {
	json_str += "{";
	bool is_first = true;
	for (auto itr = json_obj.begin(); itr != json_obj.end(); ++itr) {
		if (is_first) {
			is_first = false;
			json_str += itr->first;
		}
		else {
			json_str += ",";
			json_str += itr->first;
		}
		json_str += ":";
		json_str.append(itr->second);
	}
	json_str += "}";
	return;
}
#	include <sstream>
void json_array_stringify_s(std::vector<char*>& json_array_obj, std::string& json_str) {
	std::stringstream* ss = new std::stringstream();
	std::stringstream& copy = *ss;
	copy << "[";
	for (size_t i = 0, l = json_array_obj.size(); i < l; ++i) {
		if (i != 0)
			copy << ",";
		copy << "\"" << json_array_obj[i] << "\"";
	}
	copy << "]";
	json_str = ss->str();
	ss->clear(); delete ss;
	return;
}
/*std::string get_current_working_dir(void) {
	char buff[FILENAME_MAX];
	get_current_dir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
};*/
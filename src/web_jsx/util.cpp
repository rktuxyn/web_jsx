/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:11 PM 11/18/2018
#	include "util.h"
#	include "web_jsx_app_global.h"
#	include "core/wjsx_env.h"
#	include "core/std_wrapper.hpp"
#if !(defined(_WIN32) || defined(_WIN64))
#	include <unistd.h>
#	define get_current_dir getcwd
bool is_user_interactive();
void print_info();
#else
#	include <direct.h>
#	define get_current_dir _getcwd
#	include <Wincon.h>
#define FOREGROUND_BLACK			0x0000 // text color contains black.
#define FOREGROUND_YELLOW			0x0006 // text color contains Yellow.
#define FOREGROUND_DARK_YELLOW		0x0007 // text color contains DarkYellow.
#define FOREGROUND_LIGHT_GREEN		0XA // text color contains LightGreen.
#define FOREGROUND_LIGHT_RED		0XC // text color contains LightGreen.
#endif//!_WIN32

#define _DELETE DELETE
#undef DELETE

#if !(defined(_WIN32) || defined(_WIN64))
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
	if (!(err == FALSE && len > 0 && env_val != nullptr))return "";
	return const_cast<const char*>(env_val);
}
int get_env_c(const char* var_name, std::string& out_str) {
	char* env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	int rec = FALSE;
	if ( err == FALSE && len > 0 && env_val != nullptr) {
		out_str = std::string(env_val, len);
		rec = TRUE; delete[] env_val;
	}
	return rec;
}
int get_env_path(std::string&path_str) {
	path_str.clear();
	char* env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, "path");
	if (_dupenv_s(&env_val, &len, "path") != 0 || env_val == nullptr || len == 0)return -1;
	std::istringstream tokenStream(env_val);
	std::string line;
	int rec = -1;
	while (std::getline(tokenStream, line, ';')) {
		if (line.find("web_jsx") != std::string::npos) {
			rec = 1;
			break;
		}
	}
	tokenStream.clear(); std::istringstream().swap(tokenStream);
	delete[]env_val;
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
int get_current_working_dir(std::string& out) {
	char* cwd = new char[FILENAME_MAX];
	int ret = FALSE;
	if (get_current_dir(cwd, FILENAME_MAX) != NULL) {
		out.append(cwd, strlen(cwd));
		ret = TRUE;
		delete[]cwd;
	}
	return ret;
}
//const char* get_current_working_dir(void) {
//	static char cwd[FILENAME_MAX];
//	if (get_current_dir(cwd, FILENAME_MAX) == NULL)
//		*cwd = '\x0';
//	return cwd;
//}
void get_app_path(std::string&path) {
	if (get_env_path(path) < 0) {
		throw std::runtime_error("Please add web_jsx bin path into 'Path' environment variable!!!");
	}
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
void print_envp(wjsx_env&wj_env, char*envp[]) {
	int count = 0;
	std::ostream& cout = wj_env.cout();
	cout << "<PRE>\n";
	for (++envp; *envp; ++envp) {
		cout << *envp << "\n";
		count++;
	}
	cout << "Total ENVP##" << count << "\n";
	cout << "</PRE>\n";
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
	if (path_str.size() > MAX_PATH)return RAW_SCRIPT;
	//LOCALE_NAME_MAX_LENGTH
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return RAW_SCRIPT;
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jsx") return JSX;
	if (str_extension == "jsxh") return JSXH;
	if (str_extension == "js") return JS;
	if (str_extension == "wjsx") return WJSX;
	if (str_extension == "wjsxh") return WJSXH;
	return UNKNOWN;
}
int this_wjx_env(const char* name) {
	std::string wjcc;
	if (get_env_c(name, wjcc) == FALSE)return FALSE;
	int ic = std::atoi(wjcc.c_str()); wjcc.clear();
	return ic > 0 ? TRUE : FALSE;
}
int is_compiled_cached() {
	return FALSE;//this_wjx_env("WEB_JSX_COMPILED_CACHED");
}
int is_check_file_state() {
	return FALSE;//this_wjx_env("WEB_JSX_CHECK_FILE_STATE");
}
void get_dir_path(const std::string& path_str, std::string& dir) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
}
void request_file_info(
	const std::string& path_str,
	std::string& dir, std::string& file_name
) {
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
	if (request_method == NULL || strlen(request_method) == 0) return UNSUPPORTED;
	if (strcmp(request_method, "GET") == 0) return GET;
	if (strcmp(request_method, "POST") == 0) return POST;
	if (strcmp(request_method, "HEAD") == 0) return HEAD;
	if (strcmp(request_method, "PUT") == 0) return PUT;
	if (strcmp(request_method, "DELETE") == 0) return DELETE;
	if (strcmp(request_method, "CONNECT") == 0) return CONNECT;
	if (strcmp(request_method, "OPTIONS") == 0) return OPTIONS;
	if (strcmp(request_method, "TRACE") == 0) return TRACE;
	return UNSUPPORTED;
}

req_method determine_req_method(void) {
	_NEW_STR(request_method);
	req_method method = UNSUPPORTED;
	if (get_env_c("REQUEST_METHOD", *request_method) == TRUE) {
		method = determine_req_method(request_method->c_str());
	}
	_free_obj(request_method);
	return method;
}
void write_headert(
	const char* ct, wjsx_env& wj_env
) {
	wj_env << "Content-Type:" << ct << H_N_L;
	wj_env << "Accept-Ranges:bytes" << H_N_L;
	wj_env << "X-Powered-By:safeonline.world" << H_N_L;
	wj_env << "X-Process-By:web_jsx" << H_N_L;
}
void write_header(
	const char* ct,
	wjsx_env* wj_env
) {
	write_headert(ct, *wj_env);
	_WCOUT << "Status:200" << H_N_L;

}
const char* get_content_type(void) {
	const char* content_type = get_env_c("CONTENT_TYPE");
	if (content_type == NULL || strlen(content_type) == 0) {
		return "text/html";
	}
	return content_type;
}
const char* get_server_error(int error_code) {
	if (error_code == 500)return "Internal Server Error";
	if (error_code == 404)return "File Not found";
	return "Ok";
}
void write_h(const char* content_type, int error_code, wjsx_env* wj_env) {
	std::ostream& cout = wj_env->cout();
	//error_code = 200;
	write_headert(content_type, *wj_env);
	std::string error_code_str = std::to_string(error_code);
	//Status:200 OK
	cout << "Status:" << error_code_str << " " << ::get_server_error(error_code) << H_N_L;
	cout << "WebJsx-Error-Code:" << error_code_str << H_N_L;
	cout << "\r\n";
}
void write_internal_server_error(
	const char* content_type,
	const char* ex_dir,
	int error_code,
	const char* error_msg, 
	wjsx_env* wj_env
) {
	if (wj_env->is_available_out_stream() == FALSE)return;
	std::ostream& cout = wj_env->cout();
	std::string* str = new std::string(ex_dir);
	str->append("error\\");
	str->append(std::to_string(error_code));
	str->append(".html");
	std::string* body = new std::string();
	int ret = ::read_file(str->c_str(), *body);
	write_h(content_type, error_code, wj_env);
	if (ret == TRUE) {
		std::string html_body = REGEX_REPLACE_MATCH(*body, std::regex("(<%(.+?)%>)"), [&error_msg](const std::smatch& m) {
			return error_msg;
		});
		
		cout << html_body;
		std::string().swap(html_body);
	}
	else {
		cout << "No Error file found in /error/" << error_code << ".html<br/>";
		cout << "Server root:" << ex_dir << "<br/><br/><br/>";
		cout << error_msg;
	}
	_free_obj(body); _free_obj(str);
	cout.flush();
}

void read_query_string(std::map<std::string, std::string>&data, const std::string& query) {
	std::regex* pattern = new std::regex("([\\w+%]+)=([^&]*)");
	std::sregex_iterator words_begin = std::sregex_iterator(query.begin(), query.end(), *pattern);
	std::sregex_iterator words_end = std::sregex_iterator();
	for (auto i = words_begin; i != words_end; i++) {
		data["\"" + (*i)[1].str() + "\""] = "\"" + (*i)[2].str() + "\"";
	}
	delete pattern;
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
	_free_obj(ss);
	return;
}
int get_thread_id() {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	uint64_t id = std::stoull(ss.str());
	swap_obj(ss);
	return (int)id;
}
/*std::string get_current_working_dir(void) {
	char buff[FILENAME_MAX];
	get_current_dir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
};*/
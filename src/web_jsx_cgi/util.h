/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:11 PM 11/18/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_util_h)
#define _util_h
#if !defined(_web_jsx_app_global_h)
#include "web_jsx_app_global.h"
#endif//!_web_jsx_app_global_h
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <unistd.h>
#define get_current_dir getcwd
bool is_user_interactive();
void print_info();
#else
#if !defined(_INC_DIRECT)
#include <direct.h>
#endif//!_INC_DIRECT
#define get_current_dir _getcwd
#if !defined(_WINCON_)
#include <Wincon.h>
#endif//_WINCON_
#define FOREGROUND_BLACK			0x0000 // text color contains black.
#define FOREGROUND_YELLOW			0x0006 // text color contains DarkYellow.
#define FOREGROUND_DARK_YELLOW		0x0007 // text color contains DarkYellow.
#define FOREGROUND_LIGHT_GREEN		0XA // text color contains LightGreen.
#define FOREGROUND_LIGHT_RED		0XC // text color contains LightGreen.
BOOL is_user_interactive();
WORD get_current_console_color(HANDLE hConsole);
void print_info();
#endif//!_WIN32
typedef enum {
	GET,
	POST,
	UNSUPPORTED
}req_method;
typedef enum {
	JSX = 1,
	JSXH = 2,
	RAW_SCRIPT=3,
	UNKNOWN = -1
}web_extension;
void replace_back_slash(std::string&str);
const char* get_app_path();
void get_app_path(std::string&path);
const char* get_current_working_dir(void);
char *get_env(const char* var_name);
const char *get_env_c(const char* var_name);
int get_env_path(std::string&path_str);
void print_envp(char*envp[]);
int print_env_var(char* val, const char* env);
web_extension get_request_extension (const std::string& path_str);
void request_file_info (const std::string& path_str, std::string&dir, std::string&file_name);
void server_physical_path(const std::string& path_str, const std::string& path_info, std::string &root_dir);
req_method determine_req_method(void);
req_method determine_req_method(const char* request_method);
const char* get_content_type(void);
void read_query_string(std::map<std::string, std::string>&data, const char*query_string);
void obj_insert(std::map<std::string, std::string>&from_obj, const char* prop, std::map<std::string, std::map<std::string, std::string>>&to_obj);
void write_header(const char* ct);
#endif//!_util_h
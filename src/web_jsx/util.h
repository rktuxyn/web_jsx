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
#	define _util_h
#	include <string>
#	include <map>
#	include <vector>
#	include "core/wjsx_env.h"

int is_user_interactive();
void print_info();
void replace_back_slash(std::string&str);
char *string_copy(char *str);
const char* get_app_path();
void get_app_path(std::string&path);
int get_current_working_dir(std::string&out);
char *get_env(const char* var_name);
const char *get_env_c(const char* var_name);
int get_env_c(const char* var_name, std::string& out_str);
int get_env_path(std::string&path_str);
void print_envp(wjsx_env&wj_env, char*envp[]);
void print_envp_c(char **envp);
int print_env_var(char* val, const char* env);
web_extension get_request_extension(const std::string& path_str);
int is_compiled_cached();
int is_check_file_state();
void get_dir_path(
	const std::string& path_str, 
	std::string& dir
);
void request_file_info(
	const std::string& path_str,
	std::string&dir, 
	std::string&file_name
);
void server_physical_path(
	const std::string& path_str, 
	const std::string& path_info, 
	std::string &root_dir
);
req_method determine_req_method(void);
req_method determine_req_method(const char* request_method);
const char* get_content_type(void);
void read_query_string(
	std::map<std::string, std::string>&data, 
	const std::string&query_string
);
//int write___file(const char*path, const char*buffer);
void obj_insert(
	std::map<std::string, std::string>&from_obj, 
	const char* prop, 
	std::map<std::string, std::map<std::string, std::string>>&to_obj
);
void write_header(
	const char* ct,
	wjsx_env* wj_env
);
void write_internal_server_error(
	const char* content_type,
	const char* ex_dir,
	int error_code,
	const char* error_msg, 
	wjsx_env* wj_env
);
void json_obj_stringify(
	std::map<std::string, std::string>& json_obj,
	std::string& json_str
);
void json_array_stringify_s(
	std::vector<char*>& json_array_obj, 
	std::string& json_str
);
int get_thread_id();
#endif//!_util_h
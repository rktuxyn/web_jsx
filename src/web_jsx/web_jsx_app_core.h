/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//4:02 PM 2/3/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_web_jsx_app_core_h)
#	define _web_jsx_app_core_h
#	include "util.h"
#	include "fcreqh.h"
#	include "creqh.h"
typedef struct {
	//std::string* ex_dir_c;
	std::string* ex_dir;
	std::string* ex_name;
	const char*execute_path;
} app_ex_info;
namespace web_jsx_cgi {
	namespace app_core {
		void run__js_scrip(const char* content_type, std::string&root_dir, const app_ex_info aei, req_method&method, template_result& tr, const char*env_path, char **envp);
		void prepare_response(const char* content_type, const char*path_translated, const app_ex_info aei, req_method&method, const char* path_info, const char*env_path, char **envp);
#if defined(FAST_CGI_APP)
		void run__js_scrip(const char* content_type, std::string&root_dir, const app_ex_info aei, req_method&method, template_result& tr);
		void prepare_response(const char* content_type, const char*path_translated, const app_ex_info aei, req_method&method, const char* path_info);
#endif//!FAST_CGI_APP
		void prepare_console_response(int argc, char *argv[], bool ireq);
		void free_app_info(app_ex_info*aei);
	}
}

#endif//_util_h
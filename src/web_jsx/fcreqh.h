/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:46 AM 1/28/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_fcreqh_h)
#	define _fcreqh_h
#if defined(FAST_CGI_APP)
#	include <map>
#	include <string>
#	include "util.h"
#	include "core/wjsx_env.h"
namespace sow_web_jsx {
	namespace fcgi_request {
		const char * freq_env_c(const char* var_name, char **envp);
		int freq_env_c(const char* var_name, char** envp, std::string& out_str);
		void get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, const char*app_path, const char*env_path, char **envp);
		void not_found_response(
			const char* content_type, 
			char **envp, const char* ex_dir, 
			wjsx_env* wj_env
		);
		void get_request_object(
			std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, 
			const req_method method,
			const char*content_type, char **envp
		);
	}
}
#endif//FAST_CGI_APP
#endif//_creqh_h
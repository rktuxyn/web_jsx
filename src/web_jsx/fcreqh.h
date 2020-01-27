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
#	include "web_jsx_app_global.h"
#if defined(FAST_CGI_APP)
#	include "util.h"
#if !defined(_FCGI_STDIO)
#	include <fcgi_stdio.h>
#endif//!_FCGI_STDIO
#if !defined(FCGIO_H)
#	include "fcgio.h"
#endif//!FCGIO_H
#	include "fcgi_config.h"  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
namespace web_jsx_cgi {
	namespace fcgi_request {
		const char * freq_env_c(const char* var_name, char **envp);
		void get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, const char*app_path, const char*env_path, char **envp);
		void not_found_response(const char* content_type, char **envp, const char* ex_dir);
		void get_request_object(std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, req_method&method, const char*content_type, char **envp);
	}
}
#endif//FAST_CGI_APP
#endif//_creqh_h
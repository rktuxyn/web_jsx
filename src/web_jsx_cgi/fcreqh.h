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
#define _fcreqh_h
#if !defined(_web_jsx_app_global_h)
#include "web_jsx_app_global.h"
#endif//!_web_jsx_app_global_h
#if defined(FAST_CGI_APP)
#if !defined(_util_h)
#include "util.h"
#endif//!_util_h
#if !defined(_FCGI_STDIO)
#include <fcgi_stdio.h>
#endif//!_FCGI_STDIO
#if !defined(FCGIO_H)
#include "fcgio.h"
#endif//!FCGIO_H
#include "fcgi_config.h"  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
const char * freq_env_c(const char* var_name, char **envp);
void get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, char **envp);
void not_found_response(const char* content_type, char **envp);
void get_request_object(std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, req_method&method, const char*content_type, char **envp);
#endif//FAST_CGI_APP
#endif//_creqh_h
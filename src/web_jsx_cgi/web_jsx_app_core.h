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
#define _web_jsx_app_core_h
#if !defined(_util_h)
#include "util.h"
#endif//!_util_h
#if !defined(_fcreqh_h)
#include "fcreqh.h"
#endif//_fcreqh_h
#if !defined(_creqh_h)
#include "creqh.h"
#endif//_creqh_h
#if defined(FAST_CGI_APP)
void run__js_scrip(const char* content_type, std::string&ex_dir, std::string&root_dir, req_method&method, template_result& tr, char **envp);
void prepare_response(const char* content_type, const char*path_translated, const char* execute_path, req_method&method, const char* path_info, char **envp);
#else
void run__js_scrip(const char* content_type, std::string&ex_dir, std::string&root_dir, req_method&method, template_result& tr, const char*execute_path);
void prepare_response(const char* content_type, const char*path_translated, const char* execute_path, req_method&method, const char* path_info);
#endif//FAST_CGI_APP
void prepare_console_response(int argc, char *argv[], bool ireq);
#endif//_util_h
/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
namespace fsys_web_jsx {
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
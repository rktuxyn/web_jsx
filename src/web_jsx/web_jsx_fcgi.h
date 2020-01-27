/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:10 AM 11/23/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_web_jsx_fcgi_h)
#define _web_jsx_fcgi_h
#	include "web_jsx_app_core.h"
#if defined(FAST_CGI_APP)
#	include "core/glb_r.h"
#	include <fastcgi.h>
#	include "web_jsx_cgi.h"
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
using namespace std;
using namespace sow_web_jsx::js_compiler;
namespace web_jsx_cgi {
	namespace fcgi_request {
		int request_process(const app_ex_info aei, const char*env_path, char **envp);
		int request_handler(const char*execute_path);
	}
}
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#endif//!FAST_CGI_APP
#endif//!_web_jsx_cgi_global_h
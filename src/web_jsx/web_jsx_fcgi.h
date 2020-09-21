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
#	define _web_jsx_fcgi_h
#	include "web_jsx_app_core.h"
#if defined(FAST_CGI_APP)
namespace sow_web_jsx {
	namespace fcgi_request {
		int request_handler(
			const char*execute_path, 
			const char*path, int is_spath,
			const char* web_app_root,
			int is_interactive
		);
	}
}
#endif//!FAST_CGI_APP
#endif//!_web_jsx_global_h

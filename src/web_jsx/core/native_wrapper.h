//2:36 AM 11/21/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_native_wrapper_h)
#	define _native_wrapper_h
#	include <v8.h>
#	include <sstream>
#	include <map>
#	include "wjsx_env.h"
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

namespace sow_web_jsx {
	namespace wrapper {
		void response_body_flush(
			wjsx_env&wj_env, bool end_req
		);
		EXPORT_WJSX void clear_cache(wjsx_env& wj_env);
		/*EXPORT_WJSX const char* get_root_dir();
		EXPORT_WJSX const char* get_app_dir();*/
		//EXPORT_WJSX int is_cli();
		EXPORT_WJSX void add_header(wjsx_env* wj_env, const char*key, const char*value);
		EXPORT_WJSX int is_http_status_ok(wjsx_env* wj_env);
		EXPORT_WJSX int is_gzip_encoding(wjsx_env* wj_env);
		EXPORT_WJSX int flush_http_status(wjsx_env* wj_env);
		//EXPORT_WJSX int set_binary_output();
		//EXPORT_WJSX int set_binary_mode_in();
		EXPORT_WJSX void flush_header(wjsx_env* wj_env);
		EXPORT_WJSX void flush_cookies(wjsx_env* wj_env);
		v8::Local<v8::ObjectTemplate> get_context(
			v8::Isolate * isolate, 
			const std::map<std::string, std::map<std::string, std::string>> ctx
		);
		v8::Local<v8::ObjectTemplate> get_console_context(
			v8::Isolate * isolate, 
			const std::map<std::string, std::string> ctx
		);
		//2:02 PM 1/7/2020
	}
};
#endif//!_native_wrapper_h
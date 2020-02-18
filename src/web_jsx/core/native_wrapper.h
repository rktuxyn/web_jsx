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
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)

#if defined(__WJSX_SHARED)
#if !defined(_export_wjsx)
#	define _export_wjsx __declspec(dllexport)
#endif//!jsx_export
#else
#if !defined(_export_wjsx)
#	define _export_wjsx
#endif//!_export_wjsx
#endif//__WJSX_SHARED

namespace sow_web_jsx {
	namespace wrapper {
		void response_body_flush(bool end_req);
		void clear_cache();
		_export_wjsx void clear_cache(int clean_body, int clean_root);
		_export_wjsx const char* get_root_dir();
		_export_wjsx const char* get_app_dir();
		_export_wjsx int is_cli();
		_export_wjsx int is_flush();
		_export_wjsx int set_flush_status(int flush);
		_export_wjsx void add_header(const char*key, const char*value);
		_export_wjsx std::stringstream& get_body_stream();
		_export_wjsx int is_http_status_ok();
		_export_wjsx int is_gzip_encoding();
		_export_wjsx int flush_http_status();
		_export_wjsx int set_binary_output();
		_export_wjsx int set_binary_mode_in();
		_export_wjsx void flush_header();
		_export_wjsx void flush_cookies();
		v8::Local<v8::ObjectTemplate> get_context(v8::Isolate * isolate, std::map<std::string, std::map<std::string, std::string>>& ctx);
		v8::Local<v8::ObjectTemplate> get_console_context(v8::Isolate * isolate, std::map<std::string, std::string>&ctx);
		v8::Local<v8::ObjectTemplate> create_v8_context_object(v8::Isolate* isolate);
		//2:02 PM 1/7/2020
	}
};
#endif//!_native_wrapper_h
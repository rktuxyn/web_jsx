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
#define _native_wrapper_h
#if !defined(_v8_util_h)
#include "v8_util.h"
#endif//!v8_util_h
#if defined(FAST_CGI_APP)
#if !defined(_FCGI_STDIO)
#include <fcgi_stdio.h>
#endif//!_FCGI_STDIO
#if !defined(FCGIO_H)
#include "fcgio.h"
#endif//!FCGIO_H
#else
#if !defined(DATA_READ_CHUNK)
#define DATA_READ_CHUNK 16384
#endif//!DATA_READ_CHUNK
#endif//!FAST_CGI_APP
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
namespace sow_web_jsx {
	namespace wrapper {
		void response_body_flush(bool end_req);
		void clear_cache();
		const char* get_root_dir();
		jsx_export v8::Local<v8::ObjectTemplate> get_context(v8::Isolate * isolate, std::map<std::string, std::map<std::string, std::string>>& ctx);
		jsx_export v8::Local<v8::ObjectTemplate> get_console_context(v8::Isolate * isolate, std::map<std::string, std::string>&ctx);
		jsx_export v8::Local<v8::ObjectTemplate> create_v8_context_object(v8::Isolate* isolate);
		//2:02 PM 1/7/2020
	}
};
#endif//!_native_wrapper_h
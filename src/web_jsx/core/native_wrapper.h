//2:36 AM 11/21/2018
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

namespace fsys_web_jsx {
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
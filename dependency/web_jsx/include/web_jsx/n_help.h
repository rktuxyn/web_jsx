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
#if !defined(_n_help_h)
#	define _n_help_h
#	include <vector>
#	include <map>
#	include <v8.h>
#	include <string>

#if !defined(WJSX_API)
#	define WJSX_API(type) type
#endif//!WJSX_API

namespace fsys_web_jsx {
	enum response_status {
		OK = 200,
		MOVED = 301,
		REDIRECT = 302,
		SEEOTHER = 303,
		NOTMODIFIED = 304,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501
	};
	WJSX_API(void) string_to_hex(const std::string& input, std::string& output);
	WJSX_API(void) hex_to_string(const std::string& input, std::string& output);
	class n_help {
	public:
		WJSX_API(static void) add_http_status(std::vector<std::string>&http_status, std::string&values);
		WJSX_API(static int) write_http_status(std::vector<std::string>&http_status, bool check_status);
		WJSX_API(static response_status) get_http_response_status(std::vector<std::string>& http_status);
		WJSX_API(static response_status) get_http_response_status(int status_code);
		WJSX_API(static void) error_response(
			const char* server_root,
			response_status status_code,
			const char* error_msg,
			wjsx_env&wj_env
		);
		//https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
		WJSX_API(static void) add_header(std::map<std::string, std::string>&header, const char* key, const char* values);
		WJSX_API(static void) remove_header(std::map<std::string, std::string>&header, const char* key);
		WJSX_API(static int) is_gzip_encoding(std::map<std::string, std::string>&header);
		WJSX_API(static int) is_binary_response(std::map<std::string, std::string>& header);
		WJSX_API(static int) is_attachment_response(std::map<std::string, std::string>&header);
		WJSX_API(static void) write_header(wjsx_env&wj_env);
		WJSX_API(static void) write_cookies(wjsx_env&wj_env);
		WJSX_API(static void) v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>&out_put);
	};
}
#endif//!_n_help_h
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_n_help_h)
#define _n_help_h
#if !defined(_v8_util_h)
#include "v8_util.h"
#endif//_v8_util_h
namespace sow_web_jsx {
	class n_help {
	public:
		static void add_http_status(std::vector<std::string>&http_status, std::string&values);
		static int write_http_status(std::vector<std::string>&http_status, bool check_status);
		//https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
		static void add_header(std::map<std::string, std::string>&header, const std::string&key, const std::string&values);
		static void remove_header(std::map<std::string, std::string>&header, const std::string&key);
		static int is_gzip_encoding(std::map<std::string, std::string>&header);
		static int is_binary_response(std::map<std::string, std::string>&header);
		static int is_attachment_response(std::map<std::string, std::string>&header);
		static void write_header(std::map<std::string, std::string>&header);
		static void write_cookies(std::vector<std::string>&cookies);
		static void v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>&out_put);
	};
}
#endif//!_n_help_h
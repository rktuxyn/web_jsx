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
#if !defined(_npgsql_wrapper_h)
#include "npgsql_wrapper.h"
#endif//_npgsql_wrapper_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
#if !defined(_http_request_h)
#include "http_request.h"
#endif//_http_request_h
#if !defined(_smtp_client_h)
#include "smtp_client.h"
#endif//_smtp_client_h
#if !defined(_pdf_generator_h)
#include "pdf_generator.h"
#endif//_npgsql_tools_h
#if !defined(directory__h)
#include "directory_.h"
#endif//!directory__h
#if !defined(_zgzip_h)
#include "zgzip.hpp"
#endif//!_zgzip_h
#if defined(__client_build)
#if !defined(_encryption_h)
#include "encryption.h"
#endif//!_encryption_h
#endif//__client_build
#if defined(FAST_CGI_APP)
#if !defined(DATA_READ_CHUNK)
#define DATA_READ_CHUNK 8192
#endif//!DATA_READ_CHUNK
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
#if !defined(_n_help_h)
#include "n_help.h"
#endif//_n_help_h
#if !defined(_jsx_file_h)
#include "jsx_file.h"
#endif//_n_help_h
#if !defined(_uws_app_h)
#include "uws/uws_app.h"
#endif//!_uws_app_h
#if !defined(_mysql_wrapper_h)
#include "mysql_wrapper.h"
#endif//!_mysql_wrapper_h
#if !defined(_http_payload_h)
#include "http_payload.h"
#endif//!_http_payload_h
//#if !defined(_my_sql_h)
//#include <my_sql.h>
//#endif//!_my_sql_h

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
	}
};
#endif//!_native_wrapper_h
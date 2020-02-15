/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:49 PM 1/14/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_web_jsx_h)
#	define _web_jsx_h
#	include <v8.h>
#	include <io.h> 
#	include <regex>
#	include <sstream>

#if !defined(FAST_CGI_APP)
#	define FAST_CGI_APP 1
#endif//!FAST_CGI_APP

#if !defined(NULL)
    #if defined(__cplusplus)
        #	define NULL 0
    #else
        #	define NULL ((void *)0)
    #endif//!__cplusplus
#endif//!NULL

#if !defined(_export_wjsx)
#if (defined(_WIN32)||defined(_WIN64))
#	define _export_wjsx __declspec(dllexport)
#else
#	define _export_wjsx
#endif//_WIN32||_WIN64
#endif//!_export_wjsx

#if !defined(__file_exists)
#define __file_exists(fname)\
_access(fname, 0)!=-1
#endif//!__file_exists

#if !defined(TYPE_CHECK)
#define TYPE_CHECK(T, S)                                       \
  while (false) {                                              \
    *(static_cast<T* volatile*>(0)) = static_cast<S*>(0);      \
  }
#endif//!TYPE_CHECK

#if !defined(v8_str)
#define v8_str(isolate, x)\
	v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked()
#endif//!v8_str

#if !defined(throw_js_error)
#define throw_js_error(isolate, err)\
	isolate->ThrowException(v8::Exception::Error(v8_str(isolate, err)))
#endif//!throw_js_error

#if !defined(wjsx_set_method)
#define wjsx_set_method(isolate, target, name, func)\
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, name), v8::Function::New(isolate, func) )
#endif//!register_wjsx_module

#if !defined(wjsx_set_object)
#define wjsx_set_object(isolate, target, name, obj)\
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, name), obj )
#endif//!register_wjsx_module

#if !defined(to_char_str_n)
#define to_char_str_n(value)\
	value.length() <= 0 ? "" :(*value ? *value : "<string conversion failed>")
#endif//!to_char_str_n

#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj

#if !defined(_free_char)
#	define _free_char(obj)\
while(obj){\
	delete[] obj; obj = NULL;\
}
#endif//!_free_char

#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE

__forceinline static void format__path(std::string& str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
}
__forceinline void get_dir_from_path (const std::string& path_str, std::string&dir) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
}
__forceinline static const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
	v8::String::Utf8Value str(isolate, x);
	return to_char_str_n(str);
}

template<class _input>
inline int is_error_code(_input ret) {
	return (ret == FALSE || ret == std::string::npos || ret < 0) ? TRUE : FALSE;
}
extern "C" {
	_export_wjsx void web_jsx_native_module(v8::Handle<v8::Object>target);
}
/*[function pointers]*/
typedef void (*add_resource_func)();
/*[/function pointers]*/
namespace sow_web_jsx {
	_export_wjsx void register_resource(add_resource_func func);
	namespace wrapper {
		_export_wjsx void clear_cache(int clean_body, int clean_root);
		_export_wjsx int is_cli();
		_export_wjsx int is_flush();
		_export_wjsx int set_flush_status(int flush);
		_export_wjsx std::stringstream& get_body_stream();
		_export_wjsx const char* get_root_dir();
		_export_wjsx const char* get_app_dir();
		_export_wjsx void add_header(const char*key, const char*value);
		_export_wjsx int is_http_status_ok();
		_export_wjsx int is_gzip_encoding();
		_export_wjsx int flush_http_status();
		_export_wjsx void flush_header();
		_export_wjsx void flush_cookies();
		_export_wjsx int set_binary_output();
		_export_wjsx int set_binary_mode_in();
	}
}

#endif//!_web_jsx_h
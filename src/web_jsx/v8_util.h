/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_v8_util_h)
#define _v8_util_h
#if !defined(INCLUDE_V8_H_)
#include <v8.h>
#endif // !INCLUDE_V8_H_
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif //!_web_jsx_global_h
#if !defined(_t_async_h)
#include "t_async.h"
#endif//!_t_async_h
#include <libplatform/v8-tracing.h>
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
namespace sow_web_jsx {
	typedef struct {
		int is_flush;
		std::stringstream body_stream;
		std::map<std::string, std::string>* headers;
		std::vector<std::string>* cookies;
		std::vector<std::string>* http_status;
		std::string*root_dir;
	}internal_global_ctx;
	const char* _toCharStr(const v8::String::Utf8Value& value);
#define T_CHAR _toCharStr
	const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x);
	v8::Local<v8::String> v8_str(v8::Isolate* isolate, const char* x);
	void set__exception(v8::Isolate * isolate, v8::TryCatch*try_catch, template_result&tr);
	void get_server_map_path(const char* req_path, std::string&output);
	v8::Local<v8::String> concat_msg(v8::Isolate* isolate, const char* a, const char*b);
	v8::Handle<v8::Object> native_write_filei(v8::Isolate* isolate, const std::string abs_path, const char* buffer);
	/*
	* Retrieve the c++ object pointer from the js object
	*	This is where we take the actual c++ object that was embedded
	*	into the javascript object and get it back to a useable c++ object
	*	Build ==>3:29 AM 1/29/2019
	*/
	template <typename T>
	T* unwrap_ctx(v8::Isolate* isolate, int index = 0) {
		v8::Local<v8::Context>ctx = isolate->GetEnteredContext();
		v8::Local<v8::Object> self = ctx->Global();
		v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(index));
		return  reinterpret_cast<T*>(wrap->Value());
	};
	template <typename T>
	void wrap_ctx(v8::Isolate* isolate, v8::Local<v8::Context> ctx, T* internal_ctx, int index = 0) {
		v8::Local<v8::Object> global = ctx->Global();
		global->SetInternalField(index, v8::External::New(isolate, internal_ctx));
		return;
	};
	v8::Local<v8::Context> create_internal_context(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> v8_object);
	//
};
#endif //!_v8_util_h

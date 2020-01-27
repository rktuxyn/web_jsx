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
#	define _v8_util_h
#	include <v8.h>
#	include "web_jsx_global.h"
#	include "t_async.h"
#	include <libplatform/v8-tracing.h>
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
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

#if (defined(_WIN32)||defined(_WIN64))
#	define _export_util __declspec(dllexport)
#else
#	define _export_util
#endif//_WIN32||_WIN64

namespace sow_web_jsx {
	typedef struct {
		int is_flush;
		std::stringstream body_stream;
		std::map<std::string, std::string>* headers;
		std::vector<std::string>* cookies;
		std::vector<std::string>* http_status;
		std::string*root_dir;
	}internal_global_ctx;
	_export_util bool to_boolean(v8::Isolate* isolate, v8::Local<v8::Value> value);
	_export_util void garbage_collect(v8::Isolate* isolat);
	const char* _toCharStr(const v8::String::Utf8Value& value);
#define T_CHAR _toCharStr
	const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x);
	void read_line(const v8::FunctionCallbackInfo<v8::Value>& args);
	//v8::Local<v8::String> v8_str(v8::Isolate* isolate, const char* x);
	void set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		template_result&tr
	);
	_export_util void set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		std::string& error_str
	);
	_export_util void get_server_map_path(const char* req_path, std::string&output);
	_export_util const char* get_prop_value(v8::Isolate* isolate, v8::Local<v8::Context>ctx, v8::Local<v8::Object>v8_obj, const char* prop);
	_export_util int v8_object_get_number(v8::Isolate* isolate, v8::Local<v8::Context>ctx, v8::Local<v8::Object>obj, const char* prop);
	_export_util v8::Local<v8::String> concat_msg(v8::Isolate* isolate, const char* a, const char*b);
	v8::Handle<v8::Object> native_write_filei(v8::Isolate* isolate, const std::string abs_path, const char* buffer);
	class _export_util native_string {
	private:
		char* _data;
		size_t _length;
		char _utf8ValueMemory[sizeof(v8::String::Utf8Value)];
		v8::String::Utf8Value* _utf8Value = nullptr;
		bool _invalid = false;
	public:
		native_string(v8::Isolate* isolate, const v8::Local<v8::Value>& value);
		bool is_invalid(v8::Isolate* isolate);
		std::string_view get_string();
		const char* c_str();
		bool is_empty();
		size_t size();
		/**
		 * Sets the handle to be empty. is_empty() will then return true.
		 */
		void clear();
		~native_string();
	};
	template <typename T>
	T* unwrap(const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Local<v8::Object> self = args.Holder();
		v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
		return static_cast<T*>(wrap->Value());
	}
	template <typename T, typename... Args>
	void make_object(v8::Isolate* isolate, v8::Handle<v8::Object> object, Args&&... args) {
		T* xx = new T(std::forward<Args>(args)...);
		object->SetInternalField(0, v8::External::New(isolate, xx));
		return;
	}
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

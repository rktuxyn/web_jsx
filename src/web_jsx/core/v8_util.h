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
//#pragma comment(lib, "v8_init.lib")
//#pragma comment(lib, "v8_initializers.lib")
//#pragma comment(lib, "v8_libbase.lib")
//#pragma comment(lib, "v8_libplatform.lib")
//#pragma comment(lib, "v8_libsampler.lib")
//#pragma comment(lib, "v8_snapshot.lib")
//#pragma comment(lib, "icui18n.lib")
//#pragma comment(lib, "icuuc.lib")
namespace sow_web_jsx {
	typedef struct {
		int is_flush;
		std::stringstream body_stream;
		std::map<std::string, std::string>* headers;
		std::vector<std::string>* cookies;
		std::vector<std::string>* http_status;
		std::string*root_dir;
	}internal_global_ctx;
	bool to_boolean(v8::Isolate* isolate, v8::Local<v8::Value> value);
	void garbage_collect(v8::Isolate* isolat);
	const char* _toCharStr(const v8::String::Utf8Value& value);
#define T_CHAR _toCharStr
	const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x);
	void read_line(const v8::FunctionCallbackInfo<v8::Value>& args);
	v8::Local<v8::String> v8_str(v8::Isolate* isolate, const char* x);
	void set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		template_result&tr
	);
	void set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		std::string& error_str
	);
	void get_server_map_path(const char* req_path, std::string&output);
	const char* get_prop_value(v8::Isolate* isolate, v8::Local<v8::Context>ctx, v8::Local<v8::Object>v8_obj, const char* prop);

	v8::Local<v8::String> concat_msg(v8::Isolate* isolate, const char* a, const char*b);
	v8::Handle<v8::Object> native_write_filei(v8::Isolate* isolate, const std::string abs_path, const char* buffer);
	class native_string {
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
		/*v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, object);
		pobj.SetWeak<T*>(&xx, [](const v8::WeakCallbackInfo<T*> &data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);*/
		//

		/*v8::Persistent<v8::Object> obj;
		obj.Reset(isolate, object);
		obj.SetWeak<T>(xx, [](const v8::WeakCallbackInfo<void*> &data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);*/
		//auto obj = v8::Persistent<v8::Object>::New(isolate, object);
		/*object.MakeWeak(xx, [](v8::Persistent<v8::Value> objs, void* data) {
			auto x = static_cast<T*>(data);
			delete x;
			//objs.Dispose();
			//objs.Clear();
		});*/
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

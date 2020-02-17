/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:13 AM 1/17/2020
#if !defined(_v8_util_h)
#	define _v8_util_h
#	include <v8.h>
#	include <string>
#if !defined(_export_wjsx)
#if (defined(_WIN32)||defined(_WIN64))
#	define _export_wjsx __declspec(dllexport)
#else
#	define _export_wjsx
#endif//_WIN32||_WIN64
#endif//!_export_wjsx
namespace sow_web_jsx {
	_export_wjsx bool to_boolean(v8::Isolate* isolate, v8::Local<v8::Value> value);
	_export_wjsx void garbage_collect(v8::Isolate* isolat);
	_export_wjsx void set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		std::string& error_str
	);
	_export_wjsx void get_server_map_path(const char* req_path, std::string& output);
	_export_wjsx int get_prop_value(
		v8::Isolate* isolate, v8::Local<v8::Context> ctx, 
		v8::Local<v8::Object> v8_obj, const char* prop, std::string&out
	);
	_export_wjsx v8::Local<v8::String> concat_msg(v8::Isolate* isolate, const char* a, const char*b);
	class _export_wjsx native_string {
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
}
#endif // !_v8_util_h
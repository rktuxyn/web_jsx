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
//1:13 AM 1/17/2020
#if !defined(_v8_util_h)
#	define _v8_util_h
#	include "web_jsx.h"
#	include <v8.h>
#	include <string>

#if !defined(WJSX_API)
#	define WJSX_API(type) type
#endif//!WJSX_API

namespace fsys_web_jsx {
	WJSX_API(bool) to_boolean(v8::Isolate* isolate, v8::Local<v8::Value> value);
	WJSX_API(void) garbage_collect(v8::Isolate* isolat);
	WJSX_API(void) set__exception(
		v8::Isolate * isolate, 
		v8::TryCatch*try_catch, 
		std::string& error_str
	);
	WJSX_API(void) get_server_map_path(const char* req_path, std::string& output);
	WJSX_API(int) get_prop_value(
		v8::Isolate* isolate, v8::Local<v8::Context> ctx, 
		v8::Local<v8::Object> v8_obj, const char* prop, std::string&out
	);
	WJSX_API(v8::Local<v8::String>) concat_msg(v8::Isolate* isolate, const char* a, const char*b);
	WJSX_API(int) v8_object_get_number(v8::Isolate* isolate, v8::Local<v8::Context>ctx, v8::Local<v8::Object>obj, const char* prop);
	
	class native_string {
	private:
		char* _data;
		size_t _length;
		v8::String::Utf8Value* _utf8Value = nullptr;
		bool _invalid = false;
	public:
		native_string(v8::Isolate* isolate, const v8::Local<v8::Value>& value);
		bool is_invalid(v8::Isolate* isolate);
		std::string_view get_string();
		const char* c_str();
		bool is_empty();
		size_t size();
		/**Sets the handle to be empty. is_empty() will then return true.*/
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
	template <typename T>
	T* unwrap_isolate_data(v8::Isolate* isolate, int index = 0, int raise_exception = 1) {
		void* data = isolate->GetData((isolate->GetNumberOfDataSlots() + index) - 1);
		if (data == NULL) {
			if (raise_exception == 1)
				throw_js_error(isolate, "Unable to load native environment...");
			return NULL;
		}
		return reinterpret_cast<T*>(data);
	};
	template <typename T>
	void wrap_isolate_data(v8::Isolate* isolate, T* internal_ctx, int index = 0) {
		isolate->SetData((isolate->GetNumberOfDataSlots() + index) - 1, internal_ctx);
		return;
	};
}
#endif // !_v8_util_h
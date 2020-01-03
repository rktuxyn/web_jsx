/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:24 PM 1/22/2019
#include "v8_util.h"
#if !defined(_CSTRING_)
#include <cstring>
#endif//!_CSTRING_
namespace sow_web_jsx {
	bool sow_web_jsx::to_boolean(v8::Isolate* isolate, v8::Local<v8::Value> value) {
#if V8_MAJOR_VERSION < 7 || (V8_MAJOR_VERSION == 7 && V8_MINOR_VERSION == 0)
		/* Old */
		return value->BooleanValue(isolate->GetCurrentContext()).ToChecked();
#else
		return value->BooleanValue(isolate);
#endif
	}
	void sow_web_jsx::garbage_collect(v8::Isolate* isolate) {
		//isolate->RequestGarbageCollectionForTesting(v8::Isolate::GarbageCollectionType::kFullGarbageCollection);
		isolate->LowMemoryNotification();
		//isolate->IdleNotificationDeadline(0);
	}
	const char* sow_web_jsx::_toCharStr(const v8::String::Utf8Value& value) {
		if (value.length() <= 0)return "";
		return *value ? *value : "<string conversion failed>";
	}
	const char* sow_web_jsx::to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
		v8::String::Utf8Value str(isolate, x);
		return _toCharStr(str);
	}
	v8::Local<v8::String> sow_web_jsx::v8_str(v8::Isolate* isolate, const char* x) {
		return v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked();
	}
	void sow_web_jsx::set__exception(v8::Isolate* isolate, v8::TryCatch* try_catch, template_result& tr) {
		tr.is_error = true;
		v8::String::Utf8Value exception(isolate, try_catch->Exception());
		tr.err_msg = _toCharStr(exception);
		v8::MaybeLocal<v8::Value> st = try_catch->StackTrace(isolate->GetCurrentContext());
		if (!st.IsEmpty()) {
			v8::String::Utf8Value stackTrace(isolate, st.ToLocalChecked());
			tr.err_msg.append("\r\nStackTrace:\r\n");
			tr.err_msg.append(_toCharStr(stackTrace));
		}
	}
	const char* sow_web_jsx::set__exception(v8::Isolate* isolate, v8::TryCatch* try_catch) {
		v8::String::Utf8Value exception(isolate, try_catch->Exception());
		std::string error_str = _toCharStr(exception);
		v8::MaybeLocal<v8::Value> st = try_catch->StackTrace(isolate->GetCurrentContext());
		if (!st.IsEmpty()) {
			v8::String::Utf8Value stackTrace(isolate, st.ToLocalChecked());
			error_str.append("\r\nStackTrace:\r\n");
			error_str.append(_toCharStr(stackTrace));
		}
		return error_str.c_str();
	}
	void sow_web_jsx::get_server_map_path(const char* req_path, std::string& output) {
		output.append(req_path);
		output = std::regex_replace(output, std::regex("(?:/)"), "\\");
	}
	const char* get_prop_value(v8::Isolate* isolate, v8::Local<v8::Context> ctx, v8::Local<v8::Object> v8_obj, const char* prop){
		v8::Local<v8::Value> v8_str = v8_obj->Get(ctx, sow_web_jsx::v8_str(isolate, prop)).ToLocalChecked();
		if (v8_str->IsNullOrUndefined()) return "null";
		v8::String::Utf8Value str(isolate, v8_str);
		std::string* val = new std::string(*str);
		return val->data();
	}
	v8::Local<v8::String> sow_web_jsx::concat_msg(v8::Isolate* isolate, const char* a, const char* b) {
		//char* msg = (char*)malloc(strlen(a) + strlen(b));
		size_t length = strlen(a) + strlen(b);
		char* msg = new char[length + sizeof(char)];
		msg[length] = '\0';
		sprintf(msg, "%s%s", a, b);
		v8::Local<v8::String> val = v8::String::NewFromUtf8(isolate, *&msg, v8::NewStringType::kNormal, (int)length).ToLocalChecked();
		//v8::Local<v8::String> val = sow_web_jsx::v8_str(isolate, const_cast<const char*>(msg));
		delete[] msg;
		return val;
	}
	v8::Handle<v8::Object> sow_web_jsx::native_write_filei(v8::Isolate* isolate, const std::string abs_path, const char* buffer) {
		FILE* fstream;
		errno_t err;
		err = fopen_s(&fstream, abs_path.c_str(), "w+");
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		if (err != 0 || fstream == NULL) {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "staus_code"),
				v8::Number::New(isolate, -1)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "message"),
				sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path.c_str())
			);
			return v8_result;
		}
		size_t len = fwrite(buffer, sizeof(char), strlen(buffer), fstream);
		if (ferror(fstream)) {
			fclose(fstream);
			fstream = NULL;
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "staus_code"),
				v8::Number::New(isolate, -1)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "message"),
				sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path.c_str())
			);
			return v8_result;
		}
		fclose(fstream);
		fstream = NULL;
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, static_cast<double>(len))
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "message"),
			sow_web_jsx::v8_str(isolate, "Success...")
		);
		return v8_result;
	}
	v8::Local<v8::Context> sow_web_jsx::create_internal_context(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> v8_object) {
		internal_global_ctx* js_ctx = new internal_global_ctx();
		js_ctx->is_flush = 0;
		js_ctx->headers = new std::map<std::string, std::string>();
		js_ctx->cookies = new std::vector<std::string>();
		js_ctx->http_status = new std::vector<std::string>();
		js_ctx->root_dir = new std::string("");
		js_ctx->body_stream = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		v8_object->SetInternalFieldCount(1);
		v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, v8_object/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
		v8::Local<v8::Object> global = ctx->Global();
		global->SetInternalField(0, v8::External::New(isolate, js_ctx));
		return ctx;
	}
	/*[native_string 9:17 PM 12/5/2019]*/
	native_string::native_string(v8::Isolate* isolate, const v8::Local<v8::Value>& value){
		if (value->IsNullOrUndefined()) {
			data = nullptr;
			length = 0;
		}
		else if (value->IsString()) {
			utf8Value = new (utf8ValueMemory) v8::String::Utf8Value(isolate, value);
			data = (**utf8Value);
			length = utf8Value->length();
		}
		else if (value->IsTypedArray()) {
			v8::Local<v8::ArrayBufferView> arrayBufferView = v8::Local<v8::ArrayBufferView>::Cast(value);
			v8::ArrayBuffer::Contents contents = arrayBufferView->Buffer()->GetContents();
			length = arrayBufferView->ByteLength();
			data = (char*)contents.Data() + arrayBufferView->ByteOffset();
		}
		else if (value->IsArrayBuffer()) {
			v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(value);
			v8::ArrayBuffer::Contents contents = arrayBuffer->GetContents();
			length = contents.ByteLength();
			data = (char*)contents.Data();
		}
		else {
			invalid = true;
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "form_data required!!!")));
		}
	}
	bool native_string::is_invalid(v8::Isolate* isolate) {
		if ( invalid ) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "form_data required!!!")));
		}
		return invalid;
	}
	std::string_view native_string::get_string(){
		return { data, length };
	}
	const char* native_string::c_str() {
		return get_string().data();
	}
	bool native_string::is_empty(){
		return length == 0;
	}
	size_t native_string::size(){
		return length;
	}
	void native_string::clear(){
		if (utf8Value && length > 0) {
			utf8Value->~Utf8Value();
			utf8Value = nullptr;
			data = nullptr; length = 0;
		}
	}
	native_string::~native_string(){
		clear();
	}
	//[/native_string]
}
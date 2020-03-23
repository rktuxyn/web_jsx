/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:24 PM 1/22/2019
#	include "v8_util.h"
#	include <cstring>
#	include "template_info.h"
#	include "std_wrapper.hpp"
#	include "wjsx_env.h"
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

//_toCharStr(str)

	v8::Handle<v8::String> read_line() {
		const int kBufferSize = 1024 + 1;
		char* buffer = new char[kBufferSize];
		char* res = std::fgets(buffer, kBufferSize, stdin);
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		if (res == NULL) {
			delete[]buffer; delete[]res;
			v8::Handle<v8::Primitive> t = v8::Undefined(isolate);
			return v8::Handle<v8::String>::Cast(t);
		}
		delete[]res;
		// Remove newline char
		for (char* pos = buffer; *pos != '\0'; pos++) {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
		}
		v8::Local<v8::String>result = v8_str(isolate, const_cast<const char*>(buffer));
		delete[]buffer; 
		return result;
	}
	// The callback that is invoked by v8 whenever the JavaScript 'read_line'
	// function is called. Reads a string from standard input and returns.
	V8_JS_METHOD(sow_web_jsx::read_line) {
		if (args.Length() > 0) {
			args.GetIsolate()->ThrowException(v8_str(args.GetIsolate(), "Unexpected arguments"));
			return;
		}
		args.GetReturnValue().Set(read_line());
	}
	void add_stack_trace(v8::Isolate* isolate, v8::MaybeLocal<v8::Value> trace, std::string&error_str) {
		if (trace.IsEmpty()) {
			error_str.append("\nNo StackTrace found.");
			return;
		}
		native_string stackTrace(isolate, trace.ToLocalChecked());
		error_str.append("\nStackTrace:\n");
		error_str.append(stackTrace.c_str());
		stackTrace.clear();
	}
	void sow_web_jsx::set__exception(
		v8::Isolate* isolate, v8::TryCatch* try_catch, 
		template_result& tr
	) {
		tr.is_error = TRUE;
		native_string exception(isolate, try_catch->Exception());
		v8::MaybeLocal<v8::Value> st = try_catch->StackTrace(isolate->GetCurrentContext());
		if (exception.is_empty()) {
			add_stack_trace(isolate, st, tr.err_msg);
			return;
		}
		tr.err_msg = std::string(exception.c_str());
		add_stack_trace(isolate, st, tr.err_msg); exception.clear();

	}
	
	void sow_web_jsx::set__exception(
		v8::Isolate* isolate, 
		v8::TryCatch* try_catch, 
		std::string&error_str
	) {
		v8::MaybeLocal<v8::Value> st = try_catch->StackTrace(isolate->GetCurrentContext());
		v8::Local<v8::Value> v8_exception = try_catch->Exception();
		if (v8_exception->IsNullOrUndefined() || v8_exception.IsEmpty()) {
			add_stack_trace(isolate, st, error_str);
			return;
		}
		//v8::String::Utf8Value exception(isolate, v8_exception);
		native_string exception(isolate, v8_exception);
		if (!exception.is_empty()) {
			error_str = std::string(exception.c_str()); exception.clear();
		}
		add_stack_trace(isolate, st, error_str);
		/*if (error_str.empty()) {
			error_str = "Unable to caught v8 error...";
		}*/
		return;
	}
	void sow_web_jsx::get_server_map_path(const char* req_path, std::string& output) {
		output.append(req_path);
		output = std::regex_replace(output, std::regex("(?:/)"), "\\");
	}
	int get_prop_value(
		v8::Isolate* isolate, v8::Local<v8::Context> ctx, 
		v8::Local<v8::Object> v8_obj, const char* prop, std::string&out
	){
		out.clear();
		v8::Local<v8::Value> v8_str = v8_obj->Get(ctx, v8_str(isolate, prop)).ToLocalChecked();
		if (v8_str->IsNullOrUndefined()) return FALSE;
		v8::String::Utf8Value str(isolate, v8_str);
		out = std::string(*str);
		return TRUE;
	}
	int v8_object_get_number(v8::Isolate* isolate, v8::Local<v8::Context> ctx, v8::Local<v8::Object> obj, const char* prop){
		v8::MaybeLocal<v8::Value> mval;
		mval = obj->Get(ctx, v8_str(isolate, prop));
		if (mval.IsEmpty()) {
			throw_js_error(isolate, "Value should be number...");
			return -500;
		}
		v8::Local<v8::Value> val = mval.ToLocalChecked();
		if (!val->IsNumber()) {
			throw_js_error(isolate, "Value should be number...");
			return -500;
		}
		return val->Int32Value(ctx).FromMaybe(0);
	}

	v8::Local<v8::String> sow_web_jsx::concat_msg(
		v8::Isolate* isolate, 
		const char* a, const char* b
	) {
		std::string*msg = ::concat_str(a, b);
		v8::Local<v8::String> val = v8_str(isolate, msg->c_str());
		_free_obj(msg);
		return val;
	}
#if defined(WEB_JSX_MULTI_THREAD)
	//https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
	void replace_all(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}
#endif//!WEB_JSX_MULTI_THREAD
	void sow_web_jsx::get_script_origin(const char* path_info, std::string& origin){
#if defined(WEB_JSX_MULTI_THREAD)
		replace_all(origin, "\\", "_");
#else
		std::regex* regx = new std::regex("(?:/)");
		std::string str = std::regex_replace(path_info, *regx, "_");
		str.swap(origin); delete regx;
#endif//!WEB_JSX_MULTI_THREAD
	}
	std::unique_ptr<v8::ScriptCompiler::CachedData> sow_web_jsx::read_script_cached(
		const char* script_path,
		const char* cscript_path,
		int check_file_state
	){
		if (check_file_state == TRUE) {
			if (::file_has_changed(script_path, cscript_path) == TRUE) {
				std::remove(cscript_path);
				return nullptr;
			}
		}
		
		std::ifstream* file = new std::ifstream(cscript_path, std::ifstream::binary);
		if (!file->is_open())return nullptr;
		std::vector<char>* dest = new std::vector<char>();
		int ret = ::load_file_to_vct(*file, *dest);
		file->close(); delete file;
		if (ret == FALSE) {
			_free_obj(dest);
			return nullptr;
		}
		std::unique_ptr<v8::ScriptCompiler::CachedData> cached_data;
		cached_data.reset(new v8::ScriptCompiler::CachedData(
			reinterpret_cast<const uint8_t*>(dest->data()),
			(int)dest->size())
		);
		_free_obj(dest);
		return cached_data;
	}
	int sow_web_jsx::create_script_cached_data(v8::MaybeLocal<v8::UnboundScript> unbound_script, const char* cscript_path){
		if (unbound_script.IsEmpty())return FALSE;
		v8::ScriptCompiler::CachedData* cdata = v8::ScriptCompiler::CreateCodeCache(unbound_script.ToLocalChecked());
		int ret = ::write_file(cscript_path, reinterpret_cast<const char*>(cdata->data), cdata->length);
		/*std::ofstream* file = new std::ofstream(cscript_path, std::ifstream::binary);
		int ret = FALSE;
		if (file->is_open()) {
			file->write(reinterpret_cast<const char*>(cdata->data), cdata->length);
			file->flush(); file->close();
			ret = TRUE;
		}delete file;*/
		delete cdata;
		return ret;
	}
	v8::Handle<v8::Object> sow_web_jsx::native_write_filei(
		v8::Isolate* isolate, 
		const char* abs_path, 
		const char* buffer
	) {
		FILE* fstream;
		errno_t err;
		err = fopen_s(&fstream, abs_path, "w+");
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
		if (err != 0 || fstream == NULL) {
			v8_result->Set(
				ctx, v8_str(isolate, "staus_code"), v8::Number::New(isolate, -1)
			);
			v8_result->Set(
				ctx, v8_str(isolate, "message"), sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path)
			);
			return v8_result;
		}
		size_t len = fwrite(buffer, sizeof(char), strlen(buffer), fstream);
		if (ferror(fstream)) {
			fclose(fstream);
			fstream = NULL;
			v8_result->Set(
				ctx, v8_str(isolate, "staus_code"), v8::Number::New(isolate, -1)
			);
			v8_result->Set(
				ctx, v8_str(isolate, "message"), sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path)
			);
			return v8_result;
		}
		fclose(fstream);
		fstream = NULL;
		v8_result->Set( ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, static_cast<int>(len)) );
		v8_result->Set( ctx, v8_str(isolate, "message"), v8_str(isolate, "Success...") );
		return v8_result;
	}
	void clear_isolate_data(v8::Isolate* isolate, int index) {
		//isolate->GetCurrentContext()->SetAlignedPointerInEmbedderData(1, nullptr);
		isolate->SetData((isolate->GetNumberOfDataSlots() + index) - 1, NULL);  // Not really needed, just to be sure...
		return;
	}
	/*[native_string 9:17 PM 12/5/2019]*/
	native_string::native_string(v8::Isolate* isolate, const v8::Local<v8::Value>& value){
		if (value->IsNullOrUndefined() || value->IsUndefined() || value.IsEmpty()) {
			_data = nullptr;
			_length = 0;
		}
		else if (value->IsString()) {
			_utf8Value = new v8::String::Utf8Value(isolate, value);
			//char _utf8ValueMemory[sizeof(v8::String::Utf8Value)];
			//_utf8Value = new (_utf8ValueMemory) v8::String::Utf8Value(isolate, value);
			_data = (**_utf8Value);
			_length = (size_t)_utf8Value->length();
		}
		else if (value->IsTypedArray()) {
			v8::Local<v8::ArrayBufferView> arrayBufferView = v8::Local<v8::ArrayBufferView>::Cast(value);
			v8::ArrayBuffer::Contents contents = arrayBufferView->Buffer()->GetContents();
			_length = arrayBufferView->ByteLength();
			_data = (char*)contents.Data() + arrayBufferView->ByteOffset();
		}
		else if (value->IsArrayBuffer()) {
			v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(value);
			v8::ArrayBuffer::Contents contents = arrayBuffer->GetContents();
			_length = contents.ByteLength();
			_data = (char*)contents.Data();
		}
		else {
			_invalid = true;
			_data = nullptr;
			_length = 0;
		}
	}
	bool native_string::is_invalid(v8::Isolate* isolate) {
		if ( _invalid ) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "form_data required!!!")));
		}
		return _invalid;
	}
	std::string_view native_string::get_string(){
		return { _data, _length };
	}
	const char* native_string::c_str() {
		return _length == 0 ? "" : get_string().data();
	}
	bool native_string::is_empty(){
		return _length == 0;
	}
	size_t native_string::size(){
		return _length;
	}
	void native_string::clear(){
		if (_invalid == true)return;
		if (_utf8Value && _length > 0) {
			_utf8Value->~Utf8Value();
			_utf8Value = nullptr;
			_data = nullptr; _length = 0;
		}
	}
	native_string::~native_string(){
		clear();
	}
	//[/native_string]
}
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:14 PM 2/12/2020
#	include "stdout_w.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include <web_jsx/wjsx_env.h>
#	include <iostream>
#	include "zgzip.h"
using namespace sow_web_jsx;
class std_out {
public:
	std_out();
	~std_out();
	template<class _out_stream>
	size_t write(_out_stream& dest, const char* buff);
	template<class _out_stream, class _source_stream>
	size_t write(_out_stream& dest, _source_stream& source, int bypass);
	template<class _out_stream>
	size_t write_file(_out_stream& dest, const char* file_path);
	template<class _out_stream>
	int flush(_out_stream& dest);
	int has_error();
	const char* get_last_error();
	void set_mood_gzip();
	int is_gzip();
private:
	int _is_gzip;
	int _is_flush;
	int _is_error;
	char* _internal_error;
	void clear();
	int panic(const char* error, int error_code);
};
std_out::std_out() {
	_is_flush = FALSE; _is_error = FALSE;
	_internal_error = NULL; _is_gzip = FALSE;
}
std_out::~std_out() {
	this->clear();
}
int std_out::has_error() {
	return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
}
const char* std_out::get_last_error() {
	if (has_error() == TRUE) {
		return const_cast<const char*>(_internal_error);
	}
	return "No Error Found!!!";
}
void std_out::set_mood_gzip() {
	_is_gzip = TRUE;
}
int std_out::is_gzip() {
	return _is_gzip;
}
void std_out::clear() {
	if (_is_flush == TRUE)return;
	_free_char(_internal_error);
}
int std_out::panic(const char* error, int error_code) {
	_free_char(_internal_error);
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	_is_error = error_code;
	return _is_error;
}
template<class _out_stream>
size_t std_out::write(_out_stream& dest, const char* buff) {
	if (_is_flush == TRUE)return FALSE;
	if (_is_error == TRUE)return -1;
	size_t len = strlen(buff);
	if (buff == NULL || len == 0)return TRUE;
	if (::stream_is_available(dest) == FALSE)return FALSE;
	dest.write(buff, len);
	return len;
}
template<class _out_stream, class _source_stream>
size_t std_out::write(_out_stream& dest, _source_stream& source, int bypass) {
	if (bypass == FALSE) {
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
	}
	size_t total_len = get_stream_length(source);
	if (total_len == std::string::npos || total_len == 0)return total_len;
	size_t totalSize = total_len;
	do {
		if (::stream_is_available(source) == FALSE)break;
		if (::stream_is_available(dest) == FALSE)break;
		char* in;
		size_t read_len = totalSize > CHUNK ? CHUNK : totalSize;
		in = new char[read_len];
		source.read(in, read_len);
		totalSize -= read_len;
		dest.write(in, read_len);
		/* Free memory */
		delete[]in;
		if (totalSize == 0 || totalSize == std::string::npos) break;
	} while (true);
	return total_len;
}
template<class _out_stream>
size_t std_out::write_file(_out_stream& dest, const char* file_path) {
	if (_is_flush == TRUE)return FALSE;
	if (_is_error == TRUE)return -1;
	std::ifstream file_stream(file_path, std::ifstream::ate | std::ifstream::binary);
	if (!file_stream.is_open()) {
		return panic("Unable to open file....", -1);
	}
	size_t total_len = this->write(dest, file_stream, TRUE);
	file_stream.close();
	return total_len;
}
template<class _out_stream>
int std_out::flush(_out_stream& dest) {
	if (_is_flush == TRUE)return 0;
	this->clear();
	_is_flush = TRUE;
	dest.flush();
	return 0;
}
void stdout_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	if (::unwrap_wjsx_env(isolate)->is_cli() == FALSE)return;
	/*std_out* cout = new std_out();
	std::stringstream strm;
	cout->write(std::cout, strm, FALSE);*/
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		std_out* cout = new std_out();
		if (cout->has_error() == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			delete cout;
			return;
		}
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, cout));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<std_out*>(&cout, [](const v8::WeakCallbackInfo<std_out*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	tpl->SetClassName(v8_str(isolate, "stdout"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	set_prototype(isolate, prototype, "flush_header", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		if (wj_env == NULL)return;
		if (wj_env->is_available_out_stream() == FALSE)return;
		if (wj_env->is_flush() == TRUE) {
			throw_js_error(isolate, "Headers already been flushed...");
			return;
		}
		std_out* cout = ::unwrap<std_out>(args);
		if (cout->has_error() == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			delete cout;
			return;
		}
		if (::wrapper::flush_http_status(wj_env) == FALSE)return;
		wj_env->set_flush();
		//if (sow_web_jsx::wrapper::set_binary_output() == FALSE)return;
		//sow_web_jsx::wrapper::set_flush_status(TRUE);
		int is_gzip = ::wrapper::is_gzip_encoding(wj_env);
		::wrapper::flush_header(wj_env);
		::wrapper::flush_cookies(wj_env);
		_WCOUT << "\r\n";
		if (is_gzip == FALSE) {
			cout->write(_WCOUT, wj_env->body(), FALSE);
		}
		else {
			cout->set_mood_gzip();
		}
		::wrapper::clear_cache(*wj_env);
		_WFLUSH();
	});
	set_prototype(isolate, prototype, "write_gzip_header", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		if (wj_env == NULL)return;
		if (wj_env->is_available_out_stream() == FALSE)return;
		if (wj_env->is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		std_out* cout = ::unwrap<std_out>(args);
		if (cout->is_gzip() == FALSE) {
			throw_js_error(isolate, "This is not gzip mood...");
			return;
		}
		::gzip::write_magic_header(_WCOUT);
	});
	set_prototype(isolate, prototype, "write", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		if (wj_env == NULL)return;
		if (wj_env->is_available_out_stream() == FALSE)return;
		if (wj_env->is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		native_string str(isolate, args[0]);
		std_out* cout = ::unwrap<std_out>(args);
		size_t ret = cout->write(_WCOUT, str.c_str());
		str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	});
	set_prototype(isolate, prototype, "write_from_file", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		if (wj_env == NULL)return;
		if (wj_env->is_available_out_stream() == FALSE)return;
		if (wj_env->is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string(wj_env->get_root_dir());
		::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		std_out* cout = ::unwrap<std_out>(args);
		size_t ret = cout->write_file(_WCOUT, abs_path->c_str());
		_free_obj(abs_path); utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	});
	set_prototype(isolate, prototype, "flush", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		if (wj_env == NULL)return;
		if (wj_env->is_available_out_stream() == TRUE) {
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			std_out* cout = ::unwrap<std_out>(args);
			if (cout == NULL) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			cout->flush(_WCOUT); delete cout;
		}
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		::wrapper::clear_cache(*wj_env);
	});
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, "stdout"), tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

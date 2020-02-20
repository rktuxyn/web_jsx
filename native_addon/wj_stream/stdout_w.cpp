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
	if (buff == NULL || strlen(buff) == 0)return TRUE;
	size_t len = strlen(buff);
	dest.write(buff, len);
	return len;
}
template<class _out_stream, class _source_stream>
size_t std_out::write(_out_stream& dest, _source_stream& source, int bypass) {
	if (bypass == FALSE) {
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
	}
	source.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = source.tellg();
	size_t total_len = (size_t)totalSize;
	source.seekg(0, std::ios::beg);//Back to begain of stream
	if (total_len == std::string::npos || total_len == 0)return total_len;
	do {
		if (!source.good())break;
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
	std::ifstream file_stream(file_path, std::ifstream::binary);
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
	sow_web_jsx::wrapper::clear_cache(TRUE, TRUE);
	fflush(stdout);
	return 0;
}
void stdout_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	if (sow_web_jsx::wrapper::is_cli() == TRUE)return;
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
	set_prototype(isolate, prototype, "flush_header",  [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (sow_web_jsx::wrapper::is_flush() == TRUE) {
			throw_js_error(isolate, "Headers already been flushed...");
			return;
		}
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		if (cout->has_error() == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			delete cout;
			return;
		}
		if (wrapper::flush_http_status() == FALSE)return;
		if (sow_web_jsx::wrapper::set_binary_output() == FALSE)return;
		sow_web_jsx::wrapper::set_flush_status(TRUE);
		int is_gzip = sow_web_jsx::wrapper::is_gzip_encoding();
		sow_web_jsx::wrapper::flush_header();
		sow_web_jsx::wrapper::flush_cookies();
		std::cout << "\r\n";
		if (is_gzip == FALSE) {
			cout->write(std::cout, sow_web_jsx::wrapper::get_body_stream(), FALSE);
		}
		else {
			cout->set_mood_gzip();
		}
		sow_web_jsx::wrapper::clear_cache(TRUE, FALSE);
		fflush(stdout);
	});
	set_prototype(isolate, prototype, "write_gzip_header", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (sow_web_jsx::wrapper::is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		if (cout->is_gzip() == FALSE) {
			throw_js_error(isolate, "This is not gzip mood...");
			return;
		}
		gzip::write_magic_header(std::cout);
	});
	set_prototype(isolate, prototype, "write", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (sow_web_jsx::wrapper::is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		native_string str(isolate, args[0]);
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		size_t ret = cout->write(std::cout, str.c_str());
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
		if (sow_web_jsx::wrapper::is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		size_t ret = cout->write_file(std::cout, abs_path->c_str());
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
		if (sow_web_jsx::wrapper::is_flush() == FALSE) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		if (cout == NULL) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		cout->flush(std::cout); delete cout;
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		sow_web_jsx::wrapper::clear_cache(TRUE, TRUE);
	});
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, "stdout"), tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

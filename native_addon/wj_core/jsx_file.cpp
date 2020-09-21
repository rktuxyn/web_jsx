/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "jsx_file.h"
#	include <web_jsx/v8_util.h>
#	include <web_jsx/wjsx_env.h>
#	include <io.h> 
#	include <string>
using namespace sow_web_jsx;
enum jsx_file_mode {
	f_read = 0x0001,
	f_write = 0x0002,
	f_append = 0x0003,
	f_read_write = 0x0004,
	f_ukn = -1
};
class jsx_file {
private:
	int _errorc;
	int _is_flush;
	int _dwrite_disk;
	char* _internal_error;
	//std::ifstream* _read_stream;
	//std::ofstream* _write_stream;
	FILE* _fstream;
	jsx_file_mode _file_mode;
	size_t _total_length;
	int panic(const char* error);
public:
	explicit jsx_file(const char* path, const char* mode);
	explicit jsx_file(const char* path, const char* mode, int dwrite_disk);
	jsx_file(const jsx_file&) = delete;
	jsx_file& operator=(const jsx_file&) = delete;
	int is_flush()const;
	size_t open(const char* path, const char* mode);
	size_t read(int len, std::string& out);
	int eof();
	size_t write(const char*);
	void flush();
	const char* get_last_error();
	void clear();
	int has_error()const;
	~jsx_file();
};
jsx_file_mode get_mode(const char* mode) {
	if (strcmp(mode, "r") == 0)return f_read;
	if (strcmp(mode, "w") == 0)return f_write;
	if (strcmp(mode, "a") == 0)return f_append;
	if (strcmp(mode, "r+") == 0)return f_read_write;
	if (strcmp(mode, "w+") == 0)return f_read_write;
	if (strcmp(mode, "a+") == 0)return f_read_write;
	return f_ukn;
}
jsx_file::jsx_file(const char *path, const char*mode) {
	_dwrite_disk = FALSE;
	_internal_error = NULL; _is_flush = TRUE;
	_file_mode = f_ukn; _errorc = FALSE;
	this->open(path, mode);
}
jsx_file::jsx_file(const char* path, const char* mode, int dwrite_disk){
	_dwrite_disk = dwrite_disk;
	_internal_error = NULL; _is_flush = TRUE;
	_file_mode = f_ukn; _errorc = FALSE;
	this->open(path, mode);
}
int jsx_file::is_flush()const {
	return _is_flush;
}
size_t jsx_file::read(int len, std::string& out) {
	if (this->_is_flush == TRUE)return -1;
	if (
		_file_mode == f_append ||
		_file_mode == f_read ||
		_file_mode == f_read_write
		) {
		if (this->eof() == TRUE)return -1;
		size_t read_req_len = _total_length > len ? len : _total_length;
		char* buff = new char[read_req_len + sizeof(char)];
		buff[read_req_len] = '\0';
		size_t read_len = fread(buff, 1, read_req_len, this->_fstream);
		if (ferror(this->_fstream)) {
			read_len = -1;
			panic("Error occured while reading file");
		}
		if (not has_error() == TRUE) {
			out.clear();
			out = std::string(buff, read_req_len);
			_total_length -= read_len;
		}
		delete[]buff;
		return read_len;
	}
	return panic("You should not read file in current mood.");
}
int jsx_file::eof(){
	if (not (_total_length > 0))return TRUE;
	return FALSE;
}
size_t jsx_file::open(const char* path, const char* mode) {
	if (this->_is_flush == FALSE) return -1;
	_file_mode = get_mode(mode);
	if (_file_mode == f_ukn) {
		return panic("Invalid file mode defined...");
	}
	this->clear();
	//https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=vs-2017
	this->_is_flush = FALSE;
#if (defined(_WIN32) || defined(_WIN64))
	this->_fstream = _fsopen(path, mode, _SH_DENYWR);
	if (ferror(this->_fstream)) {
		this->clear();
		this->panic("Unable to open/create file...");
	}
	else {
		if (
			_file_mode == f_append ||
			_file_mode == f_read ||
			_file_mode == f_read_write
			) {
			fseek(this->_fstream, 0, SEEK_END);//Go to end of stream
			_total_length = ftell(this->_fstream);
			rewind(this->_fstream);//Back to begain of stream
		}
		else {
			_total_length = 0;
		}
	}
#else
	errno_t err = fopen_s(&this->_fstream, path, mode);
	if ( err != 0) {
		this->panic("Unable to open/create file...");
	}
#endif
	return _errorc;
}
size_t jsx_file::write(const char *buffer) {
	if (this->_is_flush == TRUE)return -1;
	if (
		_file_mode == f_write ||
		_file_mode == f_append ||
		_file_mode == f_read_write
		) {
		size_t len = fwrite(buffer, sizeof(char), strlen(buffer), this->_fstream);
		if (ferror(this->_fstream)) {
			this->clear();
			return panic("Error occured while writing file");
		}
		if (_dwrite_disk == TRUE)
			fflush(this->_fstream);//write each line into io
		return len;
	}
	return panic("You should not write file in current mood.");
}

void jsx_file::flush() {
	if (this->_is_flush == TRUE)return;
	this->_is_flush = TRUE;
	fflush(this->_fstream);
	fclose(this->_fstream);
	this->_fstream = NULL;
}

const char* jsx_file::get_last_error() {
	if (has_error() == FALSE) return "No Error Found!!!";
	return const_cast<const char*>(_internal_error);
}

void jsx_file::clear(){
	this->flush();
	if (_internal_error != NULL) {
		_errorc = FALSE;
		delete[]_internal_error;
	}
}

int jsx_file::has_error()const {
	return _errorc < 0 || _errorc == TRUE ? TRUE : FALSE;
}

jsx_file::~jsx_file() {
	this->clear();
}
int jsx_file::panic(const char* error) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	/*_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);*/
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	this->_errorc = -1;
	return -1;
}
void sow_web_jsx::jsx_file_bind(v8::Isolate* isolate, v8::Local<v8::Object> ctx) {
	v8::Local<v8::FunctionTemplate> jsx_file_tmplate = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_type_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			throw_js_type_error(isolate, "Path required!!!");
			return;
		}
		if (args[1]->IsNullOrUndefined() || !args[1]->IsString()) {
			throw_js_type_error(isolate, "Mood required!!!");
			return;
		}
		bool dwrite_disk = false;
		if (args.Length() > 2) {
			if (args[2]->IsNullOrUndefined() || !args[2]->IsBoolean()) {
				throw_js_type_error(isolate, "Argument should be boolean!!!");
				return;
			}
			dwrite_disk = ::to_boolean(isolate, args[2]);
		}
		native_string utf_abs_path_str(isolate, args[0]);
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		std::string* abs_path = new std::string(wj_env->get_root_dir());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		native_string utf_mood_str(isolate, args[1]);
		jsx_file* x_file = new jsx_file(abs_path->c_str(), utf_mood_str.c_str(), dwrite_disk == true ? TRUE : FALSE);
		utf_mood_str.clear();
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, x_file));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<jsx_file*>(&x_file, [](const v8::WeakCallbackInfo<jsx_file*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
		_free_obj(abs_path); utf_abs_path_str.clear();
	});
	jsx_file_tmplate->SetClassName(v8_str(isolate, "file"));
	jsx_file_tmplate->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = jsx_file_tmplate->PrototypeTemplate();
	// Add object properties to the prototype
	// Methods, Properties, etc.
	prototype->Set(isolate, "read", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->has_error() == TRUE) {
			throw_js_error(iso, jf->get_last_error());
			return;
		}
		if (jf->is_flush() == TRUE) {
			throw_js_error(iso, "Already flush this file!!!");
			return;
		}
		int read_len = 1024;
		if (args.Length() > 0) {
			v8::Local<v8::Context>ctx = iso->GetCurrentContext();
			v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
			read_len = static_cast<int>(num->ToInteger(ctx).ToLocalChecked()->Value());
		}
		_NEW_STR(out);
		size_t out_len = jf->read(read_len, *out);
		if (is_error_code(out_len) == TRUE) {
			args.GetReturnValue().Set(v8_str(iso, out->c_str()));
		}
		_free_obj(out);
	}));
	prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->has_error() == TRUE) {
			throw_js_error(iso, jf->get_last_error());
			return;
		}
		if (jf->is_flush() == TRUE) {
			throw_js_error(iso, "Already flush this file!!!");
			return;
		}
		native_string utf_str(iso, args[0]);
		size_t ret = jf->write(utf_str.c_str()); utf_str.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(iso, jf->get_last_error());
		}
		else {
			args.GetReturnValue().Set(v8::Integer::New(iso, static_cast<int>(ret)));
		}
		return;
	}));
	prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->has_error() == TRUE) {
			throw_js_error(iso, jf->get_last_error());
			return;
		}
		if (jf->is_flush() == TRUE) {
			throw_js_error(iso, "Already flush this file!!!");
			return;
		}
		jf->flush();
	}));
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	v8::Local<v8::Object>file_mood_enum = v8::Object::New(isolate);
	file_mood_enum->Set(context, v8_str(isolate, "OPEN_READ"), v8_str(isolate, "r"));
	file_mood_enum->Set(context, v8_str(isolate, "CREATE_OPEN_WRITE"), v8_str(isolate, "w"));
	file_mood_enum->Set(context, v8_str(isolate, "CREATE_OPEN_APPEND"), v8_str(isolate, "a"));
	file_mood_enum->Set(context, v8_str(isolate, "OPEN_READ_WRITE_R"), v8_str(isolate, "r+"));
	file_mood_enum->Set(context, v8_str(isolate, "OPEN_READ_WRITE_W"), v8_str(isolate, "w+"));
	file_mood_enum->Set(context, v8_str(isolate, "OPEN_READ_WRITE_A"), v8_str(isolate, "a+"));
	wjsx_set_object(isolate, ctx, "mood", file_mood_enum);
	ctx->Set(context, v8_str(isolate, "file"), jsx_file_tmplate->GetFunction(context).ToLocalChecked());
}
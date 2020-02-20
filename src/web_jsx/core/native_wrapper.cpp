/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:46 AM 11/21/2018
#	include "web_jsx_global.h"
#	include "native_wrapper.h"
#	include "v8_util.h"
#	include "directory_.h"
#	include "zgzip.hpp"
#	include "base64.h"
#	include "n_help.h"
#	include "jsx_file.h"
#	include "http_payload.h"
#if defined(WEB_JSX_CLIENT_BUILD)
#	include "encryption.h"
#endif//WEB_JSX_CLIENT_BUILD
#	include "bitmap.h"
#	include "native_module.h"
#	include "module_store.h"
#if defined(FAST_CGI_APP)
#	include <fcgi_stdio.h>
#	include "fcgio.h"
#else
#if !defined(DATA_READ_CHUNK)
#	define DATA_READ_CHUNK 16384
#endif//!DATA_READ_CHUNK
#endif//!FAST_CGI_APP
#define _set_srd(_str, str)\
_free_obj(_str);\
_str = new std::string(str);

using namespace sow_web_jsx;
/*[Help]*/
std::string* _root_dir = NULL;
const char* _root_dir_c = NULL;
std::string* _app_dir = NULL;
const char* _app_dir_c = NULL;
void set_root_dir(const char* root_dir) {
	_set_srd(_root_dir, root_dir);
	_root_dir_c = _root_dir->c_str();
}
void set_app_dir(const char* app_dir) {
	_set_srd(_app_dir, app_dir);
	_app_dir_c = _app_dir->c_str();
}
bool _is_interactive = false;
bool _is_cli = false;
bool _is_flush = false;
std::stringstream _body_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
std::map<std::string, std::string>* _headers = NULL;
std::vector<std::string>* _cookies = NULL;
std::vector<std::string>* _http_status = NULL;
/*[/Help]*/
void jsx_file_bind(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
	v8::Local<v8::FunctionTemplate> jsx_file_tmplate = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args.IsConstructCall()) {
			iso->ThrowException(v8::Exception::TypeError(v8_str(iso, "Cannot call constructor as function!!!")));
			return;
		}
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(v8_str(iso, "Path required!!!")));
			return;
		}
		if (args[1]->IsNullOrUndefined() || !args[1]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(v8_str(iso, "Mood required!!!")));
			return;
		}
		native_string utf_abs_path_str(iso, args[0]);
		std::string* abs_path = new std::string(_root_dir_c);
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		native_string utf_mood_str(iso, args[1]);
		jsx_file* x_file = new jsx_file(abs_path->c_str(), utf_mood_str.c_str());
		utf_mood_str.clear();
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(iso, x_file));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(iso, obj);
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
		std::string* out = new std::string();
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
	v8::Local<v8::ObjectTemplate>file_mood_enum = v8::ObjectTemplate::New(isolate);
	file_mood_enum->Set(isolate, "OPEN_READ", v8_str(isolate, "r"));
	file_mood_enum->Set(isolate, "CREATE_OPEN_WRITE", v8_str(isolate, "w"));
	file_mood_enum->Set(isolate, "CREATE_OPEN_APPEND", v8_str(isolate, "a"));
	file_mood_enum->Set(isolate, "OPEN_READ_WRITE_R", v8_str(isolate, "r+"));
	file_mood_enum->Set(isolate, "OPEN_READ_WRITE_W", v8_str(isolate, "w+"));
	file_mood_enum->Set(isolate, "OPEN_READ_WRITE_A", v8_str(isolate, "a+"));
	ctx->Set(isolate, "mood", file_mood_enum);
	ctx->Set(isolate, "file", jsx_file_tmplate);
}
V8_JS_METHOD(v8_gc) {
	args.GetIsolate()->LowMemoryNotification();
}
///Open new process and forget
///@param process_path process full path
///@param arg process argument
///@throws Process not found
///@returns {process_id}
V8_JS_METHOD(native_open_process) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Argument(s) required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	native_string utf_arg_str(isolate, args[1]);
	int ret = sow_web_jsx::open_process(utf_abs_path_str.c_str(), utf_arg_str.c_str());
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "Not found!!!")));
		return;
	};
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
};
void get_prop_value(v8::Local<v8::Context> ctx, v8::Isolate* isolate, v8::Local<v8::Object> obj, const char* prop, std::string& out) {
	v8::Local<v8::Value> v8_str = obj->Get(ctx, v8_str(isolate, prop)).ToLocalChecked();
	if (v8_str->IsNullOrUndefined())return;
	native_string utf8_str(isolate, v8_str);
	out = utf8_str.c_str();
}
void get_req_process_info(v8::Local<v8::Context> ctx, v8::Isolate* isolate, v8::Local<v8::Object> pi, process_info& pri) {
	get_prop_value(ctx, isolate, pi, "start_in", pri.start_in);
	if (!pri.start_in.empty())
		pri.start_in = std::regex_replace(pri.start_in, std::regex("(?:/)"), "\\");
	get_prop_value(ctx, isolate, pi, "process_name", pri.process_name);
	get_prop_value(ctx, isolate, pi, "process_path", pri.process_path);
	if (!pri.process_path.empty())
		pri.process_path = std::regex_replace(pri.process_path, std::regex("(?:/)"), "\\");
	get_prop_value(ctx, isolate, pi, "title", pri.lp_title);
	get_prop_value(ctx, isolate, pi, "arg", pri.arg);
	if (_is_cli == false) {
		//Internal request defined
		if (pri.arg.empty())
			pri.arg = "internal_request";
		else
			pri.arg = "internal_request " + pri.arg;
	}
	/*if (pri.arg.empty())
		pri.arg = "I_REQ";
	else
		pri.arg = "I_REQ " + pri.arg;*/
	pri.show_window = SW_HIDE;//SW_SHOWNORMAL;// CREATE_NO_WINDOW;//(int)get_prop_value(isolate, pi, "show_window");
	//wait_for_exit
	v8::Local<v8::Value> v8_bool = pi->Get(ctx, v8_str(isolate, "wait_for_exit")).ToLocalChecked();
	if (v8_bool->IsBoolean()) {
		pri.wait_for_exit = sow_web_jsx::to_boolean(isolate, v8_bool) ? 1 : -1;
	}
	else {
		pri.wait_for_exit = -1;
	}
}
int spawn_uv_process(const process_info pi) {
	//http://docs.libuv.org/en/v1.x/guide/processes.html
	uv_loop_t* loop;
	uv_process_t child_req;
	uv_process_options_t options;
	loop = uv_default_loop();
	const char* args[3];
	args[0] = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx.exe";
	args[1] = pi.arg.c_str();// "m.jsxh";
	args[2] = NULL;
	options.cwd = pi.start_in.c_str();////"C:\\web_jsx\\";
	options.exit_cb = NULL;
	options.file = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx.exe";
	options.args = (char**)args;

	options.flags = UV_PROCESS_DETACHED;
	int r;
	if ((r = uv_spawn(loop, &child_req, &options))) {
		fprintf(stderr, "%s\n", uv_strerror(r));
		return 1;
	}
	fprintf(stderr, "Launched sleep with PID %d\n", child_req.pid);
	uv_unref((uv_handle_t*)& child_req);
	return uv_run(loop, UV_RUN_DEFAULT);
}
///Kill any open process by name e.g. web_jsx.exe
///@param process_name e.g. web_jsx.exe
///@throws Permission denied
///@returns {-1|0}
V8_JS_METHOD(native_kill_process_by_name) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "App name required!!!")));
		return;
	}
	native_string process_name(isolate, args[0]);
	int ret = sow_web_jsx::kill_process_by_name(process_name.c_str());
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	process_name.clear();
}
///Spwan new process|child process
///@param option Containing {start_in:string, process_name:string, process_path:string, title:string, arg:string, wait_for_exit:boolean}
///param start_in -> Process start directory default current location; not required
///param process_name -> Process name default null; not required
///param process_path -> Process full location required e.g. C:/web_jsx/web_jsx.exe
///param title -> Process title not required
///param arg -> Process argument not required
///param wait_for_exit -> If you need to wait untill Process exit, than set true default false
V8_JS_METHOD(native_create_process) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Process info required!!!")));
		return;
	}
	process_info pri;// = new process_info();
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> pi = v8::Handle<v8::Object>::Cast(args[0]);
	get_req_process_info(ctx, isolate, pi, pri);
	int ret = 0;
	if (_is_cli == false) {
		//No Child process available in web
		//SW_SHOWNORMAL
		pri.show_window = SW_SHOWNORMAL;
		pri.dw_creation_flags = CREATE_NO_WINDOW;
		pri.wait_for_exit = -1;
		ret = sow_web_jsx::create_process(&pri);
	}
	else {
		v8::Local<v8::Value> js_reader = pi->Get(ctx, v8_str(isolate, "reader")).ToLocalChecked();
		if (!js_reader->IsFunction()) {
			std::string ptype;
			get_prop_value(ctx, isolate, pi, "process_type", ptype);
			if (ptype.empty())
				pri.dw_creation_flags = CREATE_NO_WINDOW;
			else {
				if (ptype == "CHILD")
					pri.dw_creation_flags = CREATE_NEW_PROCESS_GROUP;
				else
					pri.dw_creation_flags = CREATE_NO_WINDOW;
			}
			if (pri.dw_creation_flags == CREATE_NO_WINDOW)pri.wait_for_exit = -1;
			std::string().swap(ptype);
			ret = sow_web_jsx::create_process(&pri);
		}
		else {
			//ret = spawn_uv_child_process(*pri);
			pri.wait_for_exit = -1;
			v8::Persistent<v8::Function> cb;
			cb.Reset(isolate, v8::Local<v8::Function>::Cast(js_reader));
			v8::Local<v8::Object>global = args.Holder();
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
			//std::cout << "Creating child process and reading data..."  << std::endl;
			ret = sow_web_jsx::read_child_process(&pri, [&](size_t i, const char* buff) {
				//std::cout << "Reading data from pipe:" << buff << std::endl;
				v8::Handle<v8::Value> arg[2] = {
					v8::Number::New(isolate, static_cast<double>(i)),
					v8_str(isolate, buff)
				};
				callback->Call(ctx, global, 2, arg);
			});
		}
	}
	std::string().swap(pri.start_in);
	std::string().swap(pri.process_name);
	std::string().swap(pri.process_path);
	std::string().swap(pri.lp_title);
	std::string().swap(pri.arg);
	pi.Clear();
	if (ret < 0) {
		switch (ret) {
		case -4: throw_js_error(isolate, "process not found!!!"); return;
		case -10: throw_js_error(isolate, "process path required!!!"); return;
		case -9: throw_js_error(isolate, "Unable to execute shall command!!!"); return;
		default: throw_js_error(isolate, "Unknown Error defined!!!"); return;
		};
	}
	switch (ret) {
	case ERROR_FILE_NOT_FOUND: throw_js_error(isolate, "ERROR_FILE_NOT_FOUND!!!"); return;
	case ERROR_PATH_NOT_FOUND: throw_js_error(isolate, "ERROR_PATH_NOT_FOUND!!!"); return;
	case ERROR_DIRECTORY: throw_js_error(isolate, "ERROR_DIRECTORY!!!"); return;
	case ERROR_NOT_ENOUGH_MEMORY: throw_js_error(isolate, "ERROR_NOT_ENOUGH_MEMORY!!!"); return;
	case ERROR_INVALID_NAME: throw_js_error(isolate, "ERROR_INVALID_NAME!!!"); return;
	default:break;
	}
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));

}
///Create new child process
///@param process_path process full path
///@param arg process argument
///@throws Process not found
///@returns {1}
V8_JS_METHOD(native_create_child_process) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Argument(s) required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string process_path = utf_abs_path_str.c_str();
	native_string arg(isolate, args[1]);
	//std::string arg = *utf_arg_str;
	process_path = std::regex_replace(process_path, std::regex("(?:/)"), "\\");
	long ret = sow_web_jsx::create_child_process(process_path.c_str(), arg.c_str());
	arg.clear(); std::string().swap(process_path); utf_abs_path_str.clear();
	if (ret < 0) {
		throw_js_error(isolate, "Not found!!!");
		return;
	};
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
}
///Terminate open process by process id
///@param pid process id
///@throws Permission denied
///@returns {if process found 1 or 0}
V8_JS_METHOD(native_terminate_process) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	uint64_t pid = num->ToInteger(ctx).ToLocalChecked()->Value();
	int rec = 0;
#if defined(_WIN32)||defined(_WIN64)
	rec = sow_web_jsx::terminate_process((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
///Check given process id is running
///@param pid define Process Id
///@returns {true|false}
V8_JS_METHOD(native_process_is_running) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	//v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	uint64_t pid = args[0]->ToInteger(ctx).ToLocalChecked()->Value();//num->ToInteger(isolate)->Value();
	int rec = 0;
#if defined(_WIN32)||defined(_WIN64)
	rec = sow_web_jsx::process_is_running((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
V8_JS_METHOD(native_current_process_id) {
	v8::Isolate* isolate = args.GetIsolate();
	int rec = 0;
#if defined(_WIN32)||defined(_WIN64)
	rec = (int)sow_web_jsx::current_process_id();
#else
#error !TODO
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
//[FileSystem]
V8_JS_METHOD(native_exists_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	int ret = 0;
	if (__file_exists(abs_path->c_str()) == false) ret = 1;
	_free_obj(abs_path); utf_abs_path_str.clear();
	args.GetReturnValue().Set(v8::Boolean::New(isolate, ret > 0));
}
V8_JS_METHOD(native_write_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File data required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	native_string utf_data_str(isolate, args[1]);
	auto abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	v8::Handle<v8::Object> v8_result = sow_web_jsx::native_write_filei(isolate, abs_path->c_str(), utf_data_str.c_str());
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear(); _free_obj(abs_path);
	utf_abs_path_str.clear(); utf_data_str.clear();
}
V8_JS_METHOD(native_read_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), ssstream, true);
	_free_obj(abs_path);
	//const char* cstr2 = ssstream.str().c_str();
	//std::stringstream().swap(ssstream);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (is_error_code(ret) == TRUE) {
		v8_result->Set(
			ctx,
			v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, ret == std::string::npos ? -1 : (double)ret)
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "message"),
			v8_str(isolate, ssstream.str().c_str())
		);
		args.GetReturnValue().Set(v8_result);
		std::stringstream().swap(ssstream);
		return;
	}
	v8_result->Set(
		ctx,
		v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, (double)ret)
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "message"),
		v8_str(isolate, "Success...")
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "data"),
		v8_str(isolate, ssstream.str().c_str())
	);
	args.GetReturnValue().Set(v8_result);
	std::stringstream().swap(ssstream);
	return;
}
V8_JS_METHOD(native_write_from_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), _body_stream, true);
	if (is_error_code(ret) == TRUE) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->c_str())));
	}
	_free_obj(abs_path);
	return;
}
V8_JS_METHOD(exists_directory) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Directory required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	int rec = sow_web_jsx::dir_exists(abs_path->c_str());
	abs_path->clear(); _free_obj(abs_path);
	utf_abs_path_str.clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, (double)rec));
}
V8_JS_METHOD(read_directory_regx) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Match pattern required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	native_string utf_ext_str(isolate, args[1]);
	auto reg = new std::regex(utf_ext_str.c_str());//"(html|aspx|jsx|php)"
	rec = sow_web_jsx::read_directory_sub_directory_x(abs_path->c_str(), *directorys, *reg);
	delete reg; utf_ext_str.clear(); utf_abs_path_str.clear();
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (rec == EXIT_FAILURE) {
		_free_obj(directorys);
		v8_result->Set(
			ctx,
			v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
		);
		_free_obj(abs_path);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	_free_obj(abs_path);
	std::vector<std::string>& json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set(ctx, (int)i, v8_str(isolate, json_obj[i].c_str()));
	}
	_free_obj(directorys);
	v8_result->Set(
		ctx,
		v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
}
V8_JS_METHOD(read_directory) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0; utf_abs_path_str.clear();
	if (args[1]->IsString() || !args[1]->IsNullOrUndefined()) {
		native_string utf_ext_str(isolate, args[1]);
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->c_str(), *directorys, utf_ext_str.c_str());
		utf_ext_str.clear();
	}
	else {
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->c_str(), *directorys, "A");
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (rec == EXIT_FAILURE) {
		_free_obj(directorys);
		v8_result->Set(
			ctx,
			v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
		);
		args.GetReturnValue().Set(v8_result);
		_free_obj(abs_path);
		return;
	}
	_free_obj(abs_path);
	std::vector<std::string>& json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set(ctx, (int)i, v8_str(isolate, json_obj[i].c_str()));
	}
	_free_obj(directorys);
	v8_result->Set(
		ctx,
		v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
}
V8_JS_METHOD(native_delete_directory) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Directory required!!!");
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::delete_dir(abs_dir->c_str());
	if (rec > 0)
		args.GetReturnValue().Set(v8_str(isolate, "Success"));
	else
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Directory does not exists!!! Server absolute path==>", abs_dir->c_str())));
	_free_obj(abs_dir);
	return;

}
V8_JS_METHOD(native_create_directory) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Server path required!!!");
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::create_directory(abs_dir->c_str());
	if (rec > 0) {
		args.GetReturnValue().Set(v8_str(isolate, "Success"));
	}
	else {
		if (rec == -1) {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Directory already exists!!! Dir#", abs_dir->c_str()));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Unknown error please retry!!! Dir#", abs_dir->c_str()));
		}
	}
	_free_obj(abs_dir);
	return;

}
//[/FileSystem]
//8:53 PM 12/6/2019
//[Asynchronous]
V8_JS_METHOD(async_func) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "Function required!!!")));
		return;
	}
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	isolate->Enter();
	std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		try {
			v8::HandleScope			handleScope(isolate);
			v8::Isolate::Scope		isolate_scope(isolate);
			v8::Local<v8::Context>	context = isolate->GetCurrentContext();
			v8::Context::Scope		context_scope(context);
			v8::Local<v8::Object>	global = context->Global();
			v8::Handle<v8::Value> arg[1] = {
				v8::Number::New(isolate, 0)
			};
			callback->Call(context, global, 1, arg);
			context.Clear(); global.Clear();
			isolate->Exit();
			try {
				std::stringstream ss;
				ss << std::this_thread::get_id();
				uint64_t id = std::stoull(ss.str());
				return (int)id;
			}
			catch (...) {
				return 1;
			}
		}
		catch (...) {
			return -1;
		}
		});
	int rec = result.get();
	cb.Reset();
	callback.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
V8_JS_METHOD(sleep_func) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Sleep time Required!!!")));
		return;
	}
	//v8::Unlocker unlocker(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	int time = args[0]->Int32Value(ctx).FromMaybe(0);
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
	//	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	//	int64_t milliseconds = num->ToInteger(ctx).ToLocalChecked()->Value();
	//#if defined(_WIN32)||defined(_WIN64)
	//	Sleep((DWORD)milliseconds);
	//#else
	//	sleep(reinterpret_cast<unsigned int*>(milliseconds));
	//#endif//!_WINDOWS_
}
V8_JS_METHOD(set_time_out_func) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Callback Required!!!")));
		return;
	}
	uint64_t milliseconds = 0;
	if (args[1]->IsNumber()) {
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Number> num = args[1]->ToNumber(ctx).ToLocalChecked();
		milliseconds = num->ToInteger(ctx).ToLocalChecked()->Value();
	}
	async_func_arg* afa = new async_func_arg();
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	int rec = sow_web_jsx::set_time_out(isolate, sow_web_jsx::async_callback, afa, milliseconds);
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
V8_JS_METHOD(async_t) {
	v8::Isolate* isolate = args.GetIsolate();
	async_func_arg* afa = new async_func_arg();
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	sow_web_jsx::acync_init(isolate, sow_web_jsx::async_callback, afa);
	args.GetReturnValue().Set(v8_str(isolate, "START"));
}
V8_JS_METHOD(_async_thread) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsArray() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Array required!!!")));
		return;
	}
	int rec = sow_web_jsx::async_thread(isolate, v8::Local<v8::Array>::Cast(args[0]));
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
//[/Asynchronous]
//2:08 AM 11/25/2019
V8_JS_METHOD(response_redirect) {
	v8::Isolate* isolate = args.GetIsolate();
#if defined(FAST_CGI_APP)
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Redirect location required!!!")));
		return;
	}
	auto desc = new std::string("Status: 303 See Other");
	desc->append(H_N_L);
	native_string url_str(isolate, args[0]);
#else
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Redirect location required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value url_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	desc->append(" 303 See Other\n");
#endif//FAST_CGI_APP
	desc->append("Location: ");
	desc->append(url_str.c_str());
	n_help::add_http_status(*_http_status, *desc);
	_free_obj(desc); url_str.clear();
}
//[Encryption/Decryption]
V8_JS_METHOD(encrypt_source) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(WEB_JSX_CLIENT_BUILD)
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	auto cipher = new Cipher(1000);
	std::string encrypted_text = cipher->encrypt(utf_soruce_str.c_str());
	v8_result->Set(
		ctx,
		v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "data"),
		v8_str(isolate, encrypted_text.c_str())
	);
	args.GetReturnValue().Set(v8_result);
	std::string().swap(encrypted_text);
	v8_result.Clear(); utf_soruce_str.clear();
	return;
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!WEB_JSX_CLIENT_BUILD
}
V8_JS_METHOD(decrypt_source) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(WEB_JSX_CLIENT_BUILD)
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	auto cipher = new Cipher(-1000);
	std::string decrypted_text = cipher->encrypt(utf_soruce_str.c_str());
	v8_result->Set(
		ctx,
		v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "data"),
		v8_str(isolate, decrypted_text.c_str())
	);
	args.GetReturnValue().Set(v8_result);
	std::string().swap(decrypted_text);
	v8_result.Clear(); utf_soruce_str.clear();
	return;
#else
	isolate->ThrowException(v8::Exception::Error(
		v8_str(isolate, "Not Implemented!!!")));
	return;
#endif//!WEB_JSX_CLIENT_BUILD
}
V8_JS_METHOD(base64_encode) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Plain text required!!!")));
		return;
	}
	native_string utf_plain_text(isolate, args[0]);
	//std::string bas64str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(utf_plain_text.c_str()), (int)utf_plain_text.size());
	std::string* bas64str = new std::string();
	sow_web_jsx::base64::to_encode_str(utf_plain_text.c_str(), *bas64str);
	args.GetReturnValue().Set(v8_str(isolate, bas64str->c_str()));
	_free_obj(bas64str); utf_plain_text.clear();
}
V8_JS_METHOD(base64_decode) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Plain text required!!!")));
		return;
	}
	native_string utf_base64_text(isolate, args[0]);
	//std::string plain_str = sow_web_jsx::base64::to_decode_str(utf_base64_text.c_str());
	std::string* plain_str = new std::string();
	sow_web_jsx::base64::to_decode_str(utf_base64_text.c_str(), *plain_str);
	args.GetReturnValue().Set(v8_str(isolate, plain_str->c_str()));
	_free_obj(plain_str); utf_base64_text.clear();
}
V8_JS_METHOD(hex_to_string_js) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "Hex string required!!!")));
		return;
	}
	native_string utf_hex_str(isolate, args[0]);
	std::string* hex_str = new std::string(utf_hex_str.c_str());
	std::string* plain_str = new std::string("");
	sow_web_jsx::hex_to_string(*hex_str, *plain_str);
	_free_obj(hex_str); utf_hex_str.clear();
	args.GetReturnValue().Set(v8_str(isolate, plain_str->c_str()));
	_free_obj(plain_str);
	return;
}
V8_JS_METHOD(string_to_hex_js) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "Plain string required!!!")));
		return;
	}
	native_string utf_plain_str(isolate, args[0]);
	std::string* plain_str = new std::string(utf_plain_str.c_str());
	std::string* hex_str = new std::string("");
	sow_web_jsx::string_to_hex(*plain_str, *hex_str);
	_free_obj(plain_str); utf_plain_str.clear();
	args.GetReturnValue().Set(v8_str(isolate, hex_str->c_str()));
	_free_obj(hex_str);
	return;
}
//[/Encryption/Decryption]
//12:09 PM 8/27/2019
V8_JS_METHOD(set_cookie) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "HTTP Cookie required!!!")));
		return;
	}
	native_string resp_cookie_str(isolate, args[0]);
	const char* cook_val = resp_cookie_str.c_str();
	std::vector<std::string>::iterator itr = std::find(_cookies->begin(), _cookies->end(), cook_val);
	if (itr != _cookies->end()) {
		_cookies->erase(itr);
	}
	_cookies->push_back(cook_val);
}
V8_JS_METHOD(http_status) {
	v8::Isolate* isolate = args.GetIsolate();
#if defined(FAST_CGI_APP)
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "HTTP Status Code Required!!!")));
		return;
	}
	native_string status_code_str(isolate, args[0]);
	std::string* desc = new std::string("Status:");
	desc->append(status_code_str.c_str());
	if (args[1]->IsString() && !args[1]->IsNullOrUndefined()) {
		desc->append(" ");
		native_string dec_str(isolate, args[1]);
		desc->append(dec_str.c_str());
	}
#else
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "HTTP Status Code Required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value status_code_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	desc->append(" ");
	desc->append(*status_code_str);
	desc->append(" ");
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		desc->append("Unspecified Description");
	}
	else {
		v8::String::Utf8Value dec_str(isolate, args[2]);
		desc->append(*dec_str);
	}
#endif//!FAST_CGI_APP
	n_help::add_http_status(*_http_status, *desc);
	_free_obj(desc); status_code_str.clear();
	return;
}
V8_JS_METHOD(response_write_header) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Header Key string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Header Description string required!!!")));
		return;
	}
	native_string key_str(isolate, args[0]);
	native_string description_str(isolate, args[1]);
	n_help::add_header(*_headers, key_str.c_str(), description_str.c_str());
	return;
}
V8_JS_METHOD(response_write) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined())return;
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	_body_stream << *utf8_str;
	return;
}
V8_JS_METHOD(response_throw_error) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString()) {
		throw_js_error(isolate, "Error Description required....");
		return;
	}
	_body_stream.clear();
	std::stringstream().swap(_body_stream);
	_http_status->push_back("");
	native_string utf8_str(isolate, args[0]);
	_body_stream << "Throw Error:</br>";
	_body_stream << utf8_str.c_str(); utf8_str.clear();
	if (!_http_status->empty()) {
		_http_status->clear();
	}
	if (args[1]->IsNumber()) {
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int status_code = args[1]->Int32Value(ctx).FromMaybe(0);
		response_status rs = n_help::get_http_response_status(status_code);
		if (rs == response_status::UNAUTHORIZED ||
			rs == response_status::FORBIDDEN ||
			rs == response_status::INTERNAL_SERVER_ERROR||
			rs == response_status::NOT_IMPLEMENTED) {
			_http_status->push_back(std::to_string(status_code));
			return;
		}
		throw_js_error(isolate, "Invalid Status code defined....");
		return;
	}
	else {
		_http_status->push_back("500");
	}
	return;
}
V8_JS_METHOD(response_clear) {
	//v8::Isolate* isolate = args.GetIsolate();
	_body_stream.clear();
	std::stringstream().swap(_body_stream);
	args.GetReturnValue().Set(args.Holder());
}
V8_JS_METHOD(get_response_body) {
	args.GetReturnValue().Set(v8_str(args.GetIsolate(), _body_stream.str().c_str()));
}
V8_JS_METHOD(server_map_path) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	args.GetReturnValue().Set(v8_str(isolate, abs_path->c_str()));
	_free_obj(abs_path); utf_abs_path_str.clear();
}
size_t get_content_length() {
	_body_stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = _body_stream.tellg();
	_body_stream.seekg(0, std::ios::beg);//Back to begain of stream
	return (size_t)totalSize;
}
void __clear_cache(int clean_body = 0, int clean_root = 1 ) {
	sow_web_jsx::free_working_module();
	_free_obj(_http_status); _free_obj(_headers); _free_obj(_cookies);
	if (clean_root == TRUE) {
		_free_obj(_root_dir);
		_free_obj(_app_dir);
	}
	if (clean_body == TRUE) {
		if (get_content_length() > 0) {
			_body_stream.clear();
			std::stringstream().swap(_body_stream);
		}
	}
}
int _set_binary_output() {
	if (SET_BINARY_MODE_OUT() != -1)return TRUE;
	/*[Nothing to do when failed...]*/
	return TRUE;
}
int _write_http_status() {
	_body_stream.clear();
	std::stringstream().swap(_body_stream);
	n_help::write_http_status(*_http_status, false);
	n_help::write_header(*_headers);
	n_help::write_cookies(*_cookies);
	__clear_cache(TRUE);
	std::cout << "\r\n";
	fflush(stdout);
	return FALSE;
}
int write_http_status() {
	response_status status_code = n_help::get_http_response_status(*_http_status);
	switch (status_code)
	{
	case response_status::OK: return TRUE;
	case response_status::MOVED:
	case response_status::REDIRECT:
	case response_status::SEEOTHER:
	case response_status::NOTMODIFIED: return _write_http_status();
	case response_status::UNAUTHORIZED:
	case response_status::FORBIDDEN:
	case response_status::NOT_FOUND:
	case response_status::INTERNAL_SERVER_ERROR:
	case response_status::NOT_IMPLEMENTED:
		if (SET_BINARY_MODE_OUT() == -1) {
			/*_body_stream << "<br/>Another Error occured:<br/>";
			_body_stream << "ERROR: while converting cout to binary:" << strerror(errno);*/
		}
		n_help::error_response(
			/*const char* server_root*/_root_dir_c,
			/*response_status status_code*/status_code,
			/*const std::string error_msg*/_body_stream.str().c_str()
		);
		__clear_cache(TRUE);
		return FALSE;
	default:
		throw new std::runtime_error("Invalid Http Response Status defined...");
	}
}
/*[zgip]*/
void gzip_compress_write() {
	std::ios::sync_with_stdio(false);
	if (write_http_status() == FALSE)return;
	if (_set_binary_output() == FALSE)return;
	n_help::write_header(*_headers);
	n_help::write_cookies(*_cookies);
	__clear_cache(FALSE);
	std::cout << "\r\n";
	gzip::compress_gzip(_body_stream, std::cout);
	_body_stream.clear(); std::stringstream().swap(_body_stream);
	fflush(stdout);
}
/*[/zgip]*/
void bitmap_export(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		bitmap* bmp = NULL;
		if (args[0]->IsString()) {
			native_string utf_str(isolate, args[0]);
			std::string* abs_path = new std::string(_root_dir_c);
			sow_web_jsx::get_server_map_path(utf_str.c_str(), *abs_path);

			bmp = new bitmap(abs_path->c_str(), image_format::BMP);
			utf_str.clear(); _free_obj(abs_path);
		}
		else {
			bmp = new bitmap(image_format::BMP);
		}
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, bmp));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<bitmap*>(&bmp, [](const v8::WeakCallbackInfo<bitmap*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	tpl->SetClassName(v8_str(isolate, "bitmap"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL)return;
		bmp->free_memory();
		delete bmp; bmp = NULL;
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
	}));
	prototype->Set(isolate, "lock_bits", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "unlock_bits", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "reset", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		bmp->reset_rgb();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "release_mem", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL)return;
		bmp->clear();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "dump_data", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		bmp->dump_data();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "load", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string(_root_dir_c);
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int ret = bmp->load(abs_path->c_str());
		_free_obj(abs_path); utf_abs_path_str.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "to_base64", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		std::string* out = new std::string();
		int ret = bmp->to_base64(*out);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), "Unable to convert base64 image...");
		}
		else {
			args.GetReturnValue().Set(v8_str(args.GetIsolate(), out->c_str()));
		}
		_free_obj(out);
	}));
	prototype->Set(isolate, "load_from_base64", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "base64 Data required....");
			return;
		}
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		native_string utf_base64(isolate, args[0]);
		int ret = bmp->from_base64(utf_base64.c_str());
		utf_base64.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "save", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path required....");
			return;
		}
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, "Image does not loaded yet...");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string(_root_dir_c);
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int ret = bmp->save(abs_path->c_str(), image_format::BMP);
		_free_obj(abs_path); utf_abs_path_str.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "get_width", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, "Image does not loaded yet...");
			return;
		}
		uint32_t width = bmp->get_width();
		args.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int>(width)));
	}));
	prototype->Set(isolate, "get_height", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, "Image does not loaded yet...");
			return;
		}
		uint32_t height = bmp->get_height();
		args.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int>(height)));
	}));
	prototype->Set(isolate, "get_pixel", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(isolate, "x y required...");
			return;
		}
		if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
			throw_js_error(isolate, "x y should be number...");
			return;
		}
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int x = args[0]->Int32Value(ctx).FromMaybe(0);
		int y = args[1]->Int32Value(ctx).FromMaybe(0);
		rgb32* pixel = bmp->get_pixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8_result->Set(ctx, v8_str(isolate, "r"), v8::Integer::New(isolate, static_cast<int>(pixel->r)));
		v8_result->Set(ctx, v8_str(isolate, "g"), v8::Integer::New(isolate, static_cast<int>(pixel->g)));
		v8_result->Set(ctx, v8_str(isolate, "b"), v8::Integer::New(isolate, static_cast<int>(pixel->b)));
		v8_result->Set(ctx, v8_str(isolate, "a"), v8::Integer::New(isolate, static_cast<int>(pixel->a)));
		args.GetReturnValue().Set(v8_result); v8_result.Clear();
	}));
	prototype->Set(isolate, "resize", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(isolate, "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_error(isolate, "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_error(isolate, "Width required...");
			return;
		}
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "create_canvas", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(isolate, "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_error(isolate, "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_error(isolate, "Width required...");
			return;
		}
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->create_canvas(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "set_pixel", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		v8::Isolate* isolate = args.GetIsolate();
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		if (args.Length() < 3) {
			throw_js_error(isolate, "rgba and x y required...");
			return;
		}
		if (!args[0]->IsObject()) {
			throw_js_error(isolate, "rgba should be object...");
			return;
		}
		if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
			throw_js_error(isolate, "x y should be number...");
			return;
		}
		v8::Local<v8::Object> rgba = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		rgb32* pixel = new rgb32();
		int val = v8_object_get_number(isolate, ctx, rgba, "r");
		if (val == -500) { delete pixel; return; }
		pixel->r = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "g");
		if (val == -500) { delete pixel; return; }
		pixel->g = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "b");
		if (val == -500) { delete pixel; return; }
		pixel->b = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "a");
		if (val == -500) { delete pixel; return; }
		pixel->a = (uint8_t)val;
		int x = args[1]->Int32Value(ctx).FromMaybe(0);
		int y = args[2]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->set_pixel(pixel, static_cast<unsigned int>(x), static_cast<unsigned int>(y));
		//delete pixel;
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	ctx->Set(isolate, "BitMap", tpl);
}
void SetEngineInformation(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> v8_global) {
	v8::Local<v8::ObjectTemplate> js_engine_object = v8::ObjectTemplate::New(isolate);
	js_engine_object->Set(isolate, "version", v8_str(isolate, v8::V8::GetVersion()));
	js_engine_object->Set(isolate, "name", v8_str(isolate, "V8"));
	v8_global->Set(isolate, "engine", js_engine_object);
}
//9:32 PM 11/22/2018
V8_JS_METHOD(require) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(WEB_JSX_CLIENT_BUILD)
	bool is_encrypt = false;
	if (args[1]->IsBoolean()) {
		is_encrypt = sow_web_jsx::to_boolean(isolate, args[1]);
	}
#endif//!WEB_JSX_CLIENT_BUILD
	native_string utf_abs_path_str(isolate, args[0]);
	typeof_module ext = get_module_type(utf_abs_path_str.c_str());
	if (ext == typeof_module::_UNKNOWN) {
		throw_js_error(isolate, "Unsupported module...");
		utf_abs_path_str.clear();
		return;
	}
	std::string* abs_path = new std::string();
	if (ext == typeof_module::NATIVE) {
		bool is_full_path = false;
		if (args.Length() > 1) {
			if (args[1]->IsBoolean()) {
				is_full_path = to_boolean(isolate, args[1]);
			}
		}
		if (is_full_path == false) {
			abs_path->append(_root_dir_c);
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		}
		else {
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		}
		require_native(args, abs_path->c_str(), _app_dir_c, utf_abs_path_str.c_str());
		_free_obj(abs_path); utf_abs_path_str.clear();
		return;
	}
	abs_path->append(_root_dir_c);
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	if (ext == typeof_module::NO_EXT) {
		abs_path->append(".js");
	}
	if (__file_exists(abs_path->c_str()) == false) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Module not found. Module# ", utf_abs_path_str.c_str())));
		_free_obj(abs_path); utf_abs_path_str.clear();
		return;
	}
	std::string source_str("");
#if defined(WEB_JSX_CLIENT_BUILD)
	if (is_encrypt) {
		size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, false);
		_free_obj(abs_path);
		if (is_error_code(ret) == TRUE) {
			utf_abs_path_str.clear();
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, source_str.c_str())));
			return;
		}
		Cipher* decipher = new Cipher(-1000);
		source_str = decipher->encrypt(source_str);
		delete decipher;
	}
	else {
		size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, false);
		_free_obj(abs_path);
		if (is_error_code(ret) == TRUE) {
			utf_abs_path_str.clear();
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, source_str.c_str())));
			return;
		}
	}
#else
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, true);
	abs_path->clear(); delete abs_path;
	if (is_error_code(ret) == TRUE) {
		utf_abs_path_str.clear();
		isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, source_str.c_str())));
		return;
	}
#endif//!WEB_JSX_CLIENT_BUILD
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> v8_global = sow_web_jsx::wrapper::create_v8_context_object(isolate);
	v8::Local<v8::ObjectTemplate> module_object = v8::ObjectTemplate::New(isolate);
	module_object->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8_global->Set(isolate, "module", module_object);
	v8_global->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8_global);
	v8::Context::Scope context_scope(context);
	swjsx_module::scope_to_js_global(isolate, context);
	// Compile the source code.
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, v8_str(isolate, source_str.c_str()));
	if (script.IsEmpty()) {
		source_str.clear();
		utf_abs_path_str.clear();
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Unable to compile script. Check your script than try again. Path: ", utf_abs_path_str.c_str())));
		context.Clear();
		v8_global.Clear();
		return;
	}
	utf_abs_path_str.clear();
	// Run the script to get the result.
	script.ToLocalChecked()->Run(context);
	source_str.clear();
	v8::Local<v8::Object> jsGlobal =
		context->Global()->GetPrototype().As<v8::Object>();
	v8::Local<v8::Object> modules = v8::Handle<v8::Object>::Cast(jsGlobal->Get(context, v8_str(isolate, "module")).ToLocalChecked());
	args.GetReturnValue().Set(modules->Get(context, v8_str(isolate, "exports")).ToLocalChecked());
	modules.Clear(); script.ToLocalChecked().Clear();
	jsGlobal.Clear(); context.Clear();
	v8_global.Clear();
	return;
}
V8_JS_METHOD(implimant_native_module) {
	if (_is_flush == true)return;
	swjsx_module::implimant_native_module(args, _app_dir_c, _root_dir_c);
}
v8::Local<v8::ObjectTemplate> sow_web_jsx::wrapper::get_context(v8::Isolate* isolate, std::map<std::string, std::map<std::string, std::string>>& ctx) {
#if defined(FAST_CGI_APP)
	_is_flush = false;
#endif//FAST_CGI_APP
	__clear_cache(TRUE);
	v8::Local<v8::ObjectTemplate> _v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		auto key = itr->first;
		auto obj = itr->second;
		if (key == "global") {
			for (auto gitr = obj.begin(); gitr != obj.end(); ++gitr) {
				ctx_object->Set(isolate, (gitr->first).c_str(), v8_str(isolate, gitr->second.c_str()));
			}
			//if (_root_dir == NULL)
			//_root_dir = new std::string(obj["root_dir"]);
			set_root_dir(obj["root_dir"].c_str());
			set_app_dir(obj["app_dir"].c_str());
			//_app_dir = new std::string(obj["app_dir"]);
			//std::cout << "app_dir:" << _app_dir->c_str() << std::endl;
			//
			continue;
		}
		v8::Local<v8::ObjectTemplate> object = v8::ObjectTemplate::New(isolate);
		for (auto oitr = obj.begin(); oitr != obj.end(); ++oitr) {
			object->Set(isolate, (oitr->first).c_str(), v8_str(isolate, oitr->second.c_str()));
		}
		if (key == "request") {
			object->Set(isolate, "_read_payload", v8::FunctionTemplate::New(isolate, sow_web_jsx::read_payload));
			object->Set(isolate, "read_posted_file", v8::FunctionTemplate::New(isolate, read_http_posted_file));
			object->Set(isolate, "_write_file_from_payload", v8::FunctionTemplate::New(isolate, sow_web_jsx::write_file_from_payload));
		}
		ctx_object->Set(isolate, key.c_str(), object);
	}
	/*[context.respons....]*/
	v8::Local<v8::ObjectTemplate> response_object = v8::ObjectTemplate::New(isolate);
	response_object->Set(isolate, "_write", v8::FunctionTemplate::New(isolate, response_write));
	response_object->Set(isolate, "throw_error", v8::FunctionTemplate::New(isolate, response_throw_error));
	response_object->Set(isolate, "header", v8::FunctionTemplate::New(isolate, response_write_header));
	response_object->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, native_write_from_file));
	v8::Local<v8::ObjectTemplate> body_object = v8::ObjectTemplate::New(isolate);
	body_object->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "Should not here.")));
		return;
	}));
	response_object->Set(isolate, "body", body_object);
	response_object->Set(isolate, "clear", v8::FunctionTemplate::New(isolate, response_clear));
	response_object->Set(isolate, "_status", v8::FunctionTemplate::New(isolate, http_status));
	response_object->Set(isolate, "_cookie", v8::FunctionTemplate::New(isolate, set_cookie));
	response_object->Set(isolate, "_redirect", v8::FunctionTemplate::New(isolate, response_redirect));
	response_object->Set(isolate, "_as_gzip", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		n_help::add_header(*_headers, "Content-Encoding", "gzip");
		return;
	}));
	ctx_object->Set(isolate, "is_interactive", v8::Boolean::New(isolate, false));
	//_is_interactive
	ctx_object->Set(isolate, "response", response_object);
	/*[server_map_path]*/
	ctx_object->Set(isolate, "server_map_path", v8::FunctionTemplate::New(isolate, server_map_path));
	/*[/server_map_path]*/
	/*[/context.respons....]*/
	_v8_global->Set(isolate, "context", ctx_object);
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "exists_file", v8::FunctionTemplate::New(isolate, native_exists_file));
	//io_object->Set(isolate, "_write_file_from_payload", v8::FunctionTemplate::New(isolate, write_file_from_payload));
	jsx_file_bind(isolate, io_object);
	_v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "exists_directory", v8::FunctionTemplate::New(isolate, exists_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	sys_object->Set(isolate, "gc", v8::FunctionTemplate::New(isolate, v8_gc));
	sys_object->Set(isolate, "async_thread", v8::FunctionTemplate::New(isolate, _async_thread));
	sys_object->Set(isolate, "load_native_module", v8::FunctionTemplate::New(isolate, implimant_native_module));
	_v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	_v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[Base64....]*/
	v8::Local<v8::ObjectTemplate> base64_object = v8::ObjectTemplate::New(isolate);
	base64_object->Set(isolate, "encode", v8::FunctionTemplate::New(isolate, base64_encode));
	base64_object->Set(isolate, "decode", v8::FunctionTemplate::New(isolate, base64_decode));
	_v8_global->Set(isolate, "base64", base64_object);
	/*[/Base64....]*/
	/*[require]*/
	_v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	_v8_global->Set(isolate, "__get_response_body", v8::FunctionTemplate::New(isolate, get_response_body));
	//
	_v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string_js));
	_v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex_js));
	_v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	_v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	_v8_global->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, set_time_out_func));
	//
	//
	_v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	/*[bitmap]*/
	bitmap_export(isolate, _v8_global);
	/*[/bitmap]*/
	SetEngineInformation(isolate, _v8_global);
	_headers = new std::map<std::string, std::string>();
	_cookies = new std::vector<std::string>();
	_http_status = new std::vector<std::string>();
	_is_interactive = false;
	/*[Create C++ Internal Context]*/
	return _v8_global;
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
V8_JS_METHOD(Print) {
	bool first = true;
	v8::Isolate* isolate = args.GetIsolate();
	//v8::HandleScope handle_scope(args.GetIsolate());
	for (int i = 0; i < args.Length(); i++) {
		if (first) {
			first = false;
		}
		else {
			std::cout << " ";
		}
		const char* cstr = sow_web_jsx::to_char_str(isolate, args[i]);
		std::cout << cstr;
	}
	std::cout << std::endl;
	std::cout << std::flush;
}
v8::Local<v8::ObjectTemplate> sow_web_jsx::wrapper::get_console_context(v8::Isolate* isolate, std::map<std::string, std::string>& ctx) {
	_is_cli = true;
	__clear_cache(TRUE);
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		if (itr->first == "root_dir") {
			set_root_dir(itr->second.c_str());
		}
		else if (itr->first == "is_interactive") {
			_is_interactive = itr->second == "1";
			ctx_object->Set(isolate, "is_interactive", v8::Boolean::New(isolate, _is_interactive));
			continue;
		}
		else if (itr->first == "app_dir") {
			set_app_dir(itr->second.c_str());
		}
		ctx_object->Set(isolate, itr->first.c_str(), v8_str(isolate, itr->second.c_str()));
	}
	/*[server_map_path]*/
	ctx_object->Set(isolate, "server_map_path", v8::FunctionTemplate::New(isolate, server_map_path));
	/*[/server_map_path]*/
	v8_global->Set(isolate, "env", ctx_object);
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[Base64....]*/
	v8::Local<v8::ObjectTemplate> base64_object = v8::ObjectTemplate::New(isolate);
	base64_object->Set(isolate, "encode", v8::FunctionTemplate::New(isolate, base64_encode));
	base64_object->Set(isolate, "decode", v8::FunctionTemplate::New(isolate, base64_decode));
	v8_global->Set(isolate, "base64", base64_object);
	/*[/Base64....]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string_js));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex_js));
	/*[__async]*/
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	/*[__async]*/
	/*[__sleep]*/
	v8_global->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, set_time_out_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	/*[__sleep]*/
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "exists_file", v8::FunctionTemplate::New(isolate, native_exists_file));
	jsx_file_bind(isolate, io_object);
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "exists_directory", v8::FunctionTemplate::New(isolate, exists_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	sys_object->Set(isolate, "gc", v8::FunctionTemplate::New(isolate, v8_gc));
	sys_object->Set(isolate, "async_thread", v8::FunctionTemplate::New(isolate, _async_thread));
	sys_object->Set(isolate, "read_line", v8::FunctionTemplate::New(isolate, sow_web_jsx::read_line));
	sys_object->Set(isolate, "load_native_module", v8::FunctionTemplate::New(isolate, implimant_native_module));
	v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	v8_global->Set(isolate, "__clear", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		delete _root_dir; _root_dir = NULL;
	}));
	v8_global->Set(isolate, "stderr", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!_is_interactive)return;
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			fprintf(stdout, "%s", "undefined");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "Formeted string required!!!");
			return;
		}
		native_string utf_msg_str(isolate, args[0]);
		sow_web_jsx::fprintf_stderr(utf_msg_str.c_str());
		utf_msg_str.clear();
		return;
	}));
	v8_global->Set(isolate, "stdout", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!_is_interactive)return;
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			fprintf(stdout, "%s", sow_web_jsx::to_char_str(isolate, args[0]));
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "Formeted string required!!!");
			return;
		}
		native_string utf_msg_str(isolate, args[0]);
		sow_web_jsx::fprintf_stdout(utf_msg_str.c_str());
		utf_msg_str.clear();
		return;
	}));
	v8_global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!_is_interactive)return;
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument Required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			std::cout << sow_web_jsx::to_char_str(isolate, args[0]) << std::endl;
			return;
		}
		if (args[0]->IsString()) {
			native_string utf_msg_str(isolate, args[0]);
			std::cout << utf_msg_str.c_str() << "\n";
			utf_msg_str.clear();
			return;
		}
		Print(args);
	}));
	/*[bitmap]*/
	bitmap_export(isolate, v8_global);
	/*[/bitmap]*/
	SetEngineInformation(isolate, v8_global);
	return v8_global;
}
v8::Local<v8::ObjectTemplate> sow_web_jsx::wrapper::create_v8_context_object(v8::Isolate* isolate) {
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[IO/file read/write]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "exists_file", v8::FunctionTemplate::New(isolate, native_exists_file));
	jsx_file_bind(isolate, io_object);
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "exists_directory", v8::FunctionTemplate::New(isolate, exists_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	sys_object->Set(isolate, "gc", v8::FunctionTemplate::New(isolate, v8_gc));
	sys_object->Set(isolate, "async_thread", v8::FunctionTemplate::New(isolate, _async_thread));
	v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	/*[crypto]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto]*/
	/*[Base64....]*/
	v8::Local<v8::ObjectTemplate> base64_object = v8::ObjectTemplate::New(isolate);
	base64_object->Set(isolate, "encode", v8::FunctionTemplate::New(isolate, base64_encode));
	base64_object->Set(isolate, "decode", v8::FunctionTemplate::New(isolate, base64_decode));
	v8_global->Set(isolate, "base64", base64_object);
	/*[/Base64....]*/
	/*[server_map_path]*/
	v8_global->Set(isolate, "server_map_path", v8::FunctionTemplate::New(isolate, server_map_path));
	/*[/server_map_path]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string_js));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex_js));
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	v8_global->Set(isolate, "is_interactive", v8::Boolean::New(isolate, _is_interactive));
	/*[bitmap]*/
	bitmap_export(isolate, v8_global);
	/*[/bitmap]*/
	SetEngineInformation(isolate, v8_global);
	return v8_global;
}
//9:32 PM 11/22/2018
void sow_web_jsx::wrapper::response_body_flush(bool end_req) {
	if (_is_flush == true)return;
	_is_flush = true;
	if (end_req == true ||
		std::cout.good() == false ||
		std::cout.fail() == true
		) {
		//We defined here force close request...
		__clear_cache(TRUE); fflush(stdout);
		return;
	}
	if (n_help::is_gzip_encoding(*_headers) == TRUE) {
		gzip_compress_write();
		return;
	}
	std::ios::sync_with_stdio(false);
	if (write_http_status() == FALSE)return;
	if (_set_binary_output() == FALSE)return;
	n_help::write_header(*_headers);
	n_help::write_cookies(*_cookies);
	__clear_cache(FALSE);
	std::cout << "\r\n";
	fflush(stdout);
	std::copy(std::istreambuf_iterator<char>(_body_stream),
		std::istreambuf_iterator<char>(),
		std::ostream_iterator<char>(std::cout)
	);
	fflush(stdout);
	_body_stream.clear(); std::stringstream().swap(_body_stream);
	return;
}
void sow_web_jsx::wrapper::clear_cache() {
	if (_is_cli == true)return;
	if (_is_flush == true)return;
	__clear_cache(TRUE);
}
void sow_web_jsx::wrapper::clear_cache(int clean_body, int clean_root) {
	__clear_cache(clean_body, clean_root);
}

int sow_web_jsx::wrapper::set_binary_output() {
	return _set_binary_output();
}
int sow_web_jsx::wrapper::set_binary_mode_in() {
	return SET_BINARY_MODE_IN();
}
const char* sow_web_jsx::wrapper::get_root_dir() {
	return _root_dir_c;
}
const char* sow_web_jsx::wrapper::get_app_dir() {
	return _app_dir_c;
}
int sow_web_jsx::wrapper::is_cli() {
	return _is_cli == true ? TRUE : FALSE;
}
int sow_web_jsx::wrapper::is_flush() {
	return _is_flush == true ? TRUE : FALSE;
}
int sow_web_jsx::wrapper::set_flush_status(int flush) {
	_is_flush = flush == TRUE ? true : false;
	return is_flush();
}
void sow_web_jsx::wrapper::add_header(const char* key, const char* value) {
	n_help::add_header(*_headers, key, value);
}
std::stringstream& sow_web_jsx::wrapper::get_body_stream() {
	return _body_stream;
}
int sow_web_jsx::wrapper::is_http_status_ok() {
	return n_help::write_http_status(*_http_status, true) < 0 ? FALSE : TRUE;
}
int sow_web_jsx::wrapper::is_gzip_encoding() {
	return n_help::is_gzip_encoding(*_headers);
}
int sow_web_jsx::wrapper::flush_http_status() {
	return write_http_status();
}
void sow_web_jsx::wrapper::flush_header() {
	n_help::write_header(*_headers);
}
void sow_web_jsx::wrapper::flush_cookies() {
	n_help::write_cookies(*_cookies);
}
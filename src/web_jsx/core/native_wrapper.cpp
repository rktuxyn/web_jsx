/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:46 AM 11/21/2018
#include "native_wrapper.h"
#if !defined(_http_request_h)
#include "http_request.h"
#endif//_http_request_h
#if !defined(_smtp_client_h)
#include "smtp_client.h"
#endif//_smtp_client_h
#if !defined(_pdf_generator_h)
#include "pdf_generator.h"
#endif//_npgsql_tools_h
#if !defined(directory__h)
#include "directory_.h"
#endif//!directory__h
#if !defined(_zgzip_h)
#include "zgzip.h"
#endif//!_zgzip_h
#if !defined(_crypto_h)
#include "crypto.h"
#endif//!_crypto_h
#if !defined(_base64_h)
#include "base64.h"
#endif//!_base64_h
#if !defined(_npgsql_wrapper_h)
#include "npgsql_wrapper.h"
#endif//_npgsql_wrapper_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
#if !defined(_n_help_h)
#include "n_help.h"
#endif//_n_help_h
#if !defined(_jsx_file_h)
#include "jsx_file.h"
#endif//_n_help_h
#if !defined(_uws_app_h)
#include "uws/uws_app.h"
#endif//!_uws_app_h
#if !defined(_mysql_wrapper_h)
#include "mysql_wrapper.h"
#endif//!_mysql_wrapper_h
#if !defined(_http_payload_h)
#include "http_payload.h"
#endif//!_http_payload_h
#if defined(__client_build)
#if !defined(_encryption_h)
#include "encryption.h"
#endif//!_encryption_h
#endif//__client_build
using namespace sow_web_jsx;
/*[Help]*/
std::string* _root_dir = NULL;
bool _is_interactive = false;
bool _is_cli = false;
bool _is_flush = false;
std::stringstream _body_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
std::map<std::string, std::string>* _headers = NULL;
std::vector<std::string>* _cookies = NULL;
std::vector<std::string>* _http_status = NULL;
/*[/Help]*/
void throw_js_error(v8::Isolate* isolate, const char* err) {
	isolate->ThrowException(v8::Exception::Error(
		sow_web_jsx::v8_str(isolate, err)));
	return;
}
void jsx_file_bind(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args.IsConstructCall()) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Cannot call constructor as function!!!")));
			return;
		}
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Path required!!!")));
			return;
		}
		if (args[1]->IsNullOrUndefined() || !args[1]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Mood required!!!")));
			return;
		}
		v8::String::Utf8Value utf_abs_path_str(iso, args[0]);
		v8::String::Utf8Value utf_mood_str(iso, args[1]);
		//sow_web_jsx::make_object<jsx_file>(iso, args.This(), *utf_abs_path_str, *utf_mood_str);
		v8::Local<v8::Object> obj = args.This();
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
		jsx_file* xx = new jsx_file(abs_path->c_str(), *utf_mood_str);
		obj->SetInternalField(0, v8::External::New(iso, xx));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(iso, obj);
		pobj.SetWeak<jsx_file*>(&xx, [](const v8::WeakCallbackInfo<jsx_file*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
		delete abs_path;
	});
	tpl->SetClassName(sow_web_jsx::v8_str(isolate, "file"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	// Add object properties to the prototype
	// Methods, Properties, etc.
	prototype->Set(isolate, "read", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->err != 0) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Unable to create or open the file!!!")));
			return;
		}
		args.GetReturnValue().Set(sow_web_jsx::v8_str(iso, jf->read()));
	}));
	prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->err != 0) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Unable to create or open the file!!!")));
			return;
		}
		if (jf->is_flush == 1) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Already flush this file!!!")));
			return;
		}
		v8::String::Utf8Value utf_str(iso, args[0]);
		size_t ret = jf->write(*utf_str);
		if (ret > 0) {
			args.GetReturnValue().Set(v8::Number::New(iso, (double)ret));
			return;
		}
		iso->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(iso, "Unable to write data to file!!!")));
		return;
	}));
	prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		jsx_file* jf = sow_web_jsx::unwrap<jsx_file>(args);
		if (jf->err != 0) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Unable to create or open the file!!!")));
			return;
		}
		if (jf->is_flush == 1) {
			iso->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(iso, "Already flush this file!!!")));
			return;
		}
		jf->flush();
	}));
	ctx->Set(isolate, "file", tpl);
}
void v8_gc(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetIsolate()->LowMemoryNotification();
}
///Open new process and forget
///@param process_path process full path
///@param arg process argument
///@throws Process not found
///@returns {process_id}
void native_open_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Argument(s) required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	native_string utf_arg_str(isolate, args[1]);
	int ret = sow_web_jsx::open_process(utf_abs_path_str.c_str(), utf_arg_str.c_str());
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(
			sow_web_jsx::v8_str(isolate, "Not found!!!")));
		return;
	};
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
};
void get_prop_value(v8::Local<v8::Context> ctx, v8::Isolate* isolate, v8::Local<v8::Object> obj, const char* prop, std::string& out) {
	v8::Local<v8::Value> v8_str = obj->Get(ctx, sow_web_jsx::v8_str(isolate, prop)).ToLocalChecked();
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
			pri.arg = "I_REQ";
		else
			pri.arg = "I_REQ " + pri.arg;
	}
	/*if (pri.arg.empty())
		pri.arg = "I_REQ";
	else
		pri.arg = "I_REQ " + pri.arg;*/
	pri.show_window = SW_HIDE;//SW_SHOWNORMAL;// CREATE_NO_WINDOW;//(int)get_prop_value(isolate, pi, "show_window");
	//wait_for_exit
	v8::Local<v8::Value> v8_bool = pi->Get(ctx, sow_web_jsx::v8_str(isolate, "wait_for_exit")).ToLocalChecked();
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
	args[0] = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx_cgi.exe";
	args[1] = pi.arg.c_str();// "m.jsxh";
	args[2] = NULL;
	options.cwd = pi.start_in.c_str();////"C:\\web_jsx\\";
	options.exit_cb = NULL;
	options.file = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx_cgi.exe";
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
///Kill any open process by name e.g. web_jsx_cgi.exe
///@param process_name e.g. web_jsx_cgi.exe
///@throws Permission denied
///@returns {-1|0}
void native_kill_process_by_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(sow_web_jsx::v8_str(isolate, "App name required!!!")));
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
///param process_path -> Process full location required e.g. C:/web_jsx/web_jsx_cgi.exe
///param title -> Process title not required
///param arg -> Process argument not required
///param wait_for_exit -> If you need to wait untill Process exit, than set true default false
void native_create_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Process info required!!!")));
		return;
	}
	process_info* pri = new process_info();
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> pi = v8::Handle<v8::Object>::Cast(args[0]);
	get_req_process_info(ctx, isolate, pi, *pri);
	int ret = 0;
	if (_is_cli == false) {
		//No Child process available in web
		//SW_SHOWNORMAL
		pri->show_window = SW_SHOWNORMAL;
		pri->dw_creation_flags = CREATE_NO_WINDOW;
		pri->wait_for_exit = -1;
		ret = sow_web_jsx::create_process(pri);
	}
	else {
		v8::Local<v8::Value> js_reader = pi->Get(ctx, sow_web_jsx::v8_str(isolate, "reader")).ToLocalChecked();
		if (!js_reader->IsFunction()) {
			std::string ptype;
			get_prop_value(ctx, isolate, pi, "process_type", ptype);
			if (ptype.empty())
				pri->dw_creation_flags = CREATE_NO_WINDOW;
			else {
				if (ptype == "CHILD")
					pri->dw_creation_flags = CREATE_NEW_PROCESS_GROUP;
				else
					pri->dw_creation_flags = CREATE_NO_WINDOW;
			}
			if (pri->dw_creation_flags == CREATE_NO_WINDOW)
				pri->wait_for_exit = -1;
			std::string().swap(ptype);
			ret = sow_web_jsx::create_process(pri);
		}
		else {
			pri->wait_for_exit = -1;
			v8::Persistent<v8::Function> cb;
			cb.Reset(isolate, v8::Local<v8::Function>::Cast(js_reader));
			v8::Local<v8::Object>global = args.Holder();
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
			ret = sow_web_jsx::read_child_process(pri, [&](long i, const char* buff) {
				v8::Handle<v8::Value> arg[2] = {
					v8::Number::New(isolate, i),
					sow_web_jsx::v8_str(isolate, buff)
				};
				callback->Call(ctx, global, 2, arg);
			});
		}
	}
	std::string().swap(pri->start_in);
	std::string().swap(pri->process_name);
	std::string().swap(pri->process_path);
	std::string().swap(pri->lp_title);
	std::string().swap(pri->arg);
	delete pri;
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
	//throw_js_error(isolate, std::string("ret" + std::to_string(ret) + "arg:" + pri->arg + "; start_in:" + pri->start_in + "; process_name:" + pri->process_name + "; process_path:" + pri->process_path).c_str());

}
///Create new child process
///@param process_path process full path
///@param arg process argument
///@throws Process not found
///@returns {1}
void native_create_child_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Argument(s) required!!!")));
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
void native_terminate_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	uint64_t pid = num->ToInteger(ctx).ToLocalChecked()->Value();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = sow_web_jsx::terminate_process((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
///Check given process id is running
///@param pid define Process Id
///@returns {true|false}
void native_process_is_running(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	//v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	uint64_t pid = args[0]->ToInteger(ctx).ToLocalChecked()->Value();//num->ToInteger(isolate)->Value();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = sow_web_jsx::process_is_running((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
void native_current_process_id(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = (int)sow_web_jsx::current_process_id();
#else
#error !TODO
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
//[FileSystem]
void native_exists_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	int ret = 0;
	if (__file_exists(abs_path->c_str()) == false)
		ret = 1;
	delete abs_path;
	args.GetReturnValue().Set(v8::Boolean::New(isolate, ret > 0));
}
void native_write_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File data required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	v8::String::Utf8Value utf_data_str(isolate, args[1]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	v8::Handle<v8::Object> v8_result = sow_web_jsx::native_write_filei(isolate, *abs_path, *utf_data_str);
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear(); delete abs_path;
}
void native_read_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), ssstream, true);
	delete abs_path;
	//const char* cstr2 = ssstream.str().c_str();
	//std::stringstream().swap(ssstream);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (ret < 0 || ret == std::string::npos) {
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, ret == std::string::npos ? -1 : (double)ret)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "message"),
			sow_web_jsx::v8_str(isolate, ssstream.str().c_str())
		);
		args.GetReturnValue().Set(v8_result);
		std::stringstream().swap(ssstream);
		return;
	}
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, (double)ret)
	);
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "message"),
		sow_web_jsx::v8_str(isolate, "Success...")
	);
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "data"),
		sow_web_jsx::v8_str(isolate, ssstream.str().c_str())
	);
	args.GetReturnValue().Set(v8_result);
	std::stringstream().swap(ssstream);
	return;
}
void native_write_from_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), _body_stream, true);
	if (ret < 0 || ret == std::string::npos) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->c_str())));
	}
	delete abs_path;
	return;
}
void exists_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Directory required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string("");
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	int rec = sow_web_jsx::dir_exists(abs_path->c_str());
	abs_path->clear(); delete abs_path; abs_path = NULL;
	utf_abs_path_str.clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, (double)rec));
}
void read_directory_regx(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Match pattern required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
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
		directorys->clear(); delete directorys;
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
		);
		delete abs_path;
		args.GetReturnValue().Set(v8_result);
		return;
	}
	delete abs_path;
	std::vector<std::string>& json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set(ctx, (int)i, sow_web_jsx::v8_str(isolate, json_obj[i].c_str()));
	}
	directorys->clear(); delete directorys;
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
}
void read_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string("");
	abs_path->append(_root_dir->c_str());
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
		directorys->clear(); delete directorys;
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
		);
		args.GetReturnValue().Set(v8_result);
		delete abs_path;
		return;
	}
	delete abs_path;
	std::vector<std::string>& json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set(ctx, (int)i, sow_web_jsx::v8_str(isolate, json_obj[i].c_str()));
	}
	directorys->clear(); delete directorys;
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
}
void native_delete_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Directory required!!!");
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string();
	abs_dir->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::delete_dir(abs_dir->c_str());
	if (rec > 0)
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Success"));
	else
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Directory does not exists!!! Server absolute path==>", abs_dir->c_str())));
	delete abs_dir;
	return;

}
void native_create_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Server path required!!!");
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string();
	abs_dir->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::create_directory(abs_dir->c_str());
	if (rec > 0) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Success"));
	}
	else {
		if (rec == -1) {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Directory already exists!!! Dir#", abs_dir->c_str()));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Unknown error please retry!!! Dir#", abs_dir->c_str()));
		}
	}
	delete abs_dir;
	return;

}
//[/FileSystem]
//8:53 PM 12/6/2019
//[Asynchronous]
void async_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction()) {
		isolate->ThrowException(v8::Exception::Error(
			sow_web_jsx::v8_str(isolate, "Function required!!!")));
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
void sleep_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Sleep time Required!!!")));
		return;
	}
	//v8::Unlocker unlocker(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	int time = args[0]->Int32Value(ctx).FromMaybe(0);
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
	//	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	//	int64_t milliseconds = num->ToInteger(ctx).ToLocalChecked()->Value();
	//#if defined(_WINDOWS_)
	//	Sleep((DWORD)milliseconds);
	//#else
	//	sleep(reinterpret_cast<unsigned int*>(milliseconds));
	//#endif//!_WINDOWS_
}
void set_time_out_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Callback Required!!!")));
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
void async_t(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	async_func_arg* afa = new async_func_arg();
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	sow_web_jsx::acync_init(isolate, sow_web_jsx::async_callback, afa);
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "START"));
}
void _async_thread(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsArray() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Array required!!!")));
		return;
	}
	int rec = sow_web_jsx::async_thread(isolate, v8::Local<v8::Array>::Cast(args[0]));
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
//[/Asynchronous]
//[wkhtmltopdf]
void generate_pdf(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//11:14 PM 12/4/2018
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Value> v8_path_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "path")).ToLocalChecked();
		if (v8_path_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "Output path required!!!")));
			return;
		}
		v8::Local<v8::Value> v8_url_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "url")).ToLocalChecked();
		bool form_body = to_boolean(isolate, config->Get(ctx, sow_web_jsx::v8_str(isolate, "from_body")).ToLocalChecked());
		//v8::Local<v8::Boolean> b = v8::Local<v8::Boolean>::Cast(config->Get(ctx, sow_web_jsx::v8_str(isolate, "from_body")).ToLocalChecked());
		if (form_body == false) {
			if (!args[1]->IsString() || args[0]->IsNullOrUndefined()) {
				if (!v8_url_str->IsString() && v8_url_str->IsNullOrUndefined()) {
					config.Clear();
					isolate->ThrowException(v8::Exception::TypeError(
						sow_web_jsx::v8_str(isolate, "Body string required!!!")));
					return;
				}
			}
		}
		v8::String::Utf8Value utf8_path_str(isolate, v8_path_str);
		auto abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_path);
		std::string* dir = new std::string();
		sow_web_jsx::get_dir_from_path(*abs_path, *dir);
		if (!sow_web_jsx::dir_exists(dir->c_str())) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, dir->c_str(), " does not exists!!! Please at first create directory.")));
			delete dir; delete abs_path; config.Clear();
			return;
		}
		delete dir;
		v8::Local<v8::Value> v8_global_settings_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "global_settings")).ToLocalChecked();
		auto wgs_settings = new std::map<const char*, const char*>();
		if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
			n_help::v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
			v8_global_settings_str.Clear();
			v8_global_settings_object.Clear();
		}
		v8::Local<v8::Value> v8_object_settings_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "object_settings")).ToLocalChecked();
		auto wos_settings = new std::map<const char*, const char*>();
		if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
			n_help::v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
			v8_object_settings_str.Clear();
			v8_object_settings_object.Clear();
		}
		config.Clear();
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		//malloc(sizeof pdf_gen);
		int rec = 0;
		rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
		wgs_settings->clear(); wos_settings->clear();
		delete wgs_settings; delete wos_settings;
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_msg"),
				sow_web_jsx::v8_str(isolate, pdf_gen->get_status_msg())
			);
			args.GetReturnValue().Set(v8_result);
			v8_result.Clear();
			delete pdf_gen;
			return;
		}

		if (!form_body) {
			if (v8_url_str->IsString() && !v8_url_str->IsNullOrUndefined()) {
				v8::String::Utf8Value utf8_url_str(isolate, v8_url_str);
				rec = pdf_gen->generate_from_url(*utf8_url_str, abs_path->c_str());
			}
			else {
				v8::String::Utf8Value utf8_body_str(isolate, args[1]);
				rec = pdf_gen->generate_to_path(*utf8_body_str, abs_path->c_str());
			}

		}
		else {
			auto str = new std::string(_body_stream.str());
			rec = pdf_gen->generate_to_path(str->c_str(), abs_path->c_str());
			delete str;
		}
		delete abs_path;
		if (rec > 0) {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_msg"),
				sow_web_jsx::v8_str(isolate, "Success")
			);
		}
		else {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_msg"),
				sow_web_jsx::v8_str(isolate, pdf_gen->get_status_msg())
			);
		}
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		delete pdf_gen;
		return;
	}
	catch (std::exception& e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
	}
}
void generate_pdf_from_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (n_help::write_http_status(*_http_status, true) < 0) {
			args.GetReturnValue().Set(v8::Number::New(isolate, -1));
			return;
		}
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		int rec = 0;
		if (args[0]->IsObject() && !args[0]->IsNullOrUndefined()) {
			v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
			v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
			v8::Local<v8::Value> v8_path_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "path")).ToLocalChecked();
			if (!v8_path_str->IsNullOrUndefined()) {
				isolate->ThrowException(v8::Exception::Error(
					sow_web_jsx::v8_str(isolate, "You should not set output path here!!!")));
				return;
			}
			v8::Local<v8::Value> v8_url_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "url")).ToLocalChecked();
			if (!v8_url_str->IsNullOrUndefined()) {
				isolate->ThrowException(v8::Exception::Error(
					sow_web_jsx::v8_str(isolate, "You should not set reading url here!!!")));
				return;
			}
			v8::Local<v8::Value> v8_global_settings_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "global_settings")).ToLocalChecked();
			auto wgs_settings = new std::map<const char*, const char*>();
			if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
				v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
				n_help::v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
				v8_global_settings_str.Clear();
				v8_global_settings_object.Clear();
			}
			v8::Local<v8::Value> v8_object_settings_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "object_settings")).ToLocalChecked();
			auto wos_settings = new std::map<const char*, const char*>();
			if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
				v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
				n_help::v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
				v8_object_settings_str.Clear();
				v8_object_settings_object.Clear();
			}
			config.Clear();
			rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
			wgs_settings->clear(); wos_settings->clear();
			delete wgs_settings; delete wos_settings;
		}
		else {
			rec = pdf_gen->init(true);
		}
		if (rec < 0) {
			std::stringstream().swap(_body_stream);
			_body_stream << pdf_gen->get_status_msg();
			args.GetReturnValue().Set(v8::Number::New(isolate, rec));
			pdf_gen->dispose();
			delete pdf_gen;
			return;
		}

		std::string* str = new std::string(_body_stream.str());
		std::stringstream().swap(_body_stream);
		std::string str_output;
		rec = pdf_gen->generate(str->c_str(), str_output);
		str->clear(); delete str;
		if (rec < 0) {
			_body_stream << pdf_gen->get_status_msg();
			args.GetReturnValue().Set(v8::Number::New(isolate, rec));
			pdf_gen->dispose();
			delete pdf_gen;
			return;
		}
		//body_stream << "\r\n";
		_body_stream << str_output;
		std::string().swap(str_output);
		//body_stream << "\r\n\r\n";
		n_help::add_header(*_headers, "wkhtmltopdf_version", pdf_gen->version);
		pdf_gen->dispose();
		delete pdf_gen;
		n_help::add_header(*_headers, "Accept-Ranges", "bytes");
		n_help::add_header(*_headers, "Content-Type", "application/pdf");
		//n_help::add_header(*_headers, "Content-Disposition", "attachment;filename=\"auto.pdf\"");
		//n_help::add_header(*_headers, "Content-Type", "application/octet-stream");
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		//delete data; 
	}
	catch (std::runtime_error& e) {
		std::stringstream().swap(_body_stream);
		_body_stream << "PDF Generation failed!!!\r\n";
		_body_stream << "ERROR==>" << e.what();
		args.GetReturnValue().Set(v8::Number::New(isolate, -1));
	}
	return;
};
//[/wkhtmltopdf]
//2:08 AM 11/25/2019
void smtp_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Object required!!!")));
			return;
		}
		smtp_client::smtp_request* smtp = new smtp_client::smtp_request();
		if (smtp->has_error()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, smtp->get_last_error())));
			delete smtp;
			return;
		}
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_val;
		if (_is_cli == false) {
			smtp->read_debug_information(false);
		}
		else {
			v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_debug")).ToLocalChecked();
			if (v8_val->IsBoolean()) {
				smtp->read_debug_information(sow_web_jsx::to_boolean(isolate, v8_val));
			}
			else {
				smtp->read_debug_information(true);
			}
		}
		v8::Local<v8::Value> smtp_user = config->Get(ctx, sow_web_jsx::v8_str(isolate, "user")).ToLocalChecked();
		if (smtp_user->IsNullOrUndefined() || !smtp_user->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP User required...")));
			delete smtp;
			return;
		}
		v8::Local<v8::Value> smtp_pwd = config->Get(ctx, sow_web_jsx::v8_str(isolate, "password")).ToLocalChecked();
		if (smtp_pwd->IsNullOrUndefined() || !smtp_pwd->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP password required...")));
			delete smtp;
			return;
		}
		smtp->credentials(
			sow_web_jsx::to_char_str(isolate, smtp_user),
			sow_web_jsx::to_char_str(isolate, smtp_pwd)
		);
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "host")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP Host required...")));
			delete smtp;
			return;
		}
		smtp->host(sow_web_jsx::to_char_str(isolate, v8_val));
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "tls")).ToLocalChecked();
		if (v8_val->IsBoolean() && v8_val->IsTrue()) {
			smtp->enable_tls_connection();
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_verify_ssl")).ToLocalChecked();
		if (v8_val->IsBoolean()) {
			smtp->verify_ssl(sow_web_jsx::to_boolean(isolate, v8_val));
		}
		else {
			smtp->verify_ssl(true);
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_verify_ssl_host")).ToLocalChecked();
		if (v8_val->IsBoolean()) {
			smtp->verify_ssl_host(sow_web_jsx::to_boolean(isolate, v8_val));
		}
		else {
			smtp->verify_ssl_host(true);
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_http_auth")).ToLocalChecked();
		if (v8_val->IsBoolean()) {
			smtp->http_auth(sow_web_jsx::to_boolean(isolate, v8_val));
		}
		else {
			smtp->http_auth(true);
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "cert_path")).ToLocalChecked();
		if (v8_val->IsString()) {
			smtp->set_server_cert(sow_web_jsx::to_char_str(isolate, v8_val));
		}
		smtp->set_date_header();
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "to")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP To required...")));
			delete smtp;
			return;
		}
		smtp->mail_to(sow_web_jsx::to_char_str(isolate, v8_val));
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "from")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP From required...")));
			delete smtp;
			return;
		}
		smtp->mail_from(sow_web_jsx::to_char_str(isolate, v8_val));
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "cc")).ToLocalChecked();
		if (!v8_val->IsNullOrUndefined() && v8_val->IsString()) {
			smtp->mail_cc(sow_web_jsx::to_char_str(isolate, v8_val));
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "bcc")).ToLocalChecked();
		if (!v8_val->IsNullOrUndefined() && v8_val->IsString()) {
			smtp->mail_bcc(sow_web_jsx::to_char_str(isolate, v8_val));
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "mail_domain")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP Domain required...")));
			delete smtp;
			return;
		}
		else {
			smtp->set_message_id(sow_web_jsx::to_char_str(isolate, v8_val));
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "subject")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			smtp->mail_subject("");
		}
		else {
			smtp->mail_subject(sow_web_jsx::to_char_str(isolate, v8_val));
		}
		smtp->prepare();
		if (smtp->has_error()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, smtp->get_last_error())));
			delete smtp;
			return;
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "attachments")).ToLocalChecked();
		if (!v8_val->IsNullOrUndefined() && v8_val->IsArray()) {
			v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_val);
			for (uint32_t i = 0, l = harr->Length(); i < l; i++) {
				v8::Local<v8::Value>v_val = harr->Get(ctx, i).ToLocalChecked();
				if (v_val->IsNullOrUndefined() || !v_val->IsObject())continue;
				v8::Local<v8::Object> def = v8::Handle<v8::Object>::Cast(v_val);
				smtp->add_attachment(
					sow_web_jsx::to_char_str(isolate, def->Get(ctx, sow_web_jsx::v8_str(isolate, "name")).ToLocalChecked()),
					sow_web_jsx::to_char_str(isolate, def->Get(ctx, sow_web_jsx::v8_str(isolate, "mim_type")).ToLocalChecked()),
					sow_web_jsx::to_char_str(isolate, def->Get(ctx, sow_web_jsx::v8_str(isolate, "path")).ToLocalChecked()),
					sow_web_jsx::to_char_str(isolate, def->Get(ctx, sow_web_jsx::v8_str(isolate, "encoder")).ToLocalChecked())
				);
				def.Clear(); v_val.Clear();
			}
			harr.Clear(); v8_val.Clear();
		}
		if (smtp->has_error()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, smtp->get_last_error())));
			delete smtp;
			return;
		}
		bool isHtml = false;
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_html")).ToLocalChecked();
		if (!v8_val->IsNullOrUndefined() && v8_val->IsBoolean()) {
			isHtml = sow_web_jsx::to_boolean(isolate, v8_val);
		}
		v8_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "body")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "SMTP Body required...")));
			delete smtp;
			return;
		}
		//v8::String::Utf8Value str(isolate, v8_val);
		native_string body(isolate, v8_val);
		//std::string body(*str);
		//smtp->test_mail(
		//	/*body*/body.c_str(),
		//	/*is_html*/isHtml
		//);
		smtp->send_mail(
			/*body*/body.c_str(),
			/*is_html*/isHtml
		);
		if (smtp->has_error()) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, smtp->get_last_error())));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Success"));
		}
		body.clear();
		v8_val.Clear(); config.Clear();
		delete smtp;
	}
	catch (std::runtime_error& e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
		return;
	}
	catch (...) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Unknown!!!"));
	}
}
void http_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_url_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "url")).ToLocalChecked();
		if (v8_url_str->IsNullOrUndefined() || !v8_url_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Url should be string!!!")));
			return;
		}
		v8::Local<v8::Value> v8_method_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "method")).ToLocalChecked();
		if (v8_method_str->IsNullOrUndefined() || !v8_method_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Method should be string!!!")));
			return;
		}
		v8::String::Utf8Value utf_method_str(isolate, v8_method_str);
		std::string method(*utf_method_str);
		v8::Local<v8::Value>v8_payload_val = config->Get(ctx, sow_web_jsx::v8_str(isolate, "body")).ToLocalChecked();
		if (method == "POST") {
			if (v8_payload_val->IsNullOrUndefined() || !v8_payload_val->IsString()) {
				isolate->ThrowException(v8::Exception::TypeError(
					sow_web_jsx::v8_str(isolate, "Payload required for POST Request!!!")));
				return;
			}
		}
		v8::String::Utf8Value utf_url_str(isolate, v8_url_str);
		v8::Local<v8::Value> v8_follow_location = config->Get(ctx, sow_web_jsx::v8_str(isolate, "follow_location")).ToLocalChecked();
		bool follow_location = true;
		if (v8_follow_location->IsBoolean()) {
			follow_location = sow_web_jsx::to_boolean(isolate, v8_follow_location);
		}
		http_client::http_request* http = new http_client::http_request(*utf_url_str, follow_location);
		v8_url_str.Clear();
		v8::Local<v8::Value>v8_header_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "header")).ToLocalChecked();
		if (!v8_header_str->IsNullOrUndefined() && v8_header_str->IsArray()) {
			v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_header_str);
			for (uint32_t i = 0, l = harr->Length(); i < l; i++) {
				v8::Local<v8::Value>v8_val = harr->Get(ctx, i).ToLocalChecked();
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key(isolate, v8_val);
				http->set_header(*key);
			}
			v8_header_str.Clear();
		}
		v8::Local<v8::Value>v8_cookie_str = config->Get(ctx, sow_web_jsx::v8_str(isolate, "cookie")).ToLocalChecked();
		if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsString()) {
			v8::String::Utf8Value utf_cook_str(isolate, v8_cookie_str);
			http->set_cookie(*utf_cook_str);
		}
		else if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsArray()) {
			v8::Local<v8::Array> carr = v8::Local<v8::Array>::Cast(v8_cookie_str);
			std::string* cookies = new std::string();
			for (uint32_t i = 0, l = carr->Length(); i < l; i++) {
				v8::Local<v8::Value>v8_val = carr->Get(ctx, i).ToLocalChecked();
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key(isolate, v8_val);
				if (i == 0) {
					cookies->append(*key); continue;
				}
				cookies->append("; ");
				cookies->append(*key);
			}
			http->set_cookie(cookies->c_str());
			cookies->clear(); delete cookies;
			v8_cookie_str.Clear();
		}
		if (_is_cli == false) {
			http->read_debug_information(false);
		}
		else {
			v8::Local<v8::Value> v8_isDebug = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_debug")).ToLocalChecked();
			if (v8_isDebug->IsBoolean()) {
				http->read_debug_information(sow_web_jsx::to_boolean(isolate, v8_isDebug));
			}
			else {
				http->read_debug_information(true);
			}
		}

		v8::Local<v8::Value> v8_v_ssl = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_verify_ssl")).ToLocalChecked();
		if (v8_v_ssl->IsBoolean()) {
			http->verify_ssl(sow_web_jsx::to_boolean(isolate, v8_v_ssl));
		}
		else {
			http->verify_ssl(true);
		}
		v8::Local<v8::Value> v8_v_ssl_host = config->Get(ctx, sow_web_jsx::v8_str(isolate, "is_verify_ssl_host")).ToLocalChecked();
		if (v8_v_ssl_host->IsBoolean()) {
			http->verify_ssl_host(sow_web_jsx::to_boolean(isolate, v8_v_ssl_host));
		}
		else {
			http->verify_ssl_host(true);
		}
		std::string* resp_header = new std::string();
		std::string* resp_body = new std::string();
		int rec = 0;
		if (method == "POST") {
			native_string payload_str(isolate, v8_payload_val);
			rec = http->post(payload_str.c_str(), *resp_header, *resp_body);
			v8_payload_val.Clear(); payload_str.clear();
		}
		else {
			rec = http->get(*resp_header, *resp_body);
		}
		config.Clear();
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_msg"),
				sow_web_jsx::v8_str(isolate, http->get_last_error())
			);
		}
		else {
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "ret_msg"),
				sow_web_jsx::v8_str(isolate, "success")
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "response_header"),
				sow_web_jsx::v8_str(isolate, resp_header->c_str())
			);
			v8_result->Set(
				ctx,
				sow_web_jsx::v8_str(isolate, "response_body"),
				sow_web_jsx::v8_str(isolate, resp_body->c_str())
			);
		}
		args.GetReturnValue().Set(v8_result);
		delete http; resp_header->clear(); delete resp_header; resp_body->clear(); delete resp_body;
		v8_result.Clear();
		return;
	}
	catch (std::runtime_error& e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
		return;
	}
	catch (...) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Unknown!!!"));
	}

}
void response_redirect(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
#if defined(FAST_CGI_APP)
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Redirect location required!!!")));
		return;
	}
	auto desc = new std::string("Status: 303 See Other");
	desc->append(H_N_L);
	native_string url_str(isolate, args[0]);
#else
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Redirect location required!!!")));
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
	desc->clear(); delete desc; url_str.clear();
}
//[Encryption/Decryption]
void encrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	try {
		auto cipher = new Cipher(1000);
		std::string encrypted_text = cipher->encrypt(utf_soruce_str.c_str());
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, 1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "data"),
			sow_web_jsx::v8_str(isolate, encrypted_text.c_str())
		);
		args.GetReturnValue().Set(v8_result);
		std::string().swap(encrypted_text);
		v8_result.Clear(); utf_soruce_str.clear();
		return;
	}
	catch (std::runtime_error& e) {
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "msg"),
			sow_web_jsx::v8_str(isolate, e.what())
		);
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		return;
	}
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
}
void decrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	try {
		auto cipher = new Cipher(-1000);
		std::string decrypted_text = cipher->encrypt(utf_soruce_str.c_str());
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, 1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "data"),
			sow_web_jsx::v8_str(isolate, decrypted_text.c_str())
		);
		args.GetReturnValue().Set(v8_result);
		std::string().swap(decrypted_text);
		v8_result.Clear(); utf_soruce_str.clear();
		return;
	}
	catch (std::runtime_error& e) {
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			ctx,
			sow_web_jsx::v8_str(isolate, "msg"),
			sow_web_jsx::v8_str(isolate, e.what())
		);
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		return;
	}
#else
	isolate->ThrowException(v8::Exception::Error(
		sow_web_jsx::v8_str(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
}
void generate_key_iv(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string key, iv, error;
	if (crypto::generate_key_iv(key, iv, error) == FALSE) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, error.c_str())));
		error.clear();
		return;
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "key"),
		sow_web_jsx::v8_str(isolate, key.c_str())
	);
	v8_result->Set(
		ctx,
		sow_web_jsx::v8_str(isolate, "iv"),
		sow_web_jsx::v8_str(isolate, iv.c_str())
	);
	args.GetReturnValue().Set(v8_result);
	key.clear(); iv.clear();
}
void base64_encode(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Plain text required!!!")));
		return;
	}
	native_string utf_plain_text(isolate, args[0]);
	std::string bas64str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(utf_plain_text.c_str()), (int)utf_plain_text.size());
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, bas64str.c_str()));
	bas64str.clear(); utf_plain_text.clear();
}
void base64_decode(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Plain text required!!!")));
		return;
	}
	native_string utf_base64_text(isolate, args[0]);
	std::string plain_str = sow_web_jsx::base64::to_decode_str(utf_base64_text.c_str());
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, plain_str.c_str()));
	plain_str.clear(); utf_base64_text.clear();
}
void encrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();

	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Base64 KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Base64 IV required!!!")));
		return;
	}
	native_string utf_plain_text(isolate, args[0]);
	native_string utf_key(isolate, args[1]);
	native_string utf_iv(isolate, args[2]);
	try {
		std::stringstream dest(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		int rec = crypto::encrypt(utf_plain_text.c_str(), utf_key.c_str(), utf_iv.c_str(), dest);
		utf_key.clear(); utf_iv.clear();
		if (rec == FALSE) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::concat_msg(isolate, "Unable to encrypt plain text.==>", dest.str().c_str())));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, dest.str().c_str()));
		}
		utf_plain_text.clear();
		dest.clear(); std::stringstream().swap(dest);
	}
	catch (std::runtime_error& e) {
		isolate->ThrowException(v8::Exception::Error(
			sow_web_jsx::v8_str(isolate, e.what())));
	}
}
void decrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "IV required!!!")));
		return;
	}
	native_string utf_encrypted_text(isolate, args[0]);
	native_string utf_key(isolate, args[1]);
	native_string utf_iv(isolate, args[2]);
	try {
		std::stringstream dest(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		int rec = crypto::decrypt(utf_encrypted_text.c_str(), utf_key.c_str(), utf_iv.c_str(), dest);
		utf_key.clear(); utf_iv.clear();
		if (rec == FALSE) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::concat_msg(isolate, "Unable to decrypt encrypted text.==>", dest.str().c_str())));
		}
		else {
			args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, dest.str().c_str(), v8::NewStringType::kNormal).ToLocalChecked());
		}
		utf_encrypted_text.clear();
		dest.clear(); std::stringstream().swap(dest);
	}
	catch (std::runtime_error& e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
	}
}
void hex_to_string(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(sow_web_jsx::v8_str(isolate, "Hex string required!!!")));
		return;
	}
	native_string utf_hex_str(isolate, args[0]);
	std::string* hex_str = new std::string(utf_hex_str.c_str());
	std::string* plain_str = new std::string("");
	crypto::hex_to_string(*hex_str, *plain_str);
	hex_str->clear(); delete hex_str; utf_hex_str.clear();
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, plain_str->c_str()));
	plain_str->clear(); delete plain_str;
	return;
}
void string_to_hex(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(sow_web_jsx::v8_str(isolate, "Plain string required!!!")));
		return;
	}
	native_string utf_plain_str(isolate, args[0]);
	std::string* plain_str = new std::string(utf_plain_str.c_str());
	std::string* hex_str = new std::string("");
	crypto::string_to_hex(*plain_str, *hex_str);
	plain_str->clear(); delete plain_str; utf_plain_str.clear();
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, hex_str->c_str()));
	hex_str->clear(); delete hex_str;
	return;
}
//[/Encryption/Decryption]
//12:09 PM 8/27/2019
void set_cookie(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "HTTP Cookie required!!!")));
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
void http_status(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
#if defined(FAST_CGI_APP)
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "HTTP Status Code Required!!!")));
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
			sow_web_jsx::v8_str(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "HTTP Status Code Required!!!")));
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
	desc->clear(); delete desc; status_code_str.clear();
	return;
}
void response_write_header(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Header Key string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Header Description string required!!!")));
		return;
	}
	native_string key_str(isolate, args[0]);
	native_string description_str(isolate, args[1]);
	n_help::add_header(*_headers, key_str.c_str(), description_str.c_str());
	return;
}
void response_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined())return;
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	_body_stream << *utf8_str;
	return;
}
void response_throw_error(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
void response_clear(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//v8::Isolate* isolate = args.GetIsolate();
	_body_stream.clear();
	std::stringstream().swap(_body_stream);
	args.GetReturnValue().Set(args.Holder());
}
void get_response_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	args.GetReturnValue().Set(sow_web_jsx::v8_str(args.GetIsolate(), _body_stream.str().c_str()));
}
void server_map_path(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, abs_path->c_str()));
	delete abs_path; utf_abs_path_str.clear();
}
size_t get_content_length() {
	_body_stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = _body_stream.tellg();
	_body_stream.seekg(0, std::ios::beg);//Back to begain of stream
	return (size_t)totalSize;
}
void __clear_cache(int clean_body = 0, int clean_root=1) {
	if (_http_status != NULL) {
		_http_status->clear(); delete _http_status; _http_status = NULL;
	}
	if (_headers != NULL) {
		_headers->clear(); delete _headers; _headers = NULL;
	}
	if (_cookies != NULL) {
		_cookies->clear(); delete _cookies; _cookies = NULL;
	}
	if (clean_root == TRUE) {
		if (_root_dir != NULL) {
			_root_dir->clear(); delete _root_dir; _root_dir = NULL;
		}
	}
	if (clean_body == TRUE) {
		if (get_content_length() > 0) {
			_body_stream.clear();
			std::stringstream().swap(_body_stream);
		}
	}
	
}
void sow_web_jsx::wrapper::clear_cache() {
	if (!_is_cli)return;
	if (_is_flush == true)return;
	__clear_cache(TRUE);
}
BOOL set_binary_output() {
	if (SET_BINARY_MODE_OUT() != -1)return TRUE;
	/*[Nothing to do when failed...]*/
	return TRUE;
}
BOOL _write_http_status() {
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
BOOL write_http_status() {
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
			/*const char* server_root*/_root_dir->c_str(),
			/*response_status status_code*/status_code,
			/*const std::string error_msg*/_body_stream.str()
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
	if (set_binary_output() == FALSE)return;
	n_help::write_header(*_headers);
	n_help::write_cookies(*_cookies);
	__clear_cache(FALSE);
	std::cout << "\r\n";
	gzip::compress_gzip(_body_stream, std::cout);
	_body_stream.clear(); std::stringstream().swap(_body_stream);
	fflush(stdout);
}
/*[/zgip]*/
const char* sow_web_jsx::wrapper::get_root_dir() {
	return _root_dir->c_str();
}
void attachment_response() {
	if (write_http_status() < 0)return;
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
	if (set_binary_output() == FALSE)return;
	n_help::write_header(*_headers);
	n_help::write_cookies(*_cookies);
	__clear_cache(FALSE);
	fflush(stdout);
	std::cout << "\r\n";
	std::copy(std::istreambuf_iterator<char>(_body_stream),
		std::istreambuf_iterator<char>(),
		std::ostream_iterator<char>(std::cout)
	);
	fflush(stdout);
	_body_stream.clear(); std::stringstream().swap(_body_stream);
	return;
}
void gzip_compressor_export(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
	//[gzip_deflate]
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		gzip::gzip_deflate* deflate = new gzip::gzip_deflate(Z_BEST_SPEED);
		if (deflate->has_error() == TRUE) {
			throw_js_error(isolate, deflate->get_last_error());
			delete deflate;
			return;
		}
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, deflate));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<gzip::gzip_deflate*>(&deflate, [](const v8::WeakCallbackInfo<gzip::gzip_deflate*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	tpl->SetClassName(sow_web_jsx::v8_str(isolate, "compress"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	//[Out Stream std::cout]
	prototype->Set(isolate, "flush_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush) {
			throw_js_error(isolate, "Headers already been flushed...");
			return;
		}
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		if (deflate->has_error() == TRUE) {
			throw_js_error(isolate, deflate->get_last_error());
			delete deflate;
			return;
		}
		if (write_http_status() == FALSE)return;
		if (set_binary_output() == FALSE)return;
		_is_flush = true;
		if (n_help::is_gzip_encoding(*_headers) == FALSE) {
			n_help::add_header(*_headers, "Content-Encoding", "gzip");
		}
		n_help::write_header(*_headers);
		n_help::write_cookies(*_cookies);
		__clear_cache(TRUE, FALSE);
		std::cout << "\r\n";
		fflush(stdout);
		deflate->write_header(std::cout);
	}));
	prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		
		native_string str(isolate, args[0]);
		const char* data = str.c_str();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		size_t ret = 0; size_t len = strlen(data);
		if (len > CHUNK) {
			std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			source_stream.write(data, len);
			ret = deflate->write(std::cout, source_stream, do_flush, FALSE);
			source_stream.clear(); std::stringstream().swap(source_stream);
		}
		else {
			ret = deflate->write(std::cout, data, do_flush);
		}
		str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		size_t ret = deflate->write_file(std::cout, *abs_path, do_flush);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		if (deflate == NULL) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		int ret = deflate->flush(std::cout);
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		deflate->f_close_file();
		deflate->write_footer(std::cout);
		delete deflate;
		fflush(stdout);
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		if (_root_dir != NULL) {
			_root_dir->clear(); delete _root_dir; _root_dir = NULL;
		}
	}));
	//[/Out Stream std::cout]
	//[File Write]
	prototype->Set(isolate, "fs_open", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		size_t ret = deflate->f_open_file(*abs_path);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "fs_write_gzip_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		deflate->f_write_header();
	}));
	prototype->Set(isolate, "fs_write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		size_t ret = deflate->f_write_file(*abs_path, do_flush);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "fs_write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		
		native_string str(isolate, args[0]);
		const char* data = str.c_str();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		size_t ret = 0; size_t len = strlen(data);
		if (len > CHUNK) {
			std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			source_stream.write(data, len);
			ret = deflate->f_write(source_stream, do_flush);
			source_stream.clear(); std::stringstream().swap(source_stream);
		}
		else {
			ret = deflate->f_write(data, do_flush);
		}
		str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "fs_flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		int ret = deflate->f_flush();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret < 0) {
			throw_js_error(isolate, deflate->get_last_error());
			return;
		}

	}));
	prototype->Set(isolate, "fs_write_gzip_footer", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		deflate->f_write_footer();
	}));
	prototype->Set(isolate, "fs_close_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		deflate->f_close_file();
	}));
	//[/File Write]
	prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
		if (deflate != NULL) {
			delete deflate; deflate = NULL;
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		}
	}));
	//[/gzip_deflate]
	//[gzip_inflate]
	v8::Local<v8::FunctionTemplate> inflate_tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		gzip::gzip_inflate* inflate = new gzip::gzip_inflate();
		if (!SET_BINARY_MODE_IN()) {/*[Nothing to do when failed...]*/}
		if (inflate->has_error() == TRUE) {
			throw_js_error(isolate, inflate->get_last_error());
			delete inflate;
			return;
		}
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, inflate));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<gzip::gzip_inflate*>(&inflate, [](const v8::WeakCallbackInfo<gzip::gzip_inflate*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	inflate_tpl->SetClassName(sow_web_jsx::v8_str(isolate, "decompress"));
	inflate_tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> inflate_prototype = inflate_tpl->PrototypeTemplate();
	//[Out Stream std::cout]
	inflate_prototype->Set(isolate, "flush_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush) {
			throw_js_error(isolate, "Headers already been flushed...");
			return;
		}
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		if (inflate->has_error() == TRUE) {
			throw_js_error(isolate, inflate->get_last_error());
			delete inflate;
			return;
		}
		if (write_http_status() == FALSE)return;
		if (set_binary_output() == FALSE)return;
		_is_flush = true;
		if (n_help::is_gzip_encoding(*_headers) == TRUE) {
			throw_js_error(isolate, "gzip deflate not allowed...");
			return;
		}
		n_help::write_header(*_headers);
		n_help::write_cookies(*_cookies);
		__clear_cache(TRUE, FALSE);
		std::cout << "\r\n";
		fflush(stdout);
	}));
	inflate_prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
	
		native_string str(isolate, args[0]);
		const char* data = str.c_str();
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		size_t ret = 0; size_t len = strlen(data);
		if (len > CHUNK) {
			std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			source_stream.write(data, len);
			ret = inflate->write(std::cout, source_stream, do_flush, FALSE);
			source_stream.clear(); std::stringstream().swap(source_stream);
		}
		else {
			ret = inflate->write(std::cout, data, do_flush);
		}
		str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	inflate_prototype->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		size_t ret = inflate->write_file(std::cout, *abs_path, do_flush);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	inflate_prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		if (inflate == NULL) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		int ret = inflate->flush(std::cout);
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		inflate->f_close_file();
		delete inflate;
		fflush(stdout);
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		if (_root_dir != NULL) {
			_root_dir->clear(); delete _root_dir; _root_dir = NULL;
		}
	}));
	//[/Out Stream std::cout]
	//[File Write]
	inflate_prototype->Set(isolate, "fs_open", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		size_t ret = inflate->f_open_file(*abs_path);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	inflate_prototype->Set(isolate, "fs_write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		size_t ret = inflate->f_write_file(*abs_path, do_flush);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	inflate_prototype->Set(isolate, "fs_write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "data required....");
			return;
		}
		int do_flush = Z_NO_FLUSH;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
				if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
					throw_js_error(isolate, "deflate::Invalid stream end request.");
					return;
				}
			}
		}
	
		native_string str(isolate, args[0]);
		const char* data = str.c_str();
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		size_t ret = 0; size_t len = strlen(data);
		if (len > CHUNK) {
			std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
			source_stream.write(data, len);
			ret = inflate->f_write(source_stream, do_flush);
			source_stream.clear(); std::stringstream().swap(source_stream);
		}
		else {
			ret = inflate->f_write(data, do_flush);
		}
		str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	inflate_prototype->Set(isolate, "fs_flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		int ret = inflate->f_flush();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret < 0) {
			throw_js_error(isolate, inflate->get_last_error());
			return;
		}

	}));
	inflate_prototype->Set(isolate, "fs_close_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		inflate->f_close_file();
	}));
	//[/File Write]
	inflate_prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
		if (inflate != NULL) {
			delete inflate; inflate = NULL;
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		}
	}));
	//[/gzip_inflate]
	v8::Local<v8::ObjectTemplate> gzip_object = v8::ObjectTemplate::New(isolate);
	gzip_object->Set(isolate, "compress", tpl);
	gzip_object->Set(isolate, "decompress", inflate_tpl);
	gzip_object->Set(isolate, "Z_NO_FLUSH", v8::Number::New(isolate, (double)Z_NO_FLUSH));
	gzip_object->Set(isolate, "Z_FINISH", v8::Number::New(isolate, (double)Z_FINISH));
	gzip_object->Set(isolate, "ZLIB_VERSION", sow_web_jsx::v8_str(isolate, ZLIB_VERSION));
	ctx->Set(isolate, "gzip", gzip_object);
}
class std_out {
public:
	std_out();
	~std_out();
	template<class _out_stream>
	size_t write(_out_stream& dest, const char* buff);
	template<class _out_stream, class _source_stream>
	size_t write(_out_stream& dest, _source_stream& source, int bypass);
	template<class _out_stream>
	size_t write_file(_out_stream& dest, const std::string file_path);
	template<class _out_stream>
	int flush(_out_stream& dest);
	int has_error();
	const char* get_last_error();
private:
	int _is_flush;
	int _is_error;
	char* _internal_error;
	void clear();
	int panic(const char* error, int error_code);
};
std_out::std_out() {
	_is_flush = FALSE; _is_error = FALSE;
	_internal_error = new char;
}
std_out::~std_out() {
	this->clear();
}
int std_out::has_error() {
	return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
}
const char* std_out::get_last_error() {
	if (_is_error == TRUE || _is_error < 0) {
		return const_cast<const char*>(_internal_error);
	}
	return "No Error Found!!!";
}
void std_out::clear() {
	if (_is_flush == TRUE)return;
	if (_internal_error != NULL) {
		delete[]_internal_error;
		_internal_error = NULL;
	}
}
int std_out::panic(const char* error, int error_code) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_is_error = error_code;
	return _is_error;
}
template<class _out_stream>
size_t std_out::write(_out_stream& dest, const char* buff) {
	if (_is_flush == TRUE)return FALSE;
	if (_is_error == TRUE)return -1;
	if (buff == NULL || (buff != NULL && buff[0] == '\0'))return TRUE;
	size_t len = strlen(buff);
	dest.write(buff, len);
	return len;
}
template<class _out_stream, class _source_stream>
size_t std_out::write(_out_stream& dest, _source_stream& source, int bypass){
	if (bypass == FALSE) {
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
	}
	source.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = source.tellg();
	size_t total_len = (size_t)totalSize;
	source.seekg(0, std::ios::beg);//Back to begain of stream
	if (total_len == std::string::npos || total_len == 0)return total_len;
	size_t read_len = 0;
	do {
		if (!source.good())break;
		char* in;
		read_len = totalSize > CHUNK ? CHUNK : totalSize;
		in = new char[read_len];
		source.read(in, read_len);
		totalSize -= read_len;
		dest.write(in, read_len);
		/* Free memory */
		delete[]in;
		if (totalSize <= 0) break;
	} while (true);
	return total_len;
}
template<class _out_stream>
size_t std_out::write_file(_out_stream& dest, const std::string file_path) {
	if (_is_flush == TRUE)return FALSE;
	if (_is_error == TRUE)return -1;
	std::ifstream file_stream(file_path.c_str(), std::ifstream::binary);
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
	/*const char* buff = H_N_L;
	dest.write(buff, strlen(buff));*/
	fflush(stdout);
	return 0;
}
void stdout_export(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
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
	tpl->SetClassName(sow_web_jsx::v8_str(isolate, "stdout"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	prototype->Set(isolate, "flush_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush) {
			throw_js_error(isolate, "Headers already been flushed...");
			return;
		}
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		if (cout->has_error() == TRUE) {
			throw_js_error(isolate, cout->get_last_error());
			delete cout;
			return;
		}
		if (write_http_status() == FALSE)return;
		if (set_binary_output() == FALSE)return;
		_is_flush = true;
		BOOL is_gzip = n_help::is_gzip_encoding(*_headers);
		/*if (n_help::is_gzip_encoding(*_headers) == TRUE) {
			throw_js_error(isolate, "gzip compresson not allowd here...");
			return;
		}*/
		n_help::write_header(*_headers);
		n_help::write_cookies(*_cookies);
		std::cout << "\r\n";
		if (is_gzip == FALSE) {
			cout->write(std::cout, _body_stream, FALSE);
		}
		__clear_cache(TRUE, FALSE);
		fflush(stdout);
	}));
	prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
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
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, cout->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
			throw_js_error(isolate, "Headers did not flushed yet...");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abs_path = new std::string();
		abs_path->append(_root_dir->c_str());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		std_out* cout = sow_web_jsx::unwrap<std_out>(args);
		size_t ret = cout->write_file(std::cout, *abs_path);
		abs_path->clear(); delete abs_path; utf_abs_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, "Stream already flashed...");
			return;
		}
		if (ret == std::string::npos || ret < 0) {
			throw_js_error(isolate, cout->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (_is_flush == false) {
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
		if (_root_dir != NULL) {
			_root_dir->clear(); delete _root_dir; _root_dir = NULL;
		}
	}));
	ctx->Set(isolate, "stdout", tpl);
}
void SetEngineInformation(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> v8_global) {
	v8::Local<v8::ObjectTemplate> js_engine_object = v8::ObjectTemplate::New(isolate);
	js_engine_object->Set(isolate, "version", v8_str(isolate, v8::V8::GetVersion()));
	js_engine_object->Set(isolate, "name", v8_str(isolate, "V8"));
	v8_global->Set(isolate, "engine", js_engine_object);
}
//9:32 PM 11/22/2018
void require(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
	bool is_encrypt = false;
	if (args[1]->IsBoolean()) {
		is_encrypt = sow_web_jsx::to_boolean(isolate, args[1]);
	}
#endif//!__client_build
	native_string utf_abs_path_str(isolate, args[0]);
	std::string* abs_path = new std::string();
	abs_path->append(_root_dir->c_str());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	std::string source_str("");
#if defined(__client_build)
	if (is_encrypt) {
		size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, true);
		delete abs_path;
		if (ret < 0 || ret == std::string::npos) {
			utf_abs_path_str.clear();
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, source_str.c_str())));
			return;
		}
		Cipher* decipher = new Cipher(-1000);
		source_str = decipher->encrypt(source_str);
		delete decipher;
	}
	else {
		size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, true);
		delete abs_path;
		if (ret < 0 || ret == std::string::npos) {
			utf_abs_path_str.clear();
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, source_str.c_str())));
			return;
		}
		//isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, std::to_string(ret).c_str())));
		//return;
	}
#else
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, true);
	abs_path->clear(); delete abs_path;
	if (ret < 0 || ret == std::string::npos) {
		utf_abs_path_str.clear();
		isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, source_str.c_str())));
		return;
	}
#endif//!__client_build
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> v8_global = sow_web_jsx::wrapper::create_v8_context_object(isolate);
	v8::Local<v8::ObjectTemplate> module_object = v8::ObjectTemplate::New(isolate);
	module_object->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8_global->Set(isolate, "module", module_object);
	v8_global->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8_global);
	v8::Context::Scope context_scope(context);
	// Compile the source code.
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, sow_web_jsx::v8_str(isolate, source_str.c_str()));
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
	v8::Local<v8::Object> modules = v8::Handle<v8::Object>::Cast(jsGlobal->Get(context, sow_web_jsx::v8_str(isolate, "module")).ToLocalChecked());
	args.GetReturnValue().Set(modules->Get(context, sow_web_jsx::v8_str(isolate, "exports")).ToLocalChecked());
	modules.Clear(); script.ToLocalChecked().Clear();
	jsGlobal.Clear(); context.Clear();
	v8_global.Clear();
	return;
}
v8::Local<v8::ObjectTemplate> sow_web_jsx::wrapper::get_context(v8::Isolate* isolate, std::map<std::string, std::map<std::string, std::string>>& ctx) {
#if defined(FAST_CGI_APP)
	_is_flush = false;
#endif//FAST_CGI_APP
	__clear_cache(1);
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
			_root_dir = new std::string(obj["root_dir"]);
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
			sow_web_jsx::v8_str(isolate, "Should not here.")));
		return;
	}));
	body_object->Set(isolate, "write_as_pdf", v8::FunctionTemplate::New(isolate, generate_pdf_from_body));
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
	_v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	/*[uWsocket]*/
	_v8_global->Set(isolate, "uws_export", v8::FunctionTemplate::New(isolate, uws_export));
	/*[/uWsocket]*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_query", v8::FunctionTemplate::New(isolate, npgsql_execute_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	_v8_global->Set(isolate, "npgsql", npgsql_object);
	npgsql_bind(isolate, _v8_global);
	/*[/NpgSql....]*/
	/*[MySQL....]*/
	mysql_bind(isolate, _v8_global);
	/*[/MySQL....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	_v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "generate_key_iv", v8::FunctionTemplate::New(isolate, generate_key_iv));
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
	/*[http_request]*/
	_v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[smtp_request]*/
	_v8_global->Set(isolate, "create_smtp_request", v8::FunctionTemplate::New(isolate, smtp_request));
	/*[/smtp_request]*/
	/*[require]*/
	_v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	_v8_global->Set(isolate, "__get_response_body", v8::FunctionTemplate::New(isolate, get_response_body));
	//
	_v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	_v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
	_v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	_v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	_v8_global->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, set_time_out_func));
	//
	_v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	/*[gzip_compressor]*/
	gzip_compressor_export(isolate, _v8_global);
	/*[/gzip_compressor]*/
	/*[stdout]*/
	stdout_export(isolate, _v8_global);
	/*[/stdout]*/
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
void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		if (itr->first == "root_dir") {
			_root_dir = new std::string(itr->second);
		}
		else if (itr->first == "is_interactive") {
			_is_interactive = itr->second == "1";
			ctx_object->Set(isolate, "is_interactive", v8::Boolean::New(isolate, _is_interactive));
			continue;
		}
		ctx_object->Set(isolate, itr->first.c_str(), v8_str(isolate, itr->second.c_str()));
	}
	/*[server_map_path]*/
	ctx_object->Set(isolate, "server_map_path", v8::FunctionTemplate::New(isolate, server_map_path));
	/*[/server_map_path]*/
	v8_global->Set(isolate, "env", ctx_object);
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_query", v8::FunctionTemplate::New(isolate, npgsql_execute_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	npgsql_bind(isolate, v8_global);
	/*[/NpgSql....]*/
	/*[MySQL....]*/
	mysql_bind(isolate, v8_global);
	/*[/MySQL....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "generate_key_iv", v8::FunctionTemplate::New(isolate, generate_key_iv));
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
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[smtp_request]*/
	v8_global->Set(isolate, "create_smtp_request", v8::FunctionTemplate::New(isolate, smtp_request));
	/*[/smtp_request]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
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
	v8_global->Set(isolate, "sys", sys_object);
	/*[uWsocket]*/
	v8_global->Set(isolate, "uws_export", v8::FunctionTemplate::New(isolate, uws_export));
	/*[/uWsocket]*/
	/*[/Sys Object]*/
	v8_global->Set(isolate, "__clear", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		delete _root_dir; _root_dir = NULL;
	}));
	//
	v8_global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!_is_interactive)return;
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument Required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			throw_js_error(isolate, "String Required!!!");
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
	/*[uWsocket]*/
	v8_global->Set(isolate, "uws_export", v8::FunctionTemplate::New(isolate, uws_export));
	/*[/uWsocket]*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_query", v8::FunctionTemplate::New(isolate, npgsql_execute_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	npgsql_bind(isolate, v8_global);
	/*[/NpgSql....]*/
	/*[MySQL....]*/
	mysql_bind(isolate, v8_global);
	/*[/MySQL....]*/
	/*[crypto]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "generate_key_iv", v8::FunctionTemplate::New(isolate, generate_key_iv));
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
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[smtp_request]*/
	v8_global->Set(isolate, "create_smtp_request", v8::FunctionTemplate::New(isolate, smtp_request));
	/*[/smtp_request]*/
	/*[server_map_path]*/
	v8_global->Set(isolate, "server_map_path", v8::FunctionTemplate::New(isolate, server_map_path));
	/*[/server_map_path]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	v8_global->Set(isolate, "is_interactive", v8::Boolean::New(isolate, _is_interactive));
	if (_is_cli == false) {
		/*[gzip_compressor]*/
		gzip_compressor_export(isolate, v8_global);
		/*[/gzip_compressor]*/
		/*[stdout]*/
		stdout_export(isolate, v8_global);
		/*[/stdout]*/
	}
	SetEngineInformation(isolate, v8_global);
	return v8_global;
}
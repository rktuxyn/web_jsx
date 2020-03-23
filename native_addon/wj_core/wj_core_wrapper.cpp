/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:32 PM 3/9/2020
#	include "wj_global.h"
#	include "wj_core_wrapper.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/wjsx_env.h>
#	include <web_jsx/base64.h>
#	include <web_jsx/n_help.h>
#	include <web_jsx/t_async.h>
#	include <web_jsx/v8_util.h>
#	include "encryption.h"
#	include "http_payload.h"
#	include "jsx_file.h"
#	include <future>
using namespace sow_web_jsx;

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
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_cli() == FALSE) {
		throw_js_error(isolate, "Not Supported!!!");
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Process absolute path required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Argument(s) required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	native_string utf_arg_str(isolate, args[1]);
	int ret = ::open_process(utf_abs_path_str.c_str(), utf_arg_str.c_str());
	if (ret < 0) {
		throw_js_error(isolate, "Not found!!!");
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
void get_req_process_info(
	v8::Local<v8::Context> ctx, 
	v8::Isolate* isolate, 
	v8::Local<v8::Object> pi, process_info& pri,
	int is_cli
) {
	get_prop_value(ctx, isolate, pi, "start_in", pri.start_in);
	if (!pri.start_in.empty())
		pri.start_in = std::regex_replace(pri.start_in, std::regex("(?:/)"), "\\");
	get_prop_value(ctx, isolate, pi, "process_name", pri.process_name);
	get_prop_value(ctx, isolate, pi, "process_path", pri.process_path);
	if (!pri.process_path.empty())
		pri.process_path = std::regex_replace(pri.process_path, std::regex("(?:/)"), "\\");
	get_prop_value(ctx, isolate, pi, "title", pri.lp_title);
	get_prop_value(ctx, isolate, pi, "arg", pri.arg);
	if (is_cli == false) {
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
//#include <stdio.h>
//#include <stdlib.h>

V8_JS_METHOD(process_open_pipe_cmd) {
	if (args.Length() < 2) {
		throw_js_error(args.GetIsolate(), "Arguments (command and callback) should not left blank...");
		return;
	}
	if (!args[0]->IsString()) {
		throw_js_type_error(args.GetIsolate(), "callback should be function...");
		return;
	}
	if (!args[1]->IsFunction()) {
		throw_js_type_error(args.GetIsolate(), "callback should be function...");
		return;
	}
	v8::Isolate* isolate = args.GetIsolate();
	native_string utf_sys_cmd(isolate, args[0]);
	FILE* pPipe;
	/* Run DIR so that it writes its output to a pipe. Open this
	 * pipe with read text attribute so that we can read it
	 * like a text file.
	 */
	if ((pPipe = _popen(utf_sys_cmd.c_str(), "rt")) == NULL) {
		throw_js_error(args.GetIsolate(), "Unable to open your command....");
		return;
	}
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	char* psBuffer = new char[128];
	/* Read pipe until end of file, or an error occurs. */
	while (fgets(psBuffer, 128, pPipe)){
		v8::Handle<v8::Value> arg[1] = {
			v8_str(isolate, psBuffer)
		};
		//v8::MaybeLocal<v8::Value>result = callback->Call(context, context->Global(), 1, arg);
		callback->Call(context, context->Global(), 1, arg);
		delete[]psBuffer;
		psBuffer = new char[128];
		/*if (!result.IsEmpty()) {
			puts(psBuffer);
		}*/
		//puts(psBuffer); delete[]psBuffer;
	}
	if (psBuffer != NULL)delete[]psBuffer;
	/* Close pipe and print return value of pPipe. */
	if (feof(pPipe)){
		printf("\nProcess returned %d\n", _pclose(pPipe));
	}
	else{
		printf("Error: Failed to read the pipe to the end.\n");
	}
}
V8_JS_METHOD(console_cmd) {
	if (args.Length() <= 0) {
		throw_js_error(args.GetIsolate(), "Argument should not left blank...");
		return;
	}
	v8::Isolate* isolate = args.GetIsolate();
	native_string utf_sys_cmd(isolate, args[0]);
	int ret = ::system(utf_sys_cmd.c_str()); utf_sys_cmd.clear();
	if (ret < 0) {
		throw_js_error(args.GetIsolate(), "Input stream not available...");
		return;
	}
	args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
}
///Kill any open process by name e.g. web_jsx.exe
///@param process_name e.g. web_jsx.exe
///@throws Permission denied
///@returns {-1|0}
V8_JS_METHOD(native_kill_process_by_name) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "App name required!!!");
		return;
	}
	native_string process_name(isolate, args[0]);
	int ret = ::kill_process_by_name(process_name.c_str());
	args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
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
		throw_js_type_error(isolate, "Process info required!!!");
		return;
	}
	process_info pri;// = new process_info();
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> pi = v8::Handle<v8::Object>::Cast(args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	int is_cli = wj_env->is_cli();
	get_req_process_info(ctx, isolate, pi, pri, is_cli);
	int ret = 0;
	if (is_cli == false) {
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
			swap_obj(ptype);
			ret = sow_web_jsx::create_process(&pri);
		}
		else {
			pri.wait_for_exit = -1;
			v8::Persistent<v8::Function> cb;
			cb.Reset(isolate, v8::Local<v8::Function>::Cast(js_reader));
			v8::Local<v8::Object>global = args.Holder();
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
			ret = sow_web_jsx::read_child_process(&pri, [&](size_t i, const char* buff) {
				v8::Handle<v8::Value> arg[2] = {
					v8::Number::New(isolate, static_cast<double>(i)),
					v8_str(isolate, buff)
				};
				callback->Call(ctx, global, 2, arg);
			});
		}
	}
	swap_obj(pri.start_in); swap_obj(pri.process_name);
	swap_obj(pri.process_path); swap_obj(pri.lp_title);
	swap_obj(pri.arg);
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
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_cli() == FALSE) {
		throw_js_type_error(isolate, "Not Supported!!!");
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Process absolute path required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Argument(s) required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	std::string process_path = utf_abs_path_str.c_str();
	native_string arg(isolate, args[1]);
	//std::string arg = *utf_arg_str;
	process_path = std::regex_replace(process_path, std::regex("(?:/)"), "\\");
	long ret = sow_web_jsx::create_child_process(process_path.c_str(), arg.c_str());
	arg.clear(); swap_obj(process_path); utf_abs_path_str.clear();
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
		throw_js_type_error(isolate, "PID Required!!!");
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
		throw_js_type_error(isolate, "PID Required!!!");
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	//v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	uint64_t pid = args[0]->ToInteger(ctx).ToLocalChecked()->Value();//num->ToInteger(isolate)->Value();
	int rec = 0;
#if defined(_WIN32)||defined(_WIN64)
	rec = ::process_is_running((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Integer::New(isolate, rec));
}
V8_JS_METHOD(get_total_handled_req) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_cli() == TRUE) {
		throw_js_error(isolate, "Unsupported method....");
		return;
	}
	int rec = wj_env->get_total_handled_req();
	args.GetReturnValue().Set(v8::Integer::New(isolate, rec));
}
V8_JS_METHOD(native_current_process_id) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	int rec = wj_env->get_app_thread_id();
	if (rec == 0)
		rec = (int)sow_web_jsx::current_process_id();
	args.GetReturnValue().Set(v8::Integer::New(isolate, rec));
}
V8_JS_METHOD(native_current_thread_id) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_cli() == TRUE) {
		throw_js_error(isolate, "Unsupported method....");
		return;
	}
	int rec = wj_env->get_my_thread_id();
	if (rec == 0)
		rec = (int)sow_web_jsx::current_process_id();
	args.GetReturnValue().Set(v8::Integer::New(isolate, rec));
}
//[FileSystem]
V8_JS_METHOD(native_exists_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::string* abs_path = new std::string(wj_env->get_root_dir());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	int ret = 0;
	if (__file_exists(abs_path->c_str()) == false) ret = 1;
	_free_obj(abs_path); utf_abs_path_str.clear();
	args.GetReturnValue().Set(v8::Boolean::New(isolate, ret > 0));
}
V8_JS_METHOD(native_write_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File data required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	native_string utf_data_str(isolate, args[1]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_path = new std::string(wj_env->get_root_dir());
	::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	int ret = ::write_file(abs_path->c_str(), utf_data_str.c_str(), utf_data_str.size());
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	if (ret == FALSE) {
		v8_result->Set(ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, -1));
		v8_result->Set(ctx, v8_str(isolate, "message"), ::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path->c_str()));
	}
	else {
		v8_result->Set(ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, ret));
		v8_result->Set(ctx, v8_str(isolate, "message"), v8_str(isolate, "Success..."));
	}
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear(); _free_obj(abs_path);
	utf_abs_path_str.clear(); utf_data_str.clear();
}
V8_JS_METHOD(native_read_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_path = new std::string(wj_env->get_root_dir());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), ssstream, true);
	_free_obj(abs_path); utf_abs_path_str.clear();
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (is_error_code(ret) == TRUE) {
		v8_result->Set(
			ctx, v8_str(isolate, "staus_code"), v8::Number::New(isolate, ret == std::string::npos ? -1 : (double)ret)
		);
		v8_result->Set(
			ctx, v8_str(isolate, "message"), v8_str(isolate, ssstream.str().c_str())
		);
		args.GetReturnValue().Set(v8_result);
		swap_obj(ssstream);
		return;
	}
	v8_result->Set(
		ctx, v8_str(isolate, "staus_code"), v8::Number::New(isolate, (double)ret)
	);
	v8_result->Set(
		ctx, v8_str(isolate, "message"), v8_str(isolate, "Success...")
	);
	v8_result->Set(
		ctx, v8_str(isolate, "data"), v8_str(isolate, ssstream.str().c_str())
	);
	args.GetReturnValue().Set(v8_result);
	swap_obj(ssstream);
	return;
}
V8_JS_METHOD(native_write_from_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_path = new std::string(wj_env->get_root_dir());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	size_t ret = ::read_file(abs_path->c_str(), wj_env->body());
	utf_abs_path_str.clear();
	if (is_error_code(ret) == TRUE) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->c_str())));
	}
	_free_obj(abs_path);
	return;
}
V8_JS_METHOD(exists_directory) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Directory required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::string* abs_path = new std::string(wj_env->get_root_dir());
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
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Match pattern required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::string* abs_path = new std::string(wj_env->get_root_dir());
	sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	native_string utf_ext_str(isolate, args[1]);
	auto reg = new std::regex(utf_ext_str.c_str());//"(html|aspx|jsx|php)"
	rec = ::read_directory_sub_directory_x(abs_path->c_str(), *directorys, *reg);
	delete reg; utf_ext_str.clear(); utf_abs_path_str.clear();
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (rec == EXIT_FAILURE) {
		_free_obj(directorys);
		v8_result->Set(
			ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, -1)
		);
		v8_result->Set(
			ctx, v8_str(isolate, "message"), ::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
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
		ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, 1)
	);
	v8_result->Set(
		ctx, v8_str(isolate, "dir"), directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
}
V8_JS_METHOD(read_directory) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::string* abs_path = new std::string(wj_env->get_root_dir());
	::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0; utf_abs_path_str.clear();
	if (args[1]->IsString() || !args[1]->IsNullOrUndefined()) {
		native_string utf_ext_str(isolate, args[1]);
		rec = ::read_directory_sub_directory(abs_path->c_str(), *directorys, utf_ext_str.c_str());
		utf_ext_str.clear();
	}
	else {
		rec = ::read_directory_sub_directory(abs_path->c_str(), *directorys, "A");
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	if (rec == EXIT_FAILURE) {
		_free_obj(directorys);
		v8_result->Set(
			ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, -1)
		);
		v8_result->Set(
			ctx, v8_str(isolate, "message"), ::concat_msg(isolate, "Could not open directory==>", abs_path->c_str())
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
		ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, 1)
	);
	v8_result->Set(
		ctx, v8_str(isolate, "dir"), directory_v8_array
	);
	args.GetReturnValue().Set(v8_result);
	directory_v8_array.Clear();
}
V8_JS_METHOD(native_delete_directory) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Directory required!!!");
		return;
	}
	native_string utf8_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_dir = new std::string(wj_env->get_root_dir());
	sow_web_jsx::get_server_map_path(utf8_path_str.c_str(), *abs_dir);
	int rec = ::delete_dir(abs_dir->c_str());
	utf8_path_str.clear();
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
	native_string utf8_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_dir = new std::string(wj_env->get_root_dir());
	::get_server_map_path(utf8_path_str.c_str(), *abs_dir);
	int rec = ::create_directory(abs_dir->c_str());
	utf8_path_str.clear();
	if (rec > 0) {
		args.GetReturnValue().Set(v8_str(isolate, "Success"));
	}
	else {
		if (rec == -1) {
			args.GetReturnValue().Set(::concat_msg(isolate, "Directory already exists!!! Dir#", abs_dir->c_str()));
		}
		else {
			args.GetReturnValue().Set(::concat_msg(isolate, "Unknown error please retry!!! Dir#", abs_dir->c_str()));
		}
	}
	_free_obj(abs_dir);
	return;

}
//[/FileSystem]
//8:53 PM 12/6/2019
typedef struct {
	v8::Persistent<v8::Function> cb;
	v8::Persistent<v8::Promise::Resolver>resolver;
	v8::Isolate* isolate;
}stdasync_func_arg;
void std_async_process(void* args) {
	auto arg = (stdasync_func_arg*)args;
	v8::Isolate* isolate = arg->isolate;
	isolate->Enter();
	v8::Locker				locker(isolate);
	v8::HandleScope			handleScope(isolate);
	v8::Isolate::Scope		isolate_scope(isolate);
	v8::Local<v8::Context>	context = isolate->GetCurrentContext();
	v8::Context::Scope		context_scope(context);
	v8::Local<v8::Object>	global = context->Global();
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, arg->cb);
	uint64_t id = 0;
	try {
		std::stringstream ss;
		ss << std::this_thread::get_id();
		id = std::stoull(ss.str());
	}
	catch (...) {
		id = -1;
	}
	v8::Handle<v8::Value> carg[1] = {
		v8::Integer::New(isolate, (int32_t)id)
	};
	//v8::Local<v8::Promise::Resolver> resolver = v8::Local<v8::Promise::Resolver>::New(isolate, arg->resolver);
	callback->Call(context, context->Global(), 1, carg);
	//v8::MaybeLocal<v8::Value>result = callback->Call(context, context->Global(), 1, carg);
	//resolver->Resolve(context, result.ToLocalChecked());
	arg->resolver.Reset();
	v8::Unlocker unlockers(isolate);
	isolate->Exit();
}
//[Asynchronous]
V8_JS_METHOD(async_func_o) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction()) {
		throw_js_type_error(isolate, "Function required!!!");
		return;
	}
	stdasync_func_arg* afa = new stdasync_func_arg();
	v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
	afa->resolver.Reset(isolate, resolver);
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	//std::future<void> async_thread = std::async(std::launch::async | std::launch::deferred, , afa);
	
	afa->isolate = std::move(isolate);
	//wj_env->set_async_thread(std_async_process, (void*)afa);
}
V8_JS_METHOD(async_func) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsFunction()) {
		throw_js_type_error(isolate, "Function required!!!");
		return;
	}
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_cli() == FALSE) {
		if (wj_env->is_thread_req() == TRUE) {
			throw_js_error(isolate, "Not Supported!!!");
			return;
		}
	}
	
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	isolate->Enter();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		try {
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			v8::Locker				locker(isolate);
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
			v8::Unlocker unlockers(isolate);
			isolate->Exit();
			try {
				std::stringstream ss;
				ss << std::this_thread::get_id();
				uint64_t id = std::stoull(ss.str());
				swap_obj(ss);
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
	//int rec = result.get();
	//cb.Reset();
	//callback.Clear();
	//args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
V8_JS_METHOD(sleep_func) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Sleep time Required!!!");
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
void _async_callback(async_func_arg* arg) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	v8::Locker				locker(isolate);
	v8::HandleScope			handleScope(isolate);
	v8::Isolate::Scope		isolate_scope(isolate);
	v8::Local<v8::Context>	context = v8::Context::New(isolate, nullptr, v8::MaybeLocal<v8::ObjectTemplate>());
	
	v8::Context::Scope		context_scope(context);
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, arg->cb);
	uint64_t id = 0;
	try {
		std::stringstream ss;
		ss << std::this_thread::get_id();
		id = std::stoull(ss.str());
	}
	catch (...) {
		id = -1;
	}

	v8::Handle<v8::Value> carg[1] = {
		v8::Integer::New(isolate, (int32_t)id)
	};
	if (arg->is_async == TRUE) {
		v8::Local<v8::Promise::Resolver> resolver = v8::Local<v8::Promise::Resolver>::New(isolate, arg->resolver);
		if (wj_env->is_thread_req() == TRUE) {
			callback->Call(context, v8::Object::New(isolate), 1, carg);
		}
		else {
			v8::MaybeLocal<v8::Value>result = callback->Call(context, context->Global(), 1, carg);
			resolver->Resolve(context, result.ToLocalChecked());
		}
		
		arg->resolver.Reset();
	}
	else {
		callback->Call(context, context->Global(), 1, carg);
	}
	if (wj_env->is_thread_req() == FALSE) {
		v8::Unlocker unlockers(isolate);
		isolate->Exit();
	}
}
V8_JS_METHOD(set_time_out_func) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_thread_req() == TRUE) {
		throw_js_error(isolate, "Not Supported!!!");
		return;
	}
	if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Callback Required!!!");
		return;
	}
	uint64_t milliseconds = 0;
	if (args[1]->IsNumber()) {
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Number> num = args[1]->ToNumber(ctx).ToLocalChecked();
		milliseconds = num->ToInteger(ctx).ToLocalChecked()->Value();
	}
	async_func_arg* afa = new async_func_arg();
	afa->is_async = FALSE;
	afa->isolate = std::move(isolate);
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	int rec = ::set_time_out(isolate, ::_async_callback, afa, milliseconds);
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
V8_JS_METHOD(async_t) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_thread_req() == TRUE) {
		throw_js_error(isolate, "Not Supported!!!");
		return;
	}
	if (!args[0]->IsFunction()) {
		throw_js_type_error(isolate, "Function required!!!");
		return;
	}
	async_func_arg* afa = new async_func_arg();
	afa->is_async = TRUE;
	v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
	afa->resolver.Reset(isolate, resolver);
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	afa->isolate = std::move(isolate);
	::acync_init(isolate, ::_async_callback, afa);
	args.GetReturnValue().Set(resolver->GetPromise());
}
V8_JS_METHOD(_async_thread) {
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env->is_thread_req() == TRUE) {
		throw_js_error(isolate, "Not Supported!!!");
		return;
	}
	if (!args[0]->IsArray() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Array required!!!");
		return;
	}
	int rec = ::async_thread(isolate, v8::Local<v8::Array>::Cast(args[0]));
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
}
//[/Asynchronous]
//2:08 AM 11/25/2019
V8_JS_METHOD(response_redirect) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Redirect location required!!!");
		return;
	}
	auto desc = new std::string("Status: 303 See Other");
	desc->append(H_N_L);
	native_string url_str(isolate, args[0]);
	desc->append("Location: ");
	desc->append(url_str.c_str());
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	n_help::add_http_status(wj_env->get_http_status(), *desc);
	_free_obj(desc); url_str.clear();
}
//[Encryption/Decryption]
V8_JS_METHOD(cipher_encrypt) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	Cipher cipher(1000);
	_NEW_STRA(out_str, utf_soruce_str.c_str());
	cipher.encrypt_decrypt(*out_str); cipher.clear();
	v8_result->Set(
		ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, 1)
	);
	v8_result->Set(
		ctx, v8_str(isolate, "data"), v8_str(isolate, out_str->c_str())
	);
	args.GetReturnValue().Set(v8_result);
	_free_obj(out_str);
	v8_result.Clear(); utf_soruce_str.clear();
	return;
}
V8_JS_METHOD(cipher_decrypt) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	native_string utf_soruce_str(isolate, args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	Cipher cipher(-1000);
	_NEW_STRA(out_str, utf_soruce_str.c_str());
	cipher.encrypt_decrypt(*out_str); cipher.clear();
	v8_result->Set(
		ctx, v8_str(isolate, "staus_code"), v8::Integer::New(isolate, 1)
	);
	v8_result->Set(
		ctx, v8_str(isolate, "data"), v8_str(isolate, out_str->c_str())
	);
	args.GetReturnValue().Set(v8_result);
	_free_obj(out_str);
	v8_result.Clear(); utf_soruce_str.clear();
	return;
}
V8_JS_METHOD(base64_encode) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Plain text required!!!");
		return;
	}
	native_string utf_plain_text(isolate, args[0]);
	//std::string bas64str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(utf_plain_text.c_str()), (int)utf_plain_text.size());
	std::string* bas64str = new std::string();
	::base64::to_encode_str(utf_plain_text.c_str(), *bas64str);
	args.GetReturnValue().Set(v8_str(isolate, bas64str->c_str()));
	_free_obj(bas64str); utf_plain_text.clear();
}
V8_JS_METHOD(base64_decode) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Plain text required!!!");
		return;
	}
	native_string utf_base64_text(isolate, args[0]);
	_NEW_STR(plain_str);
	::base64::to_decode_str(utf_base64_text.c_str(), *plain_str);
	args.GetReturnValue().Set(v8_str(isolate, plain_str->c_str()));
	_free_obj(plain_str); utf_base64_text.clear();
}
V8_JS_METHOD(hex_to_string_js) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Hex string required!!!");
		return;
	}
	native_string utf_hex_str(isolate, args[0]);
	_NEW_STRA(hex_str, utf_hex_str.c_str());
	_NEW_STR(plain_str);
	::hex_to_string(*hex_str, *plain_str);
	_free_obj(hex_str); utf_hex_str.clear();
	args.GetReturnValue().Set(v8_str(isolate, plain_str->c_str()));
	_free_obj(plain_str);
	return;
}
V8_JS_METHOD(string_to_hex_js) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Plain string required!!!");
		return;
	}
	native_string utf_plain_str(isolate, args[0]);
	_NEW_STRA(plain_str, utf_plain_str.c_str());
	_NEW_STR(hex_str);
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
		throw_js_type_error(isolate, "HTTP Cookie required!!!");
		return;
	}
	native_string resp_cookie_str(isolate, args[0]);
	const char* cook_val = resp_cookie_str.c_str();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::vector<std::string>& http_cookie = wj_env->get_http_cookies();
	std::vector<std::string>::iterator itr = std::find(http_cookie.begin(), http_cookie.end(), cook_val);
	if (itr != http_cookie.end()) {
		http_cookie.erase(itr);
	}
	http_cookie.push_back(cook_val);
}
V8_JS_METHOD(http_status) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "HTTP Status Code Required!!!");
		return;
	}
	native_string status_code_str(isolate, args[0]);
	_NEW_STRA(desc, "Status:");
	desc->append(status_code_str.c_str());
	if (args[1]->IsString() && !args[1]->IsNullOrUndefined()) {
		desc->append(" ");
		native_string dec_str(isolate, args[1]);
		desc->append(dec_str.c_str());
	}
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	n_help::add_http_status(wj_env->get_http_status(), *desc);
	_free_obj(desc); status_code_str.clear();
	return;
}
V8_JS_METHOD(response_write_header) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Header Key string required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Header Description string required!!!");
		return;
	}
	native_string key_str(isolate, args[0]);
	native_string description_str(isolate, args[1]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	n_help::add_header(wj_env->get_http_header(), key_str.c_str(), description_str.c_str());
	return;
}
V8_JS_METHOD(response_write) {
	if (args[0]->IsNullOrUndefined())return;
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	if (wj_env == NULL)return;
	native_string utf8_str(isolate, args[0]);
	wj_env->write_b(utf8_str.c_str()); utf8_str.clear();
	//wj_env->body() << utf8_str.c_str(); utf8_str.clear();
	return;
}
V8_JS_METHOD(response_throw_error) {
	if (!args[0]->IsString()) {
		throw_js_type_error(args.GetIsolate(), "Description required....");
		return;
	}
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	wj_env->clear_body(TRUE);
	//wj_env->http_status->push_back("");
	native_string utf8_str(isolate, args[0]);
	std::vector<std::string>& http_status = wj_env->get_http_status();
	wj_env->body() << "Throw Error:</br>" << utf8_str.c_str(); utf8_str.clear();
	if (!http_status.empty()) {
		http_status.clear();
	}
	if (args[1]->IsNumber()) {
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int status_code = args[1]->Int32Value(ctx).FromMaybe(0);
		response_status rs = n_help::get_http_response_status(status_code);
		if (rs == response_status::UNAUTHORIZED ||
			rs == response_status::FORBIDDEN ||
			rs == response_status::INTERNAL_SERVER_ERROR ||
			rs == response_status::NOT_IMPLEMENTED) {
			http_status.push_back(std::to_string(status_code));
			return;
		}
		throw_js_error(isolate, "Invalid Status code defined....");
		return;
	}
	else {
		http_status.push_back("500");
	}
	return;
}
V8_JS_METHOD(response_clear) {
	wjsx_env* wj_env = ::unwrap_wjsx_env(args.GetIsolate());
	wj_env->clear_body(TRUE);
	args.GetReturnValue().Set(args.Holder());
}
V8_JS_METHOD(get_response_body) {
	wjsx_env* wj_env = ::unwrap_wjsx_env(args.GetIsolate());
	args.GetReturnValue().Set(v8_str(args.GetIsolate(), wj_env->body().str().c_str()));
}
V8_JS_METHOD(server_map_path) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	auto abs_path = new std::string(wj_env->get_root_dir());
	::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
	args.GetReturnValue().Set(v8_str(isolate, abs_path->c_str()));
	_free_obj(abs_path); utf_abs_path_str.clear();
}

void extend_global_js_func(v8::Isolate* isolate, v8::Local<v8::Object>& v8_global) {
	/*[IO/file read/write....]*/
	v8::Local<v8::Object> io_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, io_object, "read_file", ::native_read_file);
	wjsx_set_method(isolate, io_object, "write_file", ::native_write_file);
	wjsx_set_method(isolate, io_object, "exists_file", ::native_exists_file);
	jsx_file_bind(isolate, io_object);
	wjsx_set_object(isolate, v8_global, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[crypto....]*/
	v8::Local<v8::Object> cipher_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, cipher_object, "encrypt", ::cipher_encrypt);
	wjsx_set_method(isolate, cipher_object, "decrypt", ::cipher_decrypt);
	wjsx_set_object(isolate, v8_global, "cipher", cipher_object);
	/*[/crypto....]*/
	/*[Base64....]*/
	v8::Local<v8::Object> base64_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, base64_object, "encode", ::base64_encode);
	wjsx_set_method(isolate, base64_object, "decode", ::base64_decode);
	wjsx_set_object(isolate, v8_global, "base64", base64_object);
	/*[/Base64....]*/
	wjsx_set_method(isolate, v8_global, "hex_to_string", ::hex_to_string_js);
	wjsx_set_method(isolate, v8_global, "string_to_hex", ::string_to_hex_js);
	/*[__async]*/
	wjsx_set_method(isolate, v8_global, "__async", ::async_func);
	/*[__async]*/
	/*[__sleep]*/
	wjsx_set_method(isolate, v8_global, "setTimeout", ::set_time_out_func);
	wjsx_set_method(isolate, v8_global, "__sleep", ::sleep_func);
	/*[__sleep]*/
	wjsx_set_method(isolate, v8_global, "__async_t", async_t);
	v8::Local<v8::Object> js_engine_object = v8::Object::New(isolate);
	v8_global->Set(isolate->GetCurrentContext(), v8_str(isolate, "version"), v8_str(isolate, v8::V8::GetVersion()));
	v8_global->Set(isolate->GetCurrentContext(), v8_str(isolate, "name"), v8_str(isolate, "V8"));
	wjsx_set_object(isolate, v8_global, "engine", v8_global);
}
v8::Local<v8::Object> create_web_ctx_obj(v8::Isolate* isolate) {
	v8::Local<v8::Object>ctx_object = v8::Object::New(isolate);
	/*[context.respons....]*/
	v8::Local<v8::Object> response_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, response_object, "_write", ::response_write);
	wjsx_set_method(isolate, response_object, "throw_error", ::response_throw_error);
	wjsx_set_method(isolate, response_object, "header", ::response_write_header);
	wjsx_set_method(isolate, response_object, "write_from_file", ::native_write_from_file);
	v8::Local<v8::Object> body_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, body_object, "flush", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		throw_js_error(args.GetIsolate(), "Should not here.");
		return;
	});
	wjsx_set_object(isolate, response_object, "body", body_object);
	wjsx_set_method(isolate, response_object, "clear", ::response_clear);
	wjsx_set_method(isolate, response_object, "_status", ::http_status);
	wjsx_set_method(isolate, response_object, "_cookie", ::set_cookie);
	wjsx_set_method(isolate, response_object, "_redirect", ::response_redirect);
	wjsx_set_method(isolate, response_object, "_as_gzip", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		wjsx_env* wj_env = ::unwrap_wjsx_env(args.GetIsolate());
		n_help::add_header(wj_env->get_http_header(), "Content-Encoding", "gzip");
		return;
	});
	ctx_object->Set(isolate->GetCurrentContext(), v8_str(isolate, "is_interactive"), v8::Boolean::New(isolate, false));
	wjsx_set_object(isolate, ctx_object, "response", response_object);
	/*[server_map_path]*/
	wjsx_set_method(isolate, ctx_object, "server_map_path", ::server_map_path);
	/*[/server_map_path]*/
	v8::Local<v8::Object> req_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, req_object, "_read_payload", ::read_payload);
	wjsx_set_method(isolate, req_object, "read_posted_file", ::read_http_posted_file);
	wjsx_set_method(isolate, req_object, "_write_file_from_payload", ::write_file_from_payload);
	wjsx_set_object(isolate, ctx_object, "request", req_object);
	return ctx_object;
}
void export_sys_js_func(v8::Isolate* isolate, v8::Handle<v8::Object> target) {
	v8::Local<v8::Object>sys_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, sys_object, "read_directory", ::read_directory);
	wjsx_set_method(isolate, sys_object, "exists_directory", ::exists_directory);
	wjsx_set_method(isolate, sys_object, "read_directory_regx", ::read_directory_regx);
	wjsx_set_method(isolate, sys_object, "create_directory", ::native_create_directory);
	wjsx_set_method(isolate, sys_object, "delete_directory", ::native_delete_directory);
	wjsx_set_method(isolate, sys_object, "create_process", ::native_create_process);
	wjsx_set_method(isolate, sys_object, "terminate_process", ::native_terminate_process);
	wjsx_set_method(isolate, sys_object, "current_process_id", ::native_current_process_id);
	wjsx_set_method(isolate, sys_object, "current_thread_id", ::native_current_thread_id);
	wjsx_set_method(isolate, sys_object, "process_is_running", ::native_process_is_running);
	wjsx_set_method(isolate, sys_object, "create_child_process", ::native_create_child_process);
	wjsx_set_method(isolate, sys_object, "open_process", ::native_open_process);
	wjsx_set_method(isolate, sys_object, "open_process_stdout", ::process_open_pipe_cmd);
	wjsx_set_method(isolate, sys_object, "kill_process_by_name", ::native_kill_process_by_name);
	wjsx_set_method(isolate, sys_object, "cmd", ::console_cmd);
	wjsx_set_method(isolate, sys_object, "gc", ::v8_gc);
	wjsx_set_method(isolate, sys_object, "async_thread", ::_async_thread);
	wjsx_set_method(isolate, sys_object, "total_handled_req", ::get_total_handled_req);
	wjsx_set_object(isolate, target, "sys", sys_object);
}
void export_wj_core(v8::Isolate* isolate, v8::Handle<v8::Object> target) {
	v8::Local<v8::Object>global_ctx = v8::Object::New(isolate);
	wjsx_set_object(isolate, global_ctx, "context", ::create_web_ctx_obj(isolate));
	v8::Local<v8::Object>global = v8::Object::New(isolate);
	::extend_global_js_func(isolate, global);
	::export_sys_js_func(isolate, global);
	wjsx_set_object(isolate, global_ctx, "global", global);
	wjsx_set_object(isolate, target, "__wj_core", global_ctx);
}
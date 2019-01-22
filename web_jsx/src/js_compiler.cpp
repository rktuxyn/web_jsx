/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "js_compiler.h"
using namespace sow_web_jsx;
using namespace sow_web_jsx::wrapper;
void sow_web_jsx::js_compiler::parse_script_x(v8::Isolate * isolate, std::map<std::string, std::map<std::string, std::string>>& ctx, template_result&tr) {
	if (isolate->IsDead()) {
		tr.is_error = true;
		tr.err_msg = "ISOLATE IS DEAD :(";
		std::string().swap(tr.t_source);
		return;
	}
	if (tr.t_source.empty()) {
		tr.is_error = true;
		tr.err_msg = "String Required!!! (Script) :(";
		std::string().swap(tr.t_source);
		return;
	}
	v8::Local<v8::String> source= v8_str(isolate, tr.t_source.data());
	// Create a new context.
	//Javascript native wrappers in V8
	//https://v8docs.nodesource.com/node-0.8/d8/d83/classv8_1_1_function_template.html
	v8::Local<v8::Context> context = sow_web_jsx::wrapper::get_context(isolate, ctx);
	v8::Context::Scope context_scope(context);
	// Compile the source code.
	v8::TryCatch try_catch(isolate);
	v8::MaybeLocal<v8::Script> cscript = v8::Script::Compile(context, source);
	if (cscript.IsEmpty()) {
		tr.is_error = true;
		tr.err_msg = "Unable to compile script. Check your script than try again.";
		tr.err_msg.append("\r\n");
		tr.err_msg.append(tr.t_source.data());
		std::string().swap(tr.t_source);
		return;
	}
	//v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
	if (try_catch.HasCaught()) {
		__set__exception(isolate, &try_catch, tr);
		tr.is_error = true;
		tr.err_msg = "Unable to compile script. Check your script than try again.";
		tr.err_msg.append("\r\n");
		tr.err_msg.append(tr.t_source.data());
		std::string().swap(tr.t_source);
		return;
	}
	std::string().swap(tr.t_source);
	// Run the script to get the result.
	cscript.ToLocalChecked()->Run(context);
	if (try_catch.HasCaught()) {
		__set__exception(isolate, &try_catch, tr);
		tr.is_error = true;
		tr.err_msg = "Unable to compile script. Check your script than try again.";
		tr.err_msg.append("\r\n");
		tr.err_msg.append(tr.t_source.data());
		std::string().swap(tr.t_source);
		return;
	}
	return;
};
void sow_web_jsx::js_compiler::parse_script_x(v8::Isolate * isolate, v8::Local<v8::String> source, const char* root_dir, const char* path_translated, const char* arg) {
	if (isolate->IsDead()) {
		std::cout << "ISOLATE IS DEAD :(";
		return;
	}
	if (source->IsUndefined() || source->IsNull()) {
		std::cout << "String Required!!! (Script) :(";
		return;
	}
	// Create a new context.
	//Javascript native wrappers in V8
	//https://v8docs.nodesource.com/node-0.8/d8/d83/classv8_1_1_function_template.html
	//get_console_context(v8::Isolate * isolate, const char * root_dir, const char * path_translated, const char * arg)
	v8::Local<v8::Context> context = sow_web_jsx::wrapper::get_console_context(isolate, root_dir, path_translated, arg);
	v8::Context::Scope context_scope(context);
	// Compile the source code.
	v8::TryCatch try_catch(isolate);
	v8::MaybeLocal<v8::Script> cscript = v8::Script::Compile(context, source);
	if (cscript.IsEmpty()) {
		std::cout << "Unable to compile script. Check your script than try again.";
		return;
	}
	//v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
	if (try_catch.HasCaught()) {
		v8::String::Utf8Value exception(isolate, try_catch.Exception());
		std::cout << *exception;
		return;
	}
	// Run the script to get the result.
	cscript.ToLocalChecked()->Run(context);
	//script->Run(context);
	if (try_catch.HasCaught()) {
		v8::String::Utf8Value exception(isolate, try_catch.Exception());
		std::cout << *exception;
		return;
	}
	return;
};
void set_v8_flag(const char* v8Flags) {
	const char* arg[3] = {
		"--sim_stack_size=20480",
		"--expose_async_hooks=--flag",
		"--trace_turbo_jt=--flag"
	};
	int* argc = new int(3);
	v8::V8::SetFlagsFromCommandLine(argc, (char**)arg, true);
	//v8::V8::SetFlagsFromString(v8Flags, (int)strlen(v8Flags));
};
jsx_export void sow_web_jsx::js_compiler::run_template_x(std::map<std::string, std::map<std::string, std::string>>&ctx, const char*exec_path, template_result& rsinf) {
	try {
		v8::V8::InitializeICUDefaultLocation(exec_path);
		//v8::V8::InitializeExternalStartupData("x64\\");
		v8::V8::InitializeExternalStartupData(exec_path);
		std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
		//set_v8_flag("--sim_stack_size=20480");
		//set_v8_flag("--expose_async_hooks=true");
		//set_v8_flag("--trace_turbo_jt=true");
		v8::V8::InitializePlatform(platform.get());
		v8::Isolate::CreateParams create_params;
		create_params.array_buffer_allocator =
			v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		v8::V8::Initialize();
		v8::Isolate* iso = v8::Isolate::New(create_params);
		//prepare_isolate("D:/Node/Projects/TASK/x64/Release/");
		v8::Isolate::Scope isolate_scope(iso);
		// Create a stack-allocated handle scope.
		v8::HandleScope handle_scope(iso);
		//v8::Local<v8::String> _source = v8_str(iso, rsinf.t_source.data());
		//std::string().swap(rsinf.t_source);
		js_compiler::parse_script_x(iso, ctx, rsinf);
		//_source.Clear();
		//iso->Dispose();
		v8::V8::Dispose();
		v8::V8::ShutdownPlatform();

	} catch (std::exception&e) {
		rsinf.err_msg = e.what();
		rsinf.is_error = true;
		return;
	}
};
//jsx_export int run_console_script_x(const char*, const char*, const char*, const char*, const char*);
jsx_export int sow_web_jsx::js_compiler::run_console_script_x(const char*exec_path, const char*script_source, const char* root_dir, const char* path_translated, const char* arg) {
	try {
		v8::V8::InitializeICUDefaultLocation(exec_path);
		//v8::V8::InitializeExternalStartupData("x64\\");
		v8::V8::InitializeExternalStartupData(exec_path);
		std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
		//set_v8_flag("--sim_stack_size=20480");
		//set_v8_flag("--expose_async_hooks=true");
		//set_v8_flag("--trace_turbo_jt=true");
		v8::V8::InitializePlatform(platform.get());
		v8::Isolate::CreateParams create_params;
		create_params.array_buffer_allocator =
			v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		v8::V8::Initialize();
		v8::Isolate* iso = v8::Isolate::New(create_params);
		//prepare_isolate("D:/Node/Projects/TASK/x64/Release/");
		v8::Isolate::Scope isolate_scope(iso);
		// Create a stack-allocated handle scope.
		v8::HandleScope handle_scope(iso);
		v8::Local<v8::String> _source = v8_str(iso, script_source);
		js_compiler::parse_script_x(iso, _source, root_dir, path_translated, arg);
		_source.Clear();
		//iso->Dispose();
		v8::V8::Dispose();
		v8::V8::ShutdownPlatform();
		return 1;
	} catch (std::exception&e) {
		std::cout << e.what();
		return -1;
	}
};
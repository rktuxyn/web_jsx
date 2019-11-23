/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "js_compiler.h"
using namespace sow_web_jsx;
using namespace sow_web_jsx::wrapper;
using namespace sow_web_jsx::js_compiler;
typedef struct {
	std::map<std::string, std::map<std::string, std::string>>*ctx;
	const char*exec_path;
	template_result* rsinf;
}wja_func_arg;
typedef struct {
	wja_func_arg*args;
	uv_async_t*asyncRequest;
	uv_work_t*work_t;
}wja_ctx;
typedef struct {
	const char*exec_path;
	const char*script_source;
	std::map<std::string, std::string>*ctx;
}cja_func_arg;
typedef struct {
	cja_func_arg*args;
	uv_async_t*asyncRequest;
	uv_work_t*work_t;
}cja_ctx;
v8_engine*_v8eng;
#if defined(FAST_CGI_APP)
//bool _is_create_context = false;
#endif//FAST_CGI_APP
int sow_web_jsx::js_compiler::run_script(std::map<std::string, std::map<std::string, std::string>>& ctx, template_result&tr) {
	v8::Isolate * isolate = _v8eng->get_current_isolate();
	if (isolate == NULL) {
		tr.is_error = true;
		tr.err_msg = "ISOLATE IS DEAD :(";
		return -1;
	}
	if (isolate->IsDead()) {
		tr.is_error = true;
		tr.err_msg = "ISOLATE IS DEAD :(";
		return -1;
	}
	// Create a new context.
	//Javascript native wrappers in V8
	//https://v8docs.nodesource.com/node-0.8/d8/d83/classv8_1_1_function_template.html
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::Context> v8_ctx		= sow_web_jsx::wrapper::get_context(isolate, ctx);
	/*if (_v8eng->has_context) {
		v8_ctx = _v8eng->get_context();
	}
	else {
		v8_ctx = sow_web_jsx::wrapper::get_context(isolate, ctx);
		_v8eng->set_context(v8_ctx);
	}*/
	
//#if !defined(FAST_CGI_APP)
//	v8::Local<v8::Context> v8_ctx = sow_web_jsx::wrapper::get_context(isolate, ctx);
//#else
//	v8::Local<v8::Context> v8_ctx;
//	if (_is_create_context == true) {
//		v8_ctx = isolate->GetCurrentContext();
//		if (v8_ctx.IsEmpty()) {
//			v8_ctx = sow_web_jsx::wrapper::get_context(isolate, ctx);
//		}
//		else {
//			sow_web_jsx::wrapper::update_global_context(isolate, v8_ctx, ctx);
//		}
//	}
//	else {
//		v8_ctx = sow_web_jsx::wrapper::get_context(isolate, ctx);
//		_is_create_context = true;
//	}
//#endif//FAST_CGI_APP
	v8::Context::Scope					context_scope(v8_ctx);
	v8::Local<v8::String>source		=	sow_web_jsx::v8_str(isolate, tr.t_source.c_str());
	if (source->IsUndefined() || source->IsNull()) {
		std::string().swap(tr.t_source);
		tr.is_error = true;
		tr.err_msg = "String Required!!! (Script) :(";
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	// Compile the source code.
	v8::MaybeLocal<v8::Script> cscript = v8::Script::Compile(v8_ctx, source);
	if (cscript.IsEmpty()) {
		tr.is_error = true;
		tr.err_msg = "Unable to compile script. Check your script than try again.\r\n";
		tr.err_msg.append(tr.t_source.c_str());
		std::string().swap(tr.t_source);
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	std::string().swap(tr.t_source);
	v8::TryCatch trycatch(isolate);
	// Run the script to get the result.
	cscript.ToLocalChecked()->Run(v8_ctx);
	if (trycatch.HasCaught()) {
		set__exception(isolate, &trycatch, tr);
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8_ctx->DetachGlobal();
	v8_ctx.Clear();
	return 1;
}
int sow_web_jsx::js_compiler::run_script_x(const char*script_source, std::map<std::string, std::string>&ctx) {
	v8::Isolate * isolate = _v8eng->get_current_isolate();
	if (isolate->IsDead()) {
		std::cout << "ISOLATE IS DEAD.\r\n";
		return -1;
	}
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::Context> v8_ctx	=	sow_web_jsx::wrapper::get_console_context(isolate, ctx);
	v8::Context::Scope					context_scope(v8_ctx);
	v8::Local<v8::String>source		=	sow_web_jsx::v8_str(isolate, script_source);
	if (source->IsUndefined() || source->IsNull()) {
		std::cout << "String Required!!! (Script)\r\n";
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8::MaybeLocal<v8::Script> cscript = v8::Script::Compile(v8_ctx, source);
	if (cscript.IsEmpty()) {
		std::cout << "Unable to compile script. Check your script than try again.\r\n";
		std::cout << "------------------------\r\n";
		std::cout << "Source:\r\n";
		std::cout << script_source;
		std::cout << "------------------------";
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8::TryCatch trycatch(isolate);
	cscript.ToLocalChecked()->Run(v8_ctx);
	if (trycatch.HasCaught()) {
		std::cout << set__exception(isolate, &trycatch);
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8_ctx->DetachGlobal();
	v8_ctx.Clear();
	return 1;
}
void set_v8_flag(const char* v8Flags) {
	const char* arg[3] = {
		"--sim_stack_size=20480",
		"--expose_async_hooks=--flag",
		"--trace_turbo_jt=--flag"
	};
	int* argc = new int(3);
	v8::V8::SetFlagsFromCommandLine(argc, (char**)arg, true);
	//v8::V8::SetFlagsFromString(v8Flags, (int)strlen(v8Flags));
}
void run_v8_async(wja_func_arg*arg) {
	template_result& rsinf = *arg->rsinf;
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(arg->exec_path);
		js_compiler::run_script(*arg->ctx, rsinf);
		//_v8eng->get_current_isolate()->LowMemoryNotification();
	} catch (std::exception&e) {
		rsinf.err_msg = e.what();
		rsinf.is_error = true;
		return;
	}
}
void sow_web_jsx::js_compiler::run_async(
	std::map<std::string, std::map<std::string, std::string>>&ctx,
	const char*exec_path,
	template_result& rsinf
) {
	wja_ctx*context = new wja_ctx();
	wja_func_arg*wjafa = new wja_func_arg();
	wjafa->ctx = &ctx;
	wjafa->exec_path = exec_path;
	wjafa->rsinf = &rsinf;
	context->args = wjafa;
	uv_loop_t *loop = uv_default_loop();
	uv_loop_init(loop);
	if (_v8eng != NULL) {
		v8::Isolate* isolate = _v8eng->get_current_isolate();
		isolate->Enter();
	}
	async__init(context, [](uv_async_t *handle) {
		wja_ctx*context = static_cast<wja_ctx*>(handle->data);
		run_v8_async(context->args);
#if defined(FAST_CGI_APP)
		v8::Isolate* isolate = _v8eng->get_current_isolate();
		//v8::Unlocker l(isolate);
		isolate->LowMemoryNotification();
		isolate->Exit();
#endif//!FAST_CGI_APP
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
#if !defined(FAST_CGI_APP)
	sow_web_jsx::wrapper::clear_cache();
	_v8eng->dispose();
	if (rsinf.is_error == true) return;
	sow_web_jsx::wrapper::response_body_flush();
	return;
#else
	if (rsinf.is_error == true) return;
	sow_web_jsx::wrapper::response_body_flush();
	return;
#endif//!FAST_CGI_APP
}
jsx_export void sow_web_jsx::js_compiler::run_script(
	std::map<std::string, std::map<std::string, std::string>>&ctx, 
	const char*exec_path, template_result& rsinf
) {
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(exec_path);
		js_compiler::run_script(ctx, rsinf);
		sow_web_jsx::wrapper::response_body_flush();
		sow_web_jsx::wrapper::clear_cache();
	} catch (std::exception&e) {
		rsinf.err_msg = e.what();
		rsinf.is_error = true;
		return;
	}
}
jsx_export void sow_web_jsx::js_compiler::create_engine(const char*exec_path) {
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
}
jsx_export void sow_web_jsx::js_compiler::dispose_engine() {
	if (_v8eng != NULL) {
		_v8eng->dispose();
		_v8eng = NULL;
	}
}
jsx_export v8::Isolate*  sow_web_jsx::js_compiler::get_isolate() {
	if (_v8eng == NULL) throw new std::runtime_error("You should not call this method before initialize engine.");
	return _v8eng->get_current_isolate();
}
int run_v8_async(cja_func_arg*arg) {
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(arg->exec_path);
		js_compiler::run_script_x(arg->script_source, *arg->ctx);
		//_v8eng->get_current_isolate()->LowMemoryNotification();
		return 1;
	} catch (std::exception&e) {
		std::cout << e.what() << "\r\n";
		std::cout << "------------------------\r\n";
		std::cout << "Source:\r\n";
		std::cout << arg->script_source;
		std::cout << "------------------------";
		return -1;
	}
}
//Served CLI Request
jsx_export void sow_web_jsx::js_compiler::run_async(
	const char*exec_path, const char*script_source, std::map<std::string, std::string>&ctx
) {
	cja_ctx*context = new cja_ctx();
	cja_func_arg*cjafa = new cja_func_arg();
	cjafa->exec_path = exec_path;
	cjafa->script_source = script_source;
	cjafa->ctx = &ctx;
	context->args = cjafa;
	uv_loop_t *loop = uv_default_loop();
	uv_loop_init(loop);
	if (_v8eng != NULL) {
		_v8eng->get_current_isolate()->Enter();
	}
	async__init(context, [](uv_async_t *handle) {
		cja_ctx*context = static_cast<cja_ctx*>(handle->data);
		run_v8_async(context->args);
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
	sow_web_jsx::wrapper::clear_cache();
	//sow_web_jsx::wrapper::response_body_flush();
	//_v8eng->get_current_isolate()->Exit();
	_v8eng->dispose();
}
jsx_export int sow_web_jsx::js_compiler::run_script(
	const char*exec_path, const char*script_source, std::map<std::string, std::string>&ctx
) {
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(exec_path);
		js_compiler::run_script_x(script_source, ctx);
		//_v8eng->get_current_isolate()->LowMemoryNotification();
		_v8eng->dispose();
		sow_web_jsx::wrapper::clear_cache();
		return 1;
	} catch (std::exception&e) {
		std::cout << e.what() << "\r\n";
		std::cout << "------------------------\r\n";
		std::cout << "Source:\r\n";
		std::cout << script_source;
		std::cout << "------------------------";
		return -1;
	}
}
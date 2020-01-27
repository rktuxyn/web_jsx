/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "js_compiler.h"
using namespace sow_web_jsx;
using namespace sow_web_jsx::wrapper;
using namespace sow_web_jsx::js_compiler;
typedef struct {
	std::map<std::string, std::map<std::string, std::string>>*ctx;
	const char*exec_path;
	template_result* rsinf;
}wja_func_arg;

typedef struct {
	const char*exec_path;
	const char*script_source;
	std::map<std::string, std::string>*ctx;
}cja_func_arg;
typedef struct {
	void* args;
	uv_async_t* asyncRequest;
	uv_work_t* work_t;
}wja_ctx;
//typedef struct {
//	void*args;
//	uv_async_t*asyncRequest;
//	uv_work_t*work_t;
//}cja_ctx;
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
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = sow_web_jsx::wrapper::get_context(isolate, ctx);
	ctx_object->Set(isolate, "runtime_compiler", v8::FunctionTemplate::New(isolate, sow_web_jsx::runtime_compiler));
	v8::Local<v8::Context> v8_ctx = v8::Context::New(isolate, nullptr, ctx_object);
	v8::Context::Scope					context_scope(v8_ctx);
	v8::Local<v8::String>source		=	v8_str(isolate, tr.t_source.c_str());
	if (source->IsUndefined() || source->IsNull()) {
		tr.t_source.clear();
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
		//tr.err_msg.append(tr.t_source.c_str());
		tr.t_source.clear();
		std::string().swap(tr.t_source);
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	tr.t_source.clear();
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
	v8_ctx.Clear(); source.Clear();
	return 1;
}
int sow_web_jsx::js_compiler::run_script_x(const char*script_source, std::map<std::string, std::string>&ctx) {
	v8::Isolate * isolate = _v8eng->get_current_isolate();
	if (isolate->IsDead()) {
		std::cout << "ISOLATE IS DEAD.\r\n";
		return -1;
	}
	//v8::Locker				locker(isolate);
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = sow_web_jsx::wrapper::get_console_context(isolate, ctx);
	ctx_object->Set(isolate, "runtime_compiler", v8::FunctionTemplate::New(isolate, sow_web_jsx::runtime_compiler));
	v8::Local<v8::Context> v8_ctx = v8::Context::New(isolate, nullptr, ctx_object/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
	v8::Context::Scope					context_scope(v8_ctx);
	v8::Local<v8::String>source		=	v8_str(isolate, script_source);
	if (source->IsUndefined() || source->IsNull()) {
		std::cout << "String Required!!! (Script)\r\n";
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8::MaybeLocal<v8::Script> cscript = v8::Script::Compile(v8_ctx, source);
	if (cscript.IsEmpty()) {
		std::cout << "Unable to compile script. Check your script than try again.\r\n";
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8::TryCatch trycatch(isolate);
	cscript.ToLocalChecked()->Run(v8_ctx);
	if (trycatch.HasCaught()) {
		/*const char* err = set__exception(isolate, &trycatch);
		std::cout << err;*/
		std::cout << "Script runtime error:\n";
		std::string err("");
		set__exception(isolate, &trycatch, err);
		std::cout << err.c_str(); err.clear();
		v8_ctx->DetachGlobal();
		v8_ctx.Clear();
		return -1;
	}
	v8_ctx->DetachGlobal();
	v8_ctx.Clear();
	return 1;
}

void run_v8_async_wja(void*args) {
	wja_func_arg* arg = (wja_func_arg*)args;
	template_result& rsinf = *arg->rsinf;
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(arg->exec_path);
		js_compiler::run_script(*arg->ctx, rsinf);
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
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	{
		v8::Isolate* isolate = _v8eng->get_current_isolate();
		isolate->Enter();
	}
	async__init(context, [](uv_async_t *handle) {
		wja_ctx*context = static_cast<wja_ctx*>(handle->data);
		run_v8_async_wja(context->args);
#if defined(FAST_CGI_APP)
		_v8eng->wait_for_pending_task();
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
#endif//!FAST_CGI_APP
	if (rsinf.is_error == true) return;
	sow_web_jsx::wrapper::response_body_flush(false);
	return;
}
jsx_export void sow_web_jsx::js_compiler::run_script(
	std::map<std::string, std::map<std::string, std::string>>&ctx, 
	const char*exec_path, template_result& rsinf
) {
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(exec_path);
		js_compiler::run_script(ctx, rsinf);
		sow_web_jsx::wrapper::response_body_flush(false);
		//sow_web_jsx::wrapper::clear_cache();
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
int run_v8_async_cja(void*args) {
	cja_func_arg* arg = (cja_func_arg*)args;
	try {
		if (_v8eng == NULL)
			_v8eng = new v8_engine(arg->exec_path);
		js_compiler::run_script_x(arg->script_source, *arg->ctx);
		return 1;
	} catch (std::exception&e) {
		std::cout << e.what() << "\r\n";
		/*std::cout << "------------------------\r\n";
		std::cout << "Source:\r\n";
		std::cout << arg->script_source;
		std::cout << "------------------------";*/
		return -1;
	}
}
//Served CLI Request
jsx_export void sow_web_jsx::js_compiler::run_async(
	const char*exec_path, const char*script_source, std::map<std::string, std::string>&ctx
) {
	wja_ctx*context = new wja_ctx();
	cja_func_arg*cjafa = new cja_func_arg();
	cjafa->exec_path = exec_path;
	cjafa->script_source = script_source;
	cjafa->ctx = &ctx;
	context->args = cjafa;
	uv_loop_t *loop = uv_default_loop();
	uv_loop_init(loop);
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	_v8eng->get_current_isolate()->Enter();
	async__init(context, [](uv_async_t *handle) {
		wja_ctx*context = static_cast<wja_ctx*>(handle->data);
		run_v8_async_cja(context->args);
		_v8eng->get_current_isolate()->Exit();
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
	sow_web_jsx::wrapper::clear_cache();
	//sow_web_jsx::wrapper::response_body_flush();
	//_v8eng->get_current_isolate()->Exit();
	//std::cout << "Waiting...." << std::endl;
	_v8eng->wait_for_pending_task();//NOW
	//std::cout << "Disposing...." << std::endl;
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
		/*std::cout << "------------------------\r\n";
		std::cout << "Source:\r\n";
		std::cout << script_source;
		std::cout << "------------------------";*/
		return -1;
	}
}
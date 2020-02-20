/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "js_compiler.h"
#	include "v8_util.h"
#	include <uv.h>
#	include "runtime_compiler.h"
#	include <libplatform/libplatform.h>
#	include "native_wrapper.h"
#	include "v8_engine.h"

using namespace sow_web_jsx;
using namespace sow_web_jsx::wrapper;
using namespace sow_web_jsx::js_compiler;

typedef struct {
	std::map<std::string, std::map<std::string, std::string>>* ctx;
	const char* exec_path;
	template_result* rsinf;
}wja_func_arg;

typedef struct {
	const char* exec_path;
	const char* script_source;
	std::map<std::string, std::string>* ctx;
}cja_func_arg;

typedef struct {
	void* args;
	uv_async_t* asyncRequest;
	uv_work_t* work_t;
}wja_ctx;

v8_engine* _v8eng;

void test(v8::Isolate* isolate1) {
	//https://chromium.googlesource.com/v8/v8.git/+/4.5.56/test/cctest/test-api.cc#21090
	const char* source_c = "Math.sqrt(4)";
	const char* origin_c = "code cache test";
	v8::ScriptCompiler::CachedData* cache;
	v8::Isolate::Scope iscope(isolate1);
	v8::HandleScope scope(isolate1);
	v8::Local<v8::Context> context = v8::Context::New(isolate1);
	v8::Context::Scope cscope(context);
	v8::Local<v8::String> source_string = v8_str(isolate1, source_c);
	v8::ScriptOrigin script_origin(v8_str(isolate1, origin_c));
	v8::ScriptCompiler::Source source(source_string, script_origin);
	v8::MaybeLocal<v8::Script> cscript = v8::ScriptCompiler::Compile(context, &source, v8::ScriptCompiler::kNoCompileOptions).ToLocalChecked();
	int length = source.GetCachedData()->length;
	unsigned char* cache_data = new unsigned char[length];
	memcpy(cache_data, source.GetCachedData()->data, length);
	//std::stringstream strm;
	//reinterpret_cast<char*>(cache_data);
	//strm.write((char*)&cache_data[0], length);
	cache = new v8::ScriptCompiler::CachedData(cache_data, length, v8::ScriptCompiler::CachedData::BufferOwned);
	delete[]cache_data;
	cscript.ToLocalChecked()->Run(context);
	/*v8::ScriptCompiler::CompileOptions option = v8::ScriptCompiler::kConsumeCodeCache;
	v8::ScriptCompiler::Source sources(source_string, script_origin, cache);*/
}
int js_compiler::run_script(
	std::map<std::string, std::map<std::string, std::string>>& ctx,
	template_result& tr
) {
	v8::Isolate* isolate = _v8eng->get_current_isolate();
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
	v8::Local<v8::String>source = v8_str(isolate, tr.t_source.c_str());
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
	//v8::internal::wasm::NativeModule
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
int js_compiler::run_script_x(
	const char* script_source, std::map<std::string, std::string>& ctx
) {
	v8::Isolate* isolate = _v8eng->get_current_isolate();
	if (isolate->IsDead()) {
		std::cout << "ISOLATE IS DEAD.\r\n";
		return -1;
	}
	//v8::Locker				locker(isolate);
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = wrapper::get_console_context(isolate, ctx);
	ctx_object->Set(isolate, "runtime_compiler", v8::FunctionTemplate::New(isolate, sow_web_jsx::runtime_compiler));
	v8::Local<v8::Context> v8_ctx = v8::Context::New(isolate, nullptr, ctx_object/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
	v8::Context::Scope					context_scope(v8_ctx);
	v8::Local<v8::String>source = v8_str(isolate, script_source);
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
	/*v8::Local<v8::UnboundScript> uscript =  cscript.ToLocalChecked()->GetUnboundScript();
	uscript->GetSourceMappingURL();
	uscript->BindToCurrentContext();
	uscript->GetId();*/
	v8::TryCatch trycatch(isolate);
	cscript.ToLocalChecked()->Run(v8_ctx);
	if (trycatch.HasCaught()) {
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

void run_v8_async_wja(void* args) {
	wja_func_arg* arg = (wja_func_arg*)args;
	template_result& rsinf = *arg->rsinf;
	if (_v8eng == NULL)
		_v8eng = new v8_engine(arg->exec_path);
	js_compiler::run_script(*arg->ctx, rsinf);
}
void js_compiler::run_async(
	std::map<std::string, std::map<std::string, std::string>>& ctx,
	const char* exec_path,
	template_result& rsinf
) {
	wja_ctx* context = new wja_ctx();
	wja_func_arg* wjafa = new wja_func_arg();
	wjafa->ctx = &ctx;
	wjafa->exec_path = exec_path;
	wjafa->rsinf = &rsinf;
	context->args = wjafa;
	uv_loop_t* loop = uv_default_loop();
	uv_loop_init(loop);
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	{
		v8::Isolate* isolate = _v8eng->get_current_isolate();
		isolate->Enter();
	}
	async__init(context, [](uv_async_t* handle) {
		wja_ctx* context = static_cast<wja_ctx*>(handle->data);
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
	wrapper::clear_cache();
	js_compiler::dispose_engine();
#endif//!FAST_CGI_APP
	if (rsinf.is_error == true) return;
	wrapper::response_body_flush(false);
	return;
}
void js_compiler::run_script(
	std::map<std::string, std::map<std::string, std::string>>& ctx,
	const char* exec_path, template_result& rsinf
) {
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	js_compiler::run_script(ctx, rsinf);
	sow_web_jsx::wrapper::response_body_flush(false);
}
void js_compiler::create_engine(const char* exec_path) {
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
}
v8::Isolate* js_compiler::get_isolate() {
	if (_v8eng == NULL) throw new std::runtime_error("You should not call this method before initialize engine.");
	return _v8eng->get_current_isolate();
}
int run_v8_async_cja(void* args) {
	cja_func_arg* arg = (cja_func_arg*)args;
	if (_v8eng == NULL)
		_v8eng = new v8_engine(arg->exec_path);
	js_compiler::run_script_x(arg->script_source, *arg->ctx);
	return TRUE;
}
//Served CLI Request
void js_compiler::run_async(
	const char* exec_path, const char* script_source, std::map<std::string, std::string>& ctx
) {
	wja_ctx* context = new wja_ctx();
	cja_func_arg* cjafa = new cja_func_arg();
	cjafa->exec_path = exec_path;
	cjafa->script_source = script_source;
	cjafa->ctx = &ctx;
	context->args = cjafa;
	uv_loop_t* loop = uv_default_loop();
	uv_loop_init(loop);
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	_v8eng->get_current_isolate()->Enter();
	async__init(context, [](uv_async_t* handle) {
		wja_ctx* context = static_cast<wja_ctx*>(handle->data);
		run_v8_async_cja(context->args);
		_v8eng->get_current_isolate()->Exit();
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
	sow_web_jsx::wrapper::clear_cache();
	_v8eng->wait_for_pending_task();
	js_compiler::dispose_engine();
}
int js_compiler::run_script(
	const char* exec_path, const char* script_source,
	std::map<std::string, std::string>& ctx
) {
	if (_v8eng == NULL)
		_v8eng = new v8_engine(exec_path);
	js_compiler::run_script_x(script_source, ctx);
	js_compiler::dispose_engine();
	sow_web_jsx::wrapper::clear_cache();
	return TRUE;
}
#include "module_store.h"
void js_compiler::dispose_engine() {
	swjsx_module::clean_native_module();
	if (_v8eng != NULL) {
		_v8eng->dispose();
		_v8eng = NULL;
	}
}
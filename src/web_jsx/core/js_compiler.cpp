/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "js_compiler.h"
#	include <uv.h>
#	include <libplatform/libplatform.h>
#	include "v8_util.h"
#	include "native_wrapper.h"
#	include "v8_engine.h"
#	include "runtime_compiler.h"
#	include "module_store.h"
//#if defined(WEB_JSX_MULTI_THREAD)
//#	include <mutex>
//#endif//!WEB_JSX_MULTI_THREAD
using namespace sow_web_jsx;
using namespace sow_web_jsx::wrapper;
using namespace sow_web_jsx::js_compiler;
enum  script_cache_type{
	kAfterExecute = 1,
	kEager = 2
};
typedef struct {
	void* ctx;
	template_result* rsinf;
	js_script_inf* jsi;
	void* wj_env;
	v8::Isolate* isolate;
	script_cache_type cache_type;
}wja_func_arg;

typedef struct {
	const char* script_path;
	const char* script_source;
	v8::Isolate* isolate;
	std::map<std::string, std::string>* ctx;
}cja_func_arg;

typedef struct {
	void* args;
	uv_async_t* asyncRequest;
	uv_work_t* work_t;
}wja_ctx;

V8_JS_METHOD(implimant_native_module) {
	wjsx_env* wj_env = ::unwrap_wjsx_env(args.GetIsolate());
	::swjsx_module::implimant_native_module(args, wj_env->get_app_dir(), wj_env->get_root_dir());
	return;
}
void js_compiler::compile_raw_script(v8::Isolate* isolate, const char* script) {
	v8::Locker							locker(isolate);
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::ObjectTemplate>		ctx_object = v8::ObjectTemplate::New(isolate);
	wjsx_assign_js_func(isolate, ctx_object, "__implimant_native_module", ::implimant_native_module);
	v8::Local<v8::Context>				context = v8::Context::New(isolate, nullptr, ctx_object);//v8::Context::New(isolate, nullptr, v8::MaybeLocal<v8::ObjectTemplate>());
	v8::Context::Scope					context_scope(context);
	v8::Local<v8::String>				source_string = v8_str(isolate, script);
	v8::MaybeLocal<v8::Script>			cscript = v8::Script::Compile(context, source_string);
	if (cscript.IsEmpty()) {
		FATAL("Unable to compile script. Check your script than try again.");
	}
	v8::TryCatch trycatch(isolate);
	cscript.ToLocalChecked()->Run(context);
	if (trycatch.HasCaught()) {
		_NEW_STR(error_str);
		set__exception(isolate, &trycatch, *error_str);
		FATAL(error_str->c_str());
	}
	v8::Unlocker unlocker(isolate);
	context->DetachGlobal(); context.Clear();
	ctx_object.Clear(); source_string.Clear();
	return;
}
void handle_eptr(std::exception_ptr eptr, template_result& rsinf){
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception & e) {
		rsinf.err_msg.clear();
		rsinf.err_msg = "Caught exception \"";
		rsinf.err_msg += e.what();
		rsinf.err_msg += "\"\n";
		std::cerr << "Caught exception \"" << e.what() << "\"\n";
	}
}
int v8_compaile_java_script(
	v8::Isolate* isolate,
	std::map <std::string, std::map<std::string, std::string>>&ctx,
	template_result& rsinf,
	wjsx_env* wj_env,
	js_script_inf& jsi
) {
	try {
		wj_env->set_mood_script();
		isolate->Enter();
		v8::Locker							locker(isolate);
		v8::Isolate::Scope					isolate_scope(isolate);
		v8::HandleScope						handle_scope(isolate);
		v8::Local<v8::Context>				v8_ctx;
		v8::Local<v8::ObjectTemplate>		ctx_object;
		ctx_object = sow_web_jsx::wrapper::get_context(isolate, ctx);
		wjsx_assign_js_func(isolate, ctx_object, "runtime_compiler", sow_web_jsx::runtime_compiler);
		wjsx_assign_js_func(isolate, ctx_object, "__implimant_native_module", ::implimant_native_module);
		v8_ctx								= v8::Context::New(isolate, nullptr, ctx_object);
		v8::Context::Scope					context_scope(v8_ctx);
		//v8_ctx->Enter();
		v8::Local<v8::String>				source_string = v8_str(isolate, rsinf.t_source.c_str());
		::swap_obj(rsinf.t_source);//Clear source data
		v8::MaybeLocal<v8::Script>			cscript = v8::Script::Compile(v8_ctx, source_string);
		if (!cscript.IsEmpty()) {
			::wrap_wjsx_env(isolate, wj_env);
			v8::TryCatch trycatch(isolate);
			cscript.ToLocalChecked()->Run(v8_ctx);
			if (trycatch.HasCaught()) {
				rsinf.is_error = TRUE;
				set__exception(isolate, &trycatch, rsinf.err_msg);
				v8::Unlocker unlocker(isolate);
				isolate->Exit();
				goto FINAL;
			}
		}
		else {
			rsinf.is_error = TRUE;
			rsinf.err_msg = ("Unable to compile script. Check your script than try again.");
			v8::Unlocker unlocker(isolate);
			isolate->Exit();
		}
		goto FINAL;

	FINAL:
		if (rsinf.is_error == FALSE) {
			v8::Unlocker unlocker(isolate);
			isolate->Exit();
		}
		v8_ctx->DetachGlobal(); v8_ctx.Clear();
		ctx_object.Clear(); source_string.Clear();
		return rsinf.is_error;
	}
	catch (...) {
		::handle_eptr(std::current_exception(), rsinf);
		wj_env->set_runtime_error(rsinf.is_error);
		return rsinf.is_error;
	}
}
int v8_compaile_java_script(
	wja_func_arg* arg
) {
	try {
		//[try Cached Data]
		_NEW_STR(cscript_path);
		int is_cached = FALSE;
		_NEW_STR(origin_str);
		wjsx_env* wj_env = reinterpret_cast<wjsx_env*>(arg->wj_env);
		int is_cli = wj_env->is_cli();
		if (wj_env->compiled_cached == TRUE) {
			if (!(is_cli == TRUE && arg->jsi->is_raw_script == TRUE)) {
				cscript_path->append(arg->jsi->abs_path); cscript_path->append("c");
				is_cached = __file_exists(cscript_path->c_str()) ? TRUE : FALSE;
				origin_str->append(arg->jsi->path_info);
				::get_script_origin(arg->jsi->path_info, *origin_str);
			}
			else {
				origin_str->append("test_raw_script");
			}
		}
		v8::Isolate::Scope					isolate_scope(arg->isolate);
		v8::HandleScope						handle_scope(arg->isolate);
		v8::Local<v8::Context>				v8_ctx;
		v8::Local<v8::ObjectTemplate>		ctx_object;
		if (is_cli == TRUE) {
			ctx_object = sow_web_jsx::wrapper::get_console_context(
				arg->isolate, *(reinterpret_cast<std::map<std::string, std::string>*>(arg->ctx))
			);
		}
		else {
			ctx_object = sow_web_jsx::wrapper::get_context(
				arg->isolate, *(reinterpret_cast<std::map<std::string, std::map<std::string, std::string>>*>(arg->ctx))
			);
		}
		wjsx_assign_js_func(arg->isolate, ctx_object, "runtime_compiler", sow_web_jsx::runtime_compiler);
		wjsx_assign_js_func(arg->isolate, ctx_object, "__implimant_native_module", ::implimant_native_module);
		v8_ctx = v8::Context::New(arg->isolate, nullptr, ctx_object);

		if (v8_ctx.IsEmpty()) {
			_free_obj(cscript_path); _free_obj(origin_str);
			arg->rsinf->is_error = TRUE;
			arg->rsinf->err_msg = "Unable to create context.";
			return FALSE;
		}
		v8::Context::Scope					context_scope(v8_ctx);
		v8::Local<v8::String>				source_string = v8_str(arg->isolate, arg->rsinf->t_source.c_str());
		v8::ScriptOrigin					script_origin(v8_str(arg->isolate, origin_str->c_str()));
		_free_obj(origin_str); ::swap_obj(arg->rsinf->t_source);//Clear source data
		v8::MaybeLocal<v8::UnboundScript>	unbound_script;
		std::unique_ptr<v8::ScriptCompiler::CachedData> cache;
		if (is_cached == TRUE) {
			cache = read_script_cached(arg->jsi->abs_path, cscript_path->c_str(), wj_env->check_file_state);
			if (cache == nullptr) {
				is_cached = FALSE;
			}
		}
		v8::ScriptCompiler::Source sources(source_string, script_origin, cache.release());
		v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;
		if (is_cached == TRUE) {
			options = v8::ScriptCompiler::kConsumeCodeCache;
		}
		else {
			if (arg->cache_type != kAfterExecute) {
				options = v8::ScriptCompiler::kEagerCompile;
			}
		}
		unbound_script = v8::ScriptCompiler::CompileUnboundScript(
			arg->isolate, &sources, options
		);
		if (arg->cache_type != kAfterExecute) {
			if (is_cached == FALSE) {
				if (wj_env->compiled_cached == TRUE && !(is_cli == TRUE && arg->jsi->is_raw_script == TRUE)) {
					create_script_cached_data(unbound_script, cscript_path->c_str());
				}
			}
		}
		if (unbound_script.IsEmpty()) {
			_free_obj(cscript_path);
			arg->rsinf->is_error = TRUE;
			arg->rsinf->err_msg = "Unable to compile script. Check your script than try again.";
			v8_ctx->DetachGlobal();
			v8_ctx.Clear();
			return FALSE;
		}
		::wrap_wjsx_env(arg->isolate, wj_env);
		v8::TryCatch trycatch(arg->isolate);
		unbound_script.ToLocalChecked()->BindToCurrentContext()->Run(v8_ctx);
		if (trycatch.HasCaught()) {
			_free_obj(cscript_path);
			::set__exception(arg->isolate, &trycatch, *arg->rsinf);
			v8_ctx->DetachGlobal();
			v8_ctx.Clear();
			return FALSE;
		}
		if (arg->cache_type == kAfterExecute) {
			if (is_cached == FALSE) {
				if (wj_env->compiled_cached == TRUE && !(is_cli == TRUE && arg->jsi->is_raw_script == TRUE)) {
					create_script_cached_data(unbound_script, cscript_path->c_str());
				}
			}
		}
		_free_obj(cscript_path);
		v8_ctx->DetachGlobal(); v8_ctx.Clear();
		source_string.Clear();
		return TRUE;
	}
	catch (...) {
		::handle_eptr(std::current_exception(), *arg->rsinf);
		return FALSE;
	}
}
template<typename type_of_ctx>
void _prepare_script(
	v8::Isolate* isolate,
	type_of_ctx* ctx,
	template_result& rsinf,
	wjsx_env* wj_env,
	js_script_inf& jsi,
	int is_async = TRUE,
	int with_current_context = FALSE
) {
	wja_func_arg* wjafa = new wja_func_arg();
	wjafa->ctx = ctx;
	wjafa->rsinf = &rsinf;
	wjafa->isolate = isolate;
	wjafa->jsi = &jsi;
	wjafa->cache_type = kAfterExecute;
	wjafa->wj_env = wj_env;
	if (not is_async) {
		wj_env->set_mood_script();
		if (wj_env->is_thread_req() == FALSE) {
			v8_compaile_java_script(wjafa);
			if (rsinf.is_error == FALSE) {
				v8_engine::wait_for_pending_task(isolate);
				isolate->LowMemoryNotification();
			}
		}
		else {
			v8::Locker locker(isolate);
			::v8_compaile_java_script(wjafa);
			if (rsinf.is_error == TRUE) {
				wj_env->set_runtime_error(rsinf.is_error);
			}
			else {
				//v8_engine::wait_for_pending_task(isolate, *wj_env->get_mutex());
				isolate->LowMemoryNotification();
			}
			v8::Unlocker unlocker(isolate);
			return;
		}

	}
	else {
		uv_loop_t* loop = uv_default_loop();
		uv_loop_init(loop);
		uv_update_time(loop);
		isolate->Enter();
		wja_ctx* context = new wja_ctx();
		context->args = wjafa;
		async__init(context, [](uv_async_t* handle) {
			wja_ctx* context = static_cast<wja_ctx*>(handle->data);
			wja_func_arg* arg = (wja_func_arg*)context->args;
			//arg->isolate->Enter();
			v8::Locker locker(arg->isolate);
			v8_compaile_java_script(arg);
			if (arg->rsinf->is_error == FALSE) {
				v8_engine::wait_for_pending_task(arg->isolate);
				arg->isolate->LowMemoryNotification();
			}
			v8::Unlocker unlocker(arg->isolate);
			arg->isolate->Exit();
			uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
				wja_ctx* context = static_cast<wja_ctx*>(handles->data); delete context;
				delete reinterpret_cast<uv_async_t*>(handles);
			});
		});
		uv_run(loop, UV_RUN_DEFAULT);
		uv_loop_close(loop); //free(loop);
		clear_isolate_data(isolate, 0);
	}
	if (wj_env->is_cli()) {
		::wrapper::clear_cache(*wj_env);
		return;
	}
	if (rsinf.is_error == TRUE)return;
	::wrapper::response_body_flush(*wj_env, false);
}

void js_compiler::run_script_async(
	v8::Isolate* isolate,
	std::map<std::string, std::map<std::string, std::string>>& ctx,
	template_result& rsinf,
	wjsx_env* wj_env,
	js_script_inf& jsi
) {
	_prepare_script(isolate, &ctx, rsinf, wj_env, jsi);
	return;
}
void js_compiler::run_script(
	v8::Isolate* isolate,
	std::map<std::string, std::map<std::string, std::string>>& ctx,
	template_result& rsinf,
	wjsx_env* wj_env,
	js_script_inf& jsi
) {
	if (not wj_env->is_cli() && wj_env->is_thread_req() == TRUE && wj_env->compiled_cached == FALSE) {
		::v8_compaile_java_script(isolate, ctx, rsinf, wj_env, jsi);
		if (rsinf.is_error == TRUE)return;
		::wrapper::response_body_flush(*wj_env, false);
		return;
	}
	_prepare_script(isolate, &ctx, rsinf, wj_env, jsi, FALSE);
}
//void run_script(
//	v8::Isolate* isolate,
//	std::map<std::string, std::map<std::string, std::string>>& ctx,
//	template_result& rsinf,
//	wjsx_env* wj_env,
//	js_script_inf& jsi
//) {
//	_prepare_script(isolate, &ctx, rsinf, wj_env, jsi, TRUE, TRUE);
//}
//Served CLI Request
void js_compiler::run_script_async(
	v8::Isolate* isolate,
	std::map<std::string, std::string>& ctx,
	template_result& rsinf,
	wjsx_env* wj_env,
	js_script_inf& jsi
) {
	_prepare_script(isolate, &ctx, rsinf, wj_env, jsi);
	return;
}
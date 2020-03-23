/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "web_jsx_global.h"
#	include "runtime_compiler.h"
#	include <libplatform/libplatform.h>
#	include "web_jsx_exp.h"
#	include "../util.h"
#	include "v8_util.h"
#	include <uv.h>
#	include "native_wrapper.h"
#	include "template_info.h"
//7:34 PM 12/8/2019
typedef struct {
	v8::Persistent<v8::String>source;
	v8::Persistent<v8::Value>param;
	v8::Persistent<v8::Promise::Resolver>resolver;
	bool is_async = false;
}compaile_arg;
typedef struct {
	compaile_arg* args;
	uv_async_t* asyncRequest;
	uv_work_t* work_t;
}async_ctx;

void extend_context(
	v8::Isolate* isolate,
	v8::Local<v8::Context> context,
	v8::Local<v8::Value> js_ctx
) {
	v8::Local<v8::Object> js_this = context->Global()->GetPrototype().As<v8::Object>();
	js_this->Set(context, v8_str(isolate, "context"), js_ctx);
}
void compile(
	v8::Isolate* isolate, compaile_arg* wlf
) noexcept {
	v8::Isolate::Scope					isolate_scope(isolate);
	v8::HandleScope						handle_scope(isolate);
	v8::Local<v8::Context> context		= isolate->GetCurrentContext();
	v8::Context::Scope					context_scope(context);
	v8::MaybeLocal<v8::Script> script	= v8::Script::Compile(context, v8::Local<v8::String>::New(isolate, wlf->source));
	v8::Local<v8::Promise::Resolver> resolver;
	if (wlf->is_async == true) {
		resolver = v8::Local<v8::Promise::Resolver>::New(isolate, wlf->resolver);
	}
	
	if (script.IsEmpty()) {
		if (wlf->is_async == true) {
			resolver->Reject(context, v8_str(isolate, "Unable to compile script. Check your script than try again."));
		}
		else {
			isolate->ThrowException(v8::Exception::Error(
				v8_str(isolate, "Unable to compile script. Check your script than try again.")));
		}
	}
	else {
		extend_context(isolate, context, v8::Local<v8::Value>::New(isolate, wlf->param));
		// Run the script to get the result.
		script.ToLocalChecked()->Run(context);
		script.ToLocalChecked().Clear();
		if (wlf->is_async == true) {
			resolver->Resolve(context, v8_str(isolate, "Success"));
			wlf->resolver.Reset();
		}
	}
	wlf->source.Reset(); wlf->param.Reset();
	context.Clear();
	//wlf->source.Clear(); wlf->param.Clear();
}
void compile_async(v8::Isolate* isolate, compaile_arg* wlf) {
	async_ctx* context = new async_ctx();
	context->args = wlf;
	//uv_loop_t* loop = uv_default_loop();
	//uv_loop_init(loop);
	isolate->Enter();
	//v8::Persistent<v8::Promise>prom;
	sow_web_jsx::async__init(context, [](uv_async_t* handle) {
		async_ctx* context = static_cast<async_ctx*>(handle->data);
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		compile(isolate, context->args);
		//isolate->LowMemoryNotification();
		isolate->Exit();
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
}
void sow_web_jsx::runtime_compiler(
	const v8::FunctionCallbackInfo<v8::Value>& args
) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined() || !args[0]->IsObject()) {
		isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "Config required....")));
		return;
	}
	v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	//auto x = config->Get(ctx, v8_str(isolate, "task_type")).ToLocalChecked()->ToNumber(ctx).ToLocalChecked()->Value();
	int task_type = (int)config->Get(ctx, v8_str(isolate, "task_type")).ToLocalChecked()->ToNumber(ctx).ToLocalChecked()->Value();
	compaile_arg* awlf = new compaile_arg();
	awlf->is_async = to_boolean(isolate, config->Get(ctx, v8_str(isolate, "is_async")).ToLocalChecked());
	v8::Local<v8::Promise::Resolver> resolver;
	if (awlf->is_async == true) {
		resolver = v8::Promise::Resolver::New(ctx).ToLocalChecked();
		args.GetReturnValue().Set(resolver->GetPromise());
	}
	if (task_type == 0) {
		web_extension ext = (web_extension)(int)config->Get(ctx, v8_str(isolate, "extension")).ToLocalChecked()->ToNumber(ctx).ToLocalChecked()->Value();
		if (ext == JS || ext == JSXH || ext == WJSXH ) {
			native_string path(isolate, config->Get(ctx, v8_str(isolate, "full_path")).ToLocalChecked());
			if (path.is_empty()) {
				if (awlf->is_async == true) {
					//args.GetReturnValue().Set(resolver->GetPromise());
					resolver->Reject(ctx, v8_str(isolate, "Full path should not left blank...."));
				}
				else {
					isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "Full path should not left blank....")));
				}
				delete awlf; config.Clear();
				return;
			}
			std::string source_str("");
			size_t ret = ::read_file(path.get_string().data(), source_str);
			path.clear();
			if (is_error_code(ret) == TRUE) {
				if (awlf->is_async == true) {
					//args.GetReturnValue().Set(resolver->GetPromise());
					resolver->Reject(ctx, v8_str(isolate, source_str.c_str()));
				}
				else {
					isolate->ThrowException(v8::Exception::Error(v8_str(isolate, source_str.c_str())));
				}
				source_str.clear(); std::string().swap(source_str);
				delete awlf; config.Clear();
				return;
			}
			//source_str = "function create_context( context ) {\n" + source_str + "\n};";
			awlf->source.Reset(isolate, v8_str(isolate, source_str.c_str()));
			source_str.clear(); std::string().swap(source_str);
		}
		else if (ext == JSX || ext == WJSX) {
			parser_settings* ps = new parser_settings();
			template_result* tr = new template_result();
			native_string root_dir(isolate, config->Get(ctx, v8_str(isolate, "root_dir")).ToLocalChecked());
			native_string page_path(isolate, config->Get(ctx, v8_str(isolate, "page_path")).ToLocalChecked());
			ps->dir = root_dir.c_str();
			ps->page_path = page_path.c_str();
			::ntemplate_parse_x(*ps, *tr);
			root_dir.clear(); page_path.clear();
			delete ps;
			if (tr->is_error == TRUE) {
				if (awlf->is_async == true) {
					//args.GetReturnValue().Set(resolver->GetPromise());
					resolver->Reject(ctx, v8_str(isolate, tr->err_msg.c_str()));
				}
				else {
					isolate->ThrowException(v8::Exception::Error(v8_str(isolate, tr->err_msg.c_str())));
				}
				delete tr; delete awlf; config.Clear();
				return;
			}
			if (tr->is_script_template == FALSE) {
				if (awlf->is_async == true) {
					//args.GetReturnValue().Set(resolver->GetPromise());
					resolver->Resolve(ctx, v8_str(isolate, tr->t_source.c_str()));
				}
				else {
					args.GetReturnValue().Set(v8_str(isolate, tr->t_source.c_str()));
				}
				tr->t_source.clear();
				delete tr; delete awlf; config.Clear();
				return;
			}
			//tr->t_source = "function create_context( context ) {\n" + tr->t_source + "\n};";
			awlf->source.Reset(isolate, v8_str(isolate, tr->t_source.c_str()));
			tr->t_source.clear(); std::string().swap(tr->t_source);
			delete tr;
		}
		else {
			if (awlf->is_async == true) {
				//args.GetReturnValue().Set(resolver->GetPromise());
				resolver->Reject(ctx, v8_str(isolate, "Unsupported extesion defined...."));
			}
			else {
				isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, "Unsupported extesion defined....")));
			}
			delete awlf; config.Clear();
			return;
		}
	}
	else {
		awlf->source.Reset(isolate, config->Get(ctx, v8_str(isolate, "source")).ToLocalChecked()->ToString(ctx).ToLocalChecked());
	}
	awlf->param.Reset(isolate, config->Get(ctx, v8_str(isolate, "context")).ToLocalChecked());
	
	if (awlf->is_async) {
		///args.GetReturnValue().Set(resolver->GetPromise());
		awlf->resolver.Reset(isolate, resolver);
		compile_async(isolate, awlf);
	}
	else {
		compile(isolate, awlf);
		delete awlf; config.Clear();
		args.GetReturnValue().Set(v8::Undefined(isolate));
	}
	return;
}

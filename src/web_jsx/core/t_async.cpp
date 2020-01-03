/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "t_async.h"
#if !defined(_THREAD_)
#include <thread> 
#endif//!_THREAD_
#if !defined(_SSTREAM_)
#include <sstream>
#endif//_SSTREAM_
void sow_web_jsx::async_callback(async_func_arg* arg) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope			handleScope(isolate);
	v8::Isolate::Scope		isolate_scope(isolate);
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8::MaybeLocal<v8::ObjectTemplate>());
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
		//v8::String::NewFromUtf8(isolate, "START")
		v8::Number::New(isolate, id)
	};
	callback->Call(context, context->Global(), 1, carg);
	context.Clear();
	isolate->Exit();
}
int sow_web_jsx::async_thread(v8::Isolate* isolate, v8::Local<v8::Array> func_array) {
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	uint32_t l = func_array->Length();
	int total_thread = 0;
	for (uint32_t i = 0; i < l; i++) {
		v8::Local<v8::Value>v_val = func_array->Get(ctx, i).ToLocalChecked();
		if (v_val->IsFunction()) {
			total_thread++;
			async_func_arg* arg = new async_func_arg();
			arg->data = NULL;
			arg->cb.Reset(isolate, v8::Local<v8::Function>::Cast(v_val));
			acync_init(isolate, async_callback, arg);
		}
	}
	return total_thread;
}

int sow_web_jsx::set_time_out(v8::Isolate* isolate, acync_cb func, async_func_arg* args, uint64_t interval) {
	uv_timer_t* timer_req = (uv_timer_t*)malloc(sizeof(uv_timer_t));
	async__context* context = new async__context();
	context->func = func;
	context->args = args;
	timer_req->data = context;
	uv_timer_init(uv_default_loop(), timer_req);
	isolate->Enter();
	/*[Simulate the setTimeout in JavaScript]*/
	return uv_timer_start(timer_req, [](uv_timer_t* handle) {
		async__context* context = static_cast<async__context*>(handle->data);
		context->func(context->args);
		uv_timer_stop(handle);
	}, interval, 0);
	/*[/Simulate the setTimeout in JavaScript]*/
};
int sow_web_jsx::acync_init(v8::Isolate* isolate, acync_cb func, async_func_arg* args) {
	async__context* context = new async__context();
	context->func = func;
	context->args = args;
	isolate->Enter();
	async__init(context, [](uv_async_t* handle) {
		async__context* context = static_cast<async__context*>(handle->data);
		context->func(context->args);
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handles) {
			delete reinterpret_cast<uv_async_t*>(handles);
		});
	});
	return 0;
};
/*void set_time_out_bp() {
	//2:13 AM 2/7/2019
	//https://github.com/kiddkai/libuv-examples/tree/master/src/timer
	uv_timer_t timer_req;
	uv_timer_init(uv_default_loop(), &timer_req);
	//[Simulate the setTimeout -> setInterval]
	uv_timer_start(&timer_req, [](uv_timer_t *handle) {
		uv_timer_stop(handle);
	}, 1000, 100);
	//[/Simulate the setTimeout -> setInterval]
	//[Simulate the setTimeout in JavaScript]
	uv_timer_start(&timer_req, [](uv_timer_t *handle) {
		uv_timer_stop(handle);
	}, 1000, 0);
	//[/Simulate the setTimeout in JavaScript]
	//[Simulate the setInterval in JavaScript]
	uv_timer_start(&timer_req, [](uv_timer_t *handle) {
		uv_timer_stop(handle);
	}, 0, 1000);
	//[/Simulate the setInterval in JavaScript]
};*/
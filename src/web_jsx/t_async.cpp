/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "t_async.h"

int sow_web_jsx::set_time_out(v8::Isolate* isolate, acync_cb func, async_func_arg*args, uint64_t interval) {
	uv_timer_t* timer_req = (uv_timer_t*)malloc(sizeof(uv_timer_t));
	async__context*context = new async__context();
	context->func = func;
	context->args = args;
	timer_req->data = context;
	uv_timer_init(uv_default_loop(), timer_req);
	isolate->Enter();
	/*[Simulate the setTimeout in JavaScript]*/
	return uv_timer_start(timer_req, [](uv_timer_t *handle) {
		async__context*context = static_cast<async__context*>(handle->data);
		context->func(context->args);
		uv_timer_stop(handle);
	}, interval, 0);
	/*[/Simulate the setTimeout in JavaScript]*/
};
int sow_web_jsx::acync_init(v8::Isolate* isolate, acync_cb func, async_func_arg*args) {
	async__context*context = new async__context();
	context->func = func;
	context->args = args;
	isolate->Enter();
	async__init(context, [](uv_async_t *handle) {
		async__context*context = static_cast<async__context*>(handle->data);
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
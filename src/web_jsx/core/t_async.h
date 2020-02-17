/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:51 PM 1/30/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_t_async_h)
#	define _t_async_h
#	include <uv.h>
#	include <v8.h>
#	include <vector>
#	include <iostream>
#pragma comment(lib,"ws2_32")

namespace sow_web_jsx {
	/*[async arguments]*/
	typedef struct {
		v8::Persistent<v8::Function> cb;
		void* data;
	}async_func_arg;
	/*[/async arguments]*/
	/*[async callback]*/
	typedef void (*acync_cb)(async_func_arg*);
	/*[/async callback]*/
	/*[async context]*/
	typedef struct {
		async_func_arg* args;
		acync_cb func;
	}async__context;
	/*[/async context]*/
	int async_thread(v8::Isolate* isolate, v8::Local<v8::Array> func_array);
	void async_callback(async_func_arg* arg);
	template<class ctx, class func>
	void async__init(ctx* async_ctx, func async_func) {
		uv_async_t* async = (uv_async_t*)malloc(sizeof(uv_async_t));
		async->data = async_ctx;
		uv_async_init(uv_default_loop(), async, async_func);
		uv_async_send(async);
		//uv_queue_work();
	}
	int set_time_out(v8::Isolate* isolate, acync_cb func, async_func_arg* args, uint64_t interval);
	/*
	 *  Register uv async to main loop
	 */
	int acync_init(v8::Isolate* isolate, acync_cb func, async_func_arg* args);
	
};
#endif//!_t_async_h
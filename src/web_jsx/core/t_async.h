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
#define _t_async_h
#if !defined(UV_H)
#include <uv.h>
#endif//!UV_H
#if !defined(INCLUDE_V8_H_)
#include <v8.h>
#endif // !INCLUDE_V8_H_
#if !defined(_VECTOR_)
#include <vector>
#endif//!_VECTOR_
#if !defined(_IOSTREAM_)
#include <iostream>
#endif // !_IOSTREAM_
#pragma comment(lib,"ws2_32")
typedef struct {
	v8::Persistent<v8::Function> cb;
	void*data;
}async_func_arg;
typedef void (*acync_cb)(async_func_arg*);
typedef struct {
	async_func_arg*args;
	acync_cb func;
}async__context;
namespace sow_web_jsx {
	template<class ctx, class func>
	void async__init(ctx*async_ctx, func async_func) {
		uv_async_t* async = (uv_async_t*)malloc(sizeof(uv_async_t));
		async->data = async_ctx;
		uv_async_init(uv_default_loop(), async, async_func);
		uv_async_send(async);
	};
	int set_time_out(v8::Isolate* isolate, acync_cb func, async_func_arg*args, uint64_t interval);
	/*
	 *  Register uv async to main loop
	 */
	int acync_init(v8::Isolate* isolate, acync_cb func, async_func_arg*args);
};
#endif//!_t_async_h
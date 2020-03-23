/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:00 PM 12/24/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _http_payload_h)
#	define _http_payload_h
#	include <v8.h>
#if !defined(V8_JS_METHOD)
#define V8_JS_METHOD(name)\
void name(const v8::FunctionCallbackInfo<v8::Value>& args)
#endif//!V8_JS_METHOD
namespace sow_web_jsx {
	V8_JS_METHOD(read_http_posted_file);
	V8_JS_METHOD(read_payload);
	V8_JS_METHOD(write_file_from_payload);
}
#endif//!_http_payload_h

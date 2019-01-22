/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma once
#if !defined(_v8_util_h)
#define _v8_util_h
#if !defined(INCLUDE_V8_H_)
#include <v8.h>
#endif // !INCLUDE_V8_H_
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif //!_web_jsx_global_h
namespace sow_web_jsx {
	const char* _toCharStr(const v8::String::Utf8Value& value);
#define T_CHAR _toCharStr
	const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x);
	v8::Local<v8::String> v8_str(v8::Isolate* isolate, const char* x);
	void __set__exception(v8::Isolate * isolate, v8::TryCatch*try_catch, template_result&tr);
};
#endif //!_v8_util_h

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_native_process_h)
#define _native_process_h
#if !defined(_v8_util_h)
#include "v8_util.h"
#endif//_v8_util_h
namespace sow_web_jsx {
	void compile_js(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif
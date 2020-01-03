/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _runtime_compiler_h)
#define _runtime_compiler_h
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif//_web_jsx_global_h
//#if !defined(_v8_engine_h)
//#include "v8_engine.h"
//#endif//!_v8_engine_h
#if !defined( _v8_util_h)
#include "v8_util.h"
#endif //!v8_util_h
#if !defined(V8_LIBPLATFORM_LIBPLATFORM_H_)
#include <libplatform/libplatform.h>
#endif//!V8_LIBPLATFORM_LIBPLATFORM_H_
#if !defined(_native_wrapper_h)
#include "native_wrapper.h"
#endif//!_native_wrapper_h
#if !defined(UV_H)
#include <uv.h>
#endif//!UV_H
#if !defined(_web_jsx_exp_h)
#include "web_jsx_exp.h"
#endif//_web_jsx_exp_h
#if !defined(_util_h)
#include "../util.h"
#endif//_util_h
namespace sow_web_jsx {
	void runtime_compiler(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif//!_runtime_compiler_h
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
#	define _runtime_compiler_h
#	include "v8.h"
namespace sow_web_jsx {
	void runtime_compiler(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif//!_runtime_compiler_h
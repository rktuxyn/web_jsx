/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:09 PM 1/14/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_native_module_h)
#	define _native_module_h
#	include <v8.h>
#	include <string>
#	include "wjsx_env.h"
namespace sow_web_jsx {
	typeof_module get_module_type(const std::string& path_str);
	void require_native(
		const v8::FunctionCallbackInfo<v8::Value>& args,
		const char* abs_path,
		const char* app_dir,
		const char* module_name
	);
	void* load_native_module(
		v8::Isolate* isolate,
		v8::Handle<v8::Object> target,
		const char* path,
		const char* app_dir
	);
}
#endif//!_native_module_h
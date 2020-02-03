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
#	include "v8_util.h"

typedef enum {
	NATIVE = 1,
	_JS = 2,
	NO_EXT = 3,
	_UNKNOWN = -1
}typeof_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
typedef void* h_get_proc_iddl;
typedef void* h_module;
#else
typedef HINSTANCE h_get_proc_iddl;
typedef HMODULE h_module;
#endif//!_WIN32
typedef void(*web_jsx_native_module)(v8::Handle<v8::Object>target);
namespace sow_web_jsx {
	typeof_module get_module_type(const std::string& path_str);
	void require_native(
		const v8::FunctionCallbackInfo<v8::Value>& args,
		const std::string abs_path,
		const std::string app_dir,
		const char* module_name
	);
	int load_native_module(
		v8::Isolate* isolate,
		v8::Handle<v8::Object> target,
		const char* path,
		const std::string app_dir
	);
	void free_native_modules();
	void free_working_module();
}
#endif//!_native_module_h
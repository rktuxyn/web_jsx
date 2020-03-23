/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:38 PM 1/28/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_module_store_h)
#	define _module_store_h
#	include <v8.h>
#	include <string>
#	include "wjsx_env.h"
namespace swjsx_module {
	void implimant_native_module(
		const v8::FunctionCallbackInfo<v8::Value>& args, 
		const char* app_dir,
		const char* root_dir
	);
	void scope_to_js_global(
		v8::Isolate* isolate,
		v8::Local<v8::Context>context,
		wjsx_env* wj_env
	);
	void clean_native_module(native_data_structure* native_data);
	void clean_working_module(native_data_structure* native_data);
}
#endif//!_module_store_h
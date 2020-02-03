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
namespace swjsx_module {
	void implimant_native_module(
		const v8::FunctionCallbackInfo<v8::Value>& args, 
		const std::string app_dir,
		const std::string root_dir
	);
	void scope_to_js_global(v8::Isolate* isolate, v8::Local<v8::Context>context);
	void clean_native_module();
}
#endif//!_module_store_h
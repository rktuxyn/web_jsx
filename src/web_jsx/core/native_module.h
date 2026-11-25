/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
namespace fsys_web_jsx {
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
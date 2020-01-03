/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _js_compiler_h)
#define _js_compiler_h
#if !defined(_v8_engine_h)
#include "v8_engine.h"
#endif//!_v8_engine_h
#if !defined( _runtime_compiler_h)
#include "runtime_compiler.h"
#endif //!_runtime_compiler_h
#if !defined(V8_LIBPLATFORM_LIBPLATFORM_H_)
#include <libplatform/libplatform.h>
#endif//!V8_LIBPLATFORM_LIBPLATFORM_H_
#if !defined(_native_wrapper_h)
#include "native_wrapper.h"
#endif//!_native_wrapper_h
#if !defined(UV_H)
#include <uv.h>
#endif//!UV_H
namespace sow_web_jsx {
	namespace js_compiler {
		int run_script(std::map<std::string, std::map<std::string, std::string>>& ctx, template_result&tr);
		int run_script_x(const char*script_source, std::map<std::string, std::string>&ctx);
		jsx_export void create_engine(const char*exec_path);
		jsx_export void dispose_engine();
		jsx_export v8::Isolate* get_isolate();
		jsx_export int run_script(const char*exec_path, const char*script_source, std::map<std::string, std::string>&ctx);
		jsx_export void run_script(std::map<std::string, std::map<std::string, std::string>>&, const char*, template_result&);
		/*uv async*/
		jsx_export void run_async(std::map<std::string, std::map<std::string, std::string>>&ctx, const char*exec_path, template_result& rsinf);
		//jsx_export void run_async(const char*, const char*, const char*, const char*, const char*);
		jsx_export void run_async(const char*exec_path, const char*script_source, std::map<std::string, std::string>&ctx);
	}
}
#endif //!NTEMPLATE_COMP_
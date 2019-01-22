/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma once
#if !defined( _js_compiler_h)
#define _js_compiler_h
#if !defined( _v8_util_h)
#include "v8_util.h"
#endif //!v8_util_h
#if !defined(V8_LIBPLATFORM_LIBPLATFORM_H_)
#include <libplatform/libplatform.h>
#endif//!V8_LIBPLATFORM_LIBPLATFORM_H_
#if !defined(_native_wrapper_h)
#include "native_wrapper.h"
#endif//!_native_wrapper_h
namespace sow_web_jsx {
	namespace js_compiler {
		void parse_script_x(v8::Isolate* isolate, std::map<std::string, std::map<std::string, std::string>>&ctx, template_result&);
		void parse_script_x(v8::Isolate*, v8::Local<v8::String>, const char*, const char*, const char*);
		jsx_export int run_console_script_x(const char*, const char*, const char*, const char*, const char*);
		jsx_export void run_template_x(std::map<std::string, std::map<std::string, std::string>>&, const char*, template_result&);
	}
}
#endif //!NTEMPLATE_COMP_

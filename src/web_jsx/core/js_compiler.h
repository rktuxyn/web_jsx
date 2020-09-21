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
#	define _js_compiler_h
#	include "template_info.h"
#	include <string>
#	include <map>
#	include "wjsx_env.h"
#	include <v8.h>
namespace sow_web_jsx {
	namespace js_compiler {
		/*If any error occured, application will be exited with FATAL*/
		void compile_raw_script(v8::Isolate* isolate, const char* script);
		void run_script(
			v8::Isolate* isolate,
			std::map<std::string, std::map<std::string, std::string>>& ctx,
			template_result& rsinf,
			wjsx_env* wj_env,
			js_script_inf& jsi
		);
		/*uv async*/
		void run_script_async(
			v8::Isolate* isolate,
			std::map<std::string, std::map<std::string, std::string>>& ctx, 
			template_result& rsinf,
			wjsx_env* wj_env, js_script_inf& jsi
		);
		void run_script_async(
			v8::Isolate* isolate,
			std::map<std::string, std::string>& ctx,
			template_result& rsinf,
			wjsx_env* wj_env,
			js_script_inf& jsi
		);
	}
}
#endif //!NTEMPLATE_COMP_
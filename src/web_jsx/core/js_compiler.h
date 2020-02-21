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
namespace sow_web_jsx {
	namespace js_compiler {
		int run_script(std::map<std::string, std::map<std::string, std::string>>& ctx, template_result& tr);
		int run_script_x(const char* script_source, std::map<std::string, std::string>& ctx);
		void create_engine(const char* exec_path);
		void dispose_engine();
		int run_script(const char* exec_path, const char* script_source, std::map<std::string, std::string>& ctx);
		void run_script(std::map<std::string, std::map<std::string, std::string>>&, const char*, template_result&);
		/*uv async*/
		void run_async(std::map<std::string, std::map<std::string, std::string>>& ctx, const char* exec_path, template_result& rsinf);
		//jsx_export void run_async(const char*, const char*, const char*, const char*, const char*);
		void run_async(const char* exec_path, const char* script_source, std::map<std::string, std::string>& ctx);
	}
}
#endif //!NTEMPLATE_COMP_
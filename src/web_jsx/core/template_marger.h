/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_template_marger_h)
#	define _template_marger_h
#	include "web_jsx_global.h"
namespace sow_web_jsx {
	namespace template_marger {
		int marge_template(
			template_result& tr,
			std::vector<std::string>& templates,
			std::string&html_body,
			const std::regex start_tag,
			const char* root_dir
		);
		int implimant_attachment(
			template_result& tr, 
			const char* root_dir,
			std::string&html_body
		);
	};
};
#endif//!template_parser_h
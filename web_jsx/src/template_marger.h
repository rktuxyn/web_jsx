/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:17 PM 1/20/2019
#pragma once
#if !defined(_template_marger_h)
#define _template_marger_h
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif //!_web_jsx_global_h
namespace sow_web_jsx {
	namespace template_marger {
		int marge_template(
			template_result& tr,
			std::vector<std::string>& templates,
			std::string&html_body,
			const std::regex start_tag,
			const std::string root_dir
		);
	};
};
#endif//!template_parser_h
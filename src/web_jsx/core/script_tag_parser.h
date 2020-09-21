/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:19 PM 1/20/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_script_tag_parser_h)
#	define _script_tag_parser_h
#	include "template_info.h"
namespace sow_web_jsx {
	namespace script_tag_parser {
		int parse(
			template_result & tr,
			std::string&html_body
		);
	};
};
#endif//!_script_tag_parser_h
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
#	include "web_jsx_global.h"
namespace sow_web_jsx {
	void add_common_func(std::stringstream& stream);
	void js_write_header(std::stringstream&js_stream);
	void js_write_footer(std::string&str);
	void js_write_footer(std::stringstream&js_stream);
	void js_write_console_header(std::stringstream&js_stream);
	void js_write_console_footer(std::stringstream&js_stream);
	namespace script_tag_parser {
		int parse(
			template_result & tr,
			std::string&html_body
		);
	};
};
#endif//!_script_tag_parser_h
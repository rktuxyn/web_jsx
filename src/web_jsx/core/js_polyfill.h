/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:43 PM 2/21/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_js_polyfill_h)
#	define _js_polyfill_h
#	include <sstream>
#	include <string>
namespace sow_web_jsx {
	void create_wj_core_extend_script(std::stringstream& js_stream, int is_cli);
	void js_write_header(std::stringstream& js_stream);
	void js_write_console_header(std::stringstream& js_stream);
};
#endif//!_js_polyfill_h
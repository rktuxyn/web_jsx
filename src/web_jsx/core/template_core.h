/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:45 PM 1/20/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_template_core_h)
#	define _template_core_h
#	include "template_info.h"
namespace sow_web_jsx {
	namespace template_core {
		int process_template(
			template_result& tr,
			const char* root_dir,
			const char* request_path
		);
	};
};
#endif//!template_parser_h
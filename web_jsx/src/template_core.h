/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:45 PM 1/20/2019
#pragma once
#if !defined(_template_core_h)
#define _template_core_h
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif //!_web_jsx_global_h
#if !defined(_template_marger_h)
#include "template_marger.h"
#endif//_template_marger_h
#if !defined(_template_reader_h)
#include "template_reader.h"
#endif//_template_reader_h
#if !defined(_script_tag_parser_h)
#include "script_tag_parser.h"
#endif//_script_tag_parser_h
namespace sow_web_jsx {
	namespace template_core {
		int process_template(
			template_result& tr,
			const std::string root_dir,
			const std::string request_path
		);
	};
};
#endif//!template_parser_h
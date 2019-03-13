/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:08 PM 11/10/2018
#include "web_jsx_exp.h"
using namespace sow_web_jsx;
#ifdef __cplusplus 
extern "C" {
#endif
	bool is_empty_char(const char* val) {
		if ((val != NULL) && (val[0] == '\0'))
			return true;
		return false;
	};
	jsx_export void ntemplate_parse_x(parser_settings&ps, template_result&rsinf) {
		if (is_empty_char(ps.dir)) {
			rsinf.err_msg = "Directory required!!!";
			rsinf.is_error = true;
			return;
		}
		if (is_empty_char(ps.page_path)) {
			rsinf.err_msg = "Scrip path required!!!";
			rsinf.is_error = true;
			return;
		}
		if (!dir_exists(ps.dir)) {
			rsinf.err_msg = "Directory not found!!! ";
			rsinf.is_error = true;
			return;
		}
		template_core::process_template(rsinf, ps.dir, ps.page_path);
		return;
	};
#ifdef __cplusplus
}
#endif
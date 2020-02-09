/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:08 PM 11/10/2018
#if !defined(_web_jsx_exp_h)
#define _web_jsx_exp_h
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Not Implemented for UNIX-style OS.
#else
#	include "template_core.h"
#ifdef __cplusplus 
extern "C" {
#endif
	void ntemplate_parse_x(parser_settings&ps, template_result&rsinf);
#ifdef __cplusplus
}
#endif
#endif//!_WIN32
#endif//!EXP_H
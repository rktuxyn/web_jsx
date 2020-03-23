/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:03 PM 2/17/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_template_info_h)
#	define _template_info_h
#	include <string>
#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE
//typedef struct {
//	std::string t_source;
//	std::string err_msg;
//	bool is_error;
//	bool is_script;
//	bool is_script_template;
//	bool remove_new_line;
//	bool is_strict;
//} template_result;
class template_result {
public:
	std::string t_source;
	std::string err_msg;
	int has_wjsx_template;
	int is_error;
	int is_script;
	int is_script_template;
	int remove_new_line;
	int is_strict;
	explicit template_result(){
		has_wjsx_template = is_error = is_script = is_script_template = remove_new_line = is_strict = FALSE;
	}
	~template_result() {
		if (t_source.size() > 0) {
			t_source.clear();
			std::string().swap(t_source);
		}
		if (err_msg.size() > 0) {
			err_msg.clear();
			std::string().swap(err_msg);
		}
	}
};
typedef struct {
	const char* page_path;
	const char* dir;
	const char* data;
} parser_settings;

typedef struct {
	const char* abs_path;
	const char* path_info;
	int is_raw_script;
}js_script_inf;

#endif//!_template_info_h

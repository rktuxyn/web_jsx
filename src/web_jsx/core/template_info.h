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
	bool is_error;
	bool is_script;
	bool is_script_template;
	bool remove_new_line;
	bool is_strict;
	explicit template_result(){
		is_error = is_script = is_script_template = remove_new_line = is_strict = false;
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
#endif//!_template_info_h

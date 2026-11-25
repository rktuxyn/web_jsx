/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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

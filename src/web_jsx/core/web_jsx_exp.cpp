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
//10:08 PM 11/10/2018
#	include "web_jsx_exp.h"
using namespace fsys_web_jsx;
bool is_empty_char(const char* val) {
	return (val == NULL || strlen(val) == 0);
}
void ntemplate_parse_x(parser_settings& ps, template_result& rsinf) {
	if (is_empty_char(ps.dir)) {
		rsinf.err_msg = "Directory required!!!";
		rsinf.is_error = 1;
		return;
	}
	if (is_empty_char(ps.page_path)) {
		rsinf.err_msg = "Script path required!!!";
		rsinf.is_error = 1;
		return;
	}
	/*if (dir_exists(ps.dir) < 0) {
		rsinf.err_msg = "This directory `";
		rsinf.err_msg.append(ps.dir);
		rsinf.err_msg.append("` not found!!! ");
		rsinf.is_error = 1;
		return;
	}*/
	template_core::process_template(rsinf, ps.dir, ps.page_path);
	return;
}
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
//1:25 PM 1/20/2019
#	include "template_reader.h"
#	include "web_jsx_global.h"
#	include "wjsx_env.h"
#	include "std_wrapper.hpp"
using namespace fsys_web_jsx;
int template_read(
	std::string& cur_template_path,
	const char* root_dir, 
	std::string&parent_template,
	const std::regex pattern_regx,
	template_result & tr,
	std::string&body) {
	fsys_web_jsx::format__path(cur_template_path);
	//std::cout << cur_template_path;//ERR_SPDY_PROTOCOL_ERROR chrome://net-export/
	std::string physicalpath(root_dir); physicalpath.append("\\"); physicalpath.append(cur_template_path);
	if (physicalpath.size() > _MAX_PATH) {
		//Ignore invalid template path
		return -2;
	}
	int ret = ::read_file(physicalpath.c_str(), body);
	if (is_error_code(ret) == TRUE) {
		tr.is_error = TRUE;
		if (ret == -2) {
			tr.err_msg += body.c_str();
		}
		else {
			tr.err_msg = tr.err_msg + "Parent Template =>" + cur_template_path + " not found. Chield Template =>" + parent_template + ".";
		}
		return -1;
	}
	parent_template = cur_template_path.c_str();
	swap_obj(cur_template_path);
	std::regex__str(body, pattern_regx, cur_template_path);
	body = std::regex_replace(body, pattern_regx, "");
	if (cur_template_path.empty() || cur_template_path == "INVALID" || cur_template_path.size() > _MAX_PATH) {
		//No more template here
		return 0;
	}
	//read more
	return 2;
};
int ::template_reader::read_template(template_result& tr,
	const char* root_dir,
	std::vector<std::string>& templates,
	std::string&parent_template,
	std::string&source) {
	auto pattern_regx = new std::regex("#extends (.*)");
	std::string* cur_template_path = new std::string();
	std::regex__str(source, *pattern_regx, *cur_template_path);
	if (cur_template_path->empty() || cur_template_path->size() > _MAX_PATH) {
		delete pattern_regx; delete cur_template_path;
		return 0;
	}
	tr.has_wjsx_template = 1;
	int count, error;
	templates.push_back(source); swap_obj(source);
	count = 1; error = 1;
	std::string&cur = *cur_template_path;
	while (count > 0) {
		count = ::template_read(cur, root_dir, parent_template, *pattern_regx, tr, source);
		if (count < 0) {
			error = -1; break;
		}
		if (count > 0 || count == 0) {
			templates.push_back(source);
			swap_obj(source);
		}
	}
	swap_obj(source);
	delete cur_template_path; delete pattern_regx;
	return error;
};
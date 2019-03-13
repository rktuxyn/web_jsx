/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:25 PM 1/20/2019
#include "template_reader.h"
using namespace sow_web_jsx;
int template_read(
	std::string& cur_template_path,
	const std::string root_dir, 
	std::string&parent_template,
	const std::regex pattern_regx,
	template_result & tr,
	std::string&body) {
	format__path(cur_template_path);
	//std::cout << cur_template_path;//ERR_SPDY_PROTOCOL_ERROR
	std::string physicalpath = root_dir + "\\" + cur_template_path;
	size_t ret = read_file(physicalpath.c_str(), body, true);
	if (ret < 0) {
		tr.is_error = true;
		tr.err_msg = tr.err_msg + "Parent Template =>" + cur_template_path + " not found. Chield Template =>" + parent_template + ".";
		return -1;
	}
	parent_template = cur_template_path;
	std::string().swap(cur_template_path);
	std::regex__str(body, pattern_regx, cur_template_path);
	body = std::regex_replace(body, pattern_regx, "");
	if (cur_template_path.empty() || cur_template_path == "INVALID") {
		//No more template here
		return 0;
	}
	//read more
	return 2;
};
int template_reader::read_template(template_result& tr,
	const std::string root_dir, 
	std::vector<std::string>& templates,
	std::string&parent_template,
	std::string&source) {
	auto pattern_regx = new std::regex("#extends (.*)");
	std::string* cur_template_path = new std::string();
	std::regex__str(source, *pattern_regx, *cur_template_path);
	if (cur_template_path->empty())return 0;
	int count, error;
	templates.push_back(source);
	std::string().swap(source);
	count = 1; error = 1;
	std::string&cur = *cur_template_path;
	while (count > 0) {
		count = template_read(cur, root_dir, parent_template, *pattern_regx, tr, source);
		if (count < 0) {
			error = -1; break;
		}
		if (count > 0 || count == 0) {
			templates.push_back(source);
			std::string().swap(source); 
		}
	}
	std::string().swap(source);
	delete cur_template_path;
	delete pattern_regx;
	return error;
};
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:45 PM 1/20/2019
#include "template_core.h"
using namespace sow_web_jsx;
int sow_web_jsx::template_core::process_template(template_result & tr,
	const std::string root_dir,
	const std::string request_path) {
	tr.is_error = false;
	std::string page_path = root_dir + request_path;
	format__path(page_path);
	std::string html_body("");
	if (read_file(page_path.c_str(), html_body, false) < 0) {
		std::string().swap(page_path);
		tr.err_msg = "_NOT_FOUND_";
		tr.is_error = true;
		return -1;
	}
	std::string().swap(page_path);
	int ret = 0;
	std::vector<std::string>* templates = new std::vector<std::string>();
	auto parent_path = new std::string(request_path.c_str());
	ret = template_reader::read_template(tr, root_dir, *templates, *parent_path, html_body);
	delete parent_path;
	if (ret < 0) {
		delete templates;
		return ret;
	}
	if (ret > 0) {
		std::regex* start_tag = new std::regex("<placeholder[^>]*>");
		ret = template_marger::marge_template(
			tr, *templates, html_body, *start_tag, root_dir
		);
		delete templates; delete start_tag;
		if (ret < 0) {
			std::string().swap(html_body);
			return ret;
		}
	}
	else {
		delete templates;
	}
	ret = script_tag_parser::parse(tr, html_body);
	html_body.swap(tr.t_source);
	return ret;
};
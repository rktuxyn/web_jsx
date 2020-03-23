/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:45 PM 1/20/2019
#	include "template_core.h"
#	include "web_jsx_global.h"
#	include "wjsx_env.h"
#	include "template_marger.h"
#	include "template_reader.h"
#	include "script_tag_parser.h"
using namespace sow_web_jsx;
int sow_web_jsx::template_core::process_template(template_result & tr,
	const char* root_dir,
	const char* request_path
) {
	tr.is_error = FALSE;
	std::string* page_path = new std::string(root_dir); page_path->append(request_path);
	::format__path(*page_path);
	std::string* html_body = new std::string();
	int fret = ::read_file(page_path->c_str(), *html_body);
	if (::is_error_code(fret) == TRUE) {
		_free_obj(page_path);
		tr.err_msg = "_NOT_FOUND_";
		tr.is_error = TRUE;
		html_body->swap(tr.t_source);
		_free_obj(html_body);
		return -1;
	}
	_free_obj(page_path);
	if (html_body->size() == 0)return 1;
	int ret = 0;
	std::vector<std::string>* templates = new std::vector<std::string>();
	std::string* parent_path = new std::string(request_path);
	ret = template_reader::read_template(tr, root_dir, *templates, *parent_path, *html_body);
	_free_obj(parent_path);
	if (ret < 0) {
		_free_obj(templates);
		_free_obj(html_body);
		return ret;
	}
	if (ret > 0) {
		std::regex* start_tag = new std::regex("<placeholder[^>]*>");
		ret = template_marger::marge_template(
			tr, *templates, *html_body, *start_tag, root_dir
		);
		_free_obj(templates); delete start_tag;
		if (ret < 0) {
			_free_obj(html_body);
			return ret;
		}
	}
	else {
		_free_obj(templates);
		::template_marger::implimant_attachment(tr, root_dir, *html_body);
		if (tr.is_error == TRUE)return -1;
	}
	ret = script_tag_parser::parse(tr, *html_body);
	html_body->swap(tr.t_source);
	_free_obj(html_body);
	return ret;
}
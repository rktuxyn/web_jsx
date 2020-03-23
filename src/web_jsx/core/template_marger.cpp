/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:17 PM 1/20/2019
#	include "template_marger.h"
#	include "web_jsx_global.h"
#	include "wjsx_env.h"
#	include "std_wrapper.hpp"
using namespace sow_web_jsx;
int parse_placeholder_node(
	std::list<std::string>&ml, std::string& body, 
	std::string& full_body, std::regex&id_regx, 
	std::regex&is_regx, std::regex&ie_regx,
	template_result& tr
) {
	for (auto s = ml.begin(); s != ml.end(); ++s) {
		auto sk = *s;
		if (sk.empty())continue;
		auto tmpl_id = REGEX_MATCH_STR(sk, id_regx);
		if (tmpl_id.empty() || tmpl_id == "INVALID") {
			tr.is_error = TRUE;
			tr.err_msg.append("Invalid tag defnined==>");
			tr.err_msg.append(sk + "<br/>");
			continue;
		};
		std::string impl_str;
		body = REGEX_REPLACE_MATCH(body, std::regex("(?:<impl-placeholder id=\"" + tmpl_id + "\">.+?</impl-placeholder>)"), [&](const std::smatch& m) {
			impl_str = m.str(0);
			if (impl_str.size() > 0) {
				impl_str = std::regex_replace(impl_str, is_regx, "");
				impl_str = std::regex_replace(impl_str, ie_regx, "");
			}
			return impl_str;
		}, 0);
		if (impl_str.empty()) impl_str = "";
		auto own_regx = new std::regex("(?:" + sk + ".+?</placeholder>)");
		full_body = REGEX_REPLACE_MATCH(full_body, *own_regx, [&](const std::smatch& m) {
			return impl_str;
		}, 0);
		delete own_regx;
		std::string().swap(impl_str);
	}
	return tr.is_error == TRUE ? -1 : 1;
}
/*template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
	for (auto& el : vec) {
		os << el << ' ';
	}
	return os;
};*/
int sow_web_jsx::template_marger::implimant_attachment(
	template_result& tr, 
	const char* root_dir,
	std::string&html_body
) {
	auto rml = new std::list<std::string>();
	auto attach_regx = new std::regex("#attach (.*)");
	REGEX_MATCH_LIST(*rml, html_body, *attach_regx);
	delete attach_regx;
	if (rml->size() == 0) {
		_free_obj(rml);
		return 0;
	}
	tr.has_wjsx_template = 1;
	int error_foud = -1;
	auto error_msg = new std::string();
	for (auto s = rml->begin(); s != rml->end(); ++s) {
		std::string itr = *s;
		std::string path = std::regex_replace(itr, std::regex("#attach "), "");
		std::string relative_path = root_dir; relative_path.append("\\").append(path);
		if (relative_path.size() > _MAX_PATH)continue;
		::format__path(relative_path);
		if (__file_exists(relative_path.c_str()) == false) {
			error_msg->append("Unable to found this attachment=> " + path + " & realative full-path =>" + relative_path + ".<br/>");
			error_foud = 1; continue;
		}
		if (error_foud > 0)continue;
		_NEW_STR(part);
		int ret = ::read_file(relative_path.c_str(), *part);
		if (::is_error_code(ret) == TRUE) {
			tr.is_error = TRUE;
			if (ret == -2L) {
				error_msg->append("file=> " + path + " Error:");
				error_msg->append(tr.err_msg += part->c_str());
				error_msg->append("<br/>");
			}
			else {
				error_msg->append("Unable to read this file=> " + path + " & realative full-path =>" + relative_path + ".<br/>");
			}
			goto _ERROR;
		}
		html_body = std::regex_replace(html_body, std::regex("#attach \\" + path), *part);
		_free_obj(part); swap_obj(itr); swap_obj(path); swap_obj(relative_path);
	}
	if (error_foud > 0)
		goto _ERROR;
	goto _SUCCESS;
_ERROR:
	_free_obj(rml);
	tr.is_error = TRUE;
	tr.err_msg = error_msg->c_str();
	_free_obj(error_msg);
	return -1;
_SUCCESS:
	_free_obj(rml);
	_free_obj(error_msg);
	return 1;
}
int sow_web_jsx::template_marger::marge_template(
	template_result& tr, 
	std::vector<std::string>& templates,
	std::string&html_body,
	const std::regex start_tag,
	const char* root_dir
) {
	int count = 0;
	std::vector<std::string>::reverse_iterator first = templates.rbegin();
	std::vector<std::string>::reverse_iterator last = templates.rend();
	std::__reverse(first, last);
	std::string parent_template;
	//auto regx = new std::regex("<placeholder[^>]*>");
	auto id_regx = new std::regex("<placeholder id=\"(.*)\">");
	auto is_regx = new std::regex("(?:<impl-placeholder[^>]*>)");
	auto ie_regx = new std::regex("(?:</impl-placeholder>)");
	std::regex* nrgx = new std::regex("(?:\\r\\n|\\r|\\n)");//Make Single Line
	//std::regex* ws = new std::regex("/^\\s*|\\s*$");//Remove White Space
	for (auto px : templates) {
		px = std::regex_replace(px, *nrgx, "8_r_n_gx_8");
		if (count == 0) {
			html_body = px;
			parent_template = px;
			count++;
			std::string().swap(px);
			continue;
		}
		auto ml = new std::list<std::string>();
		REGEX_MATCH_LIST(*ml, parent_template, start_tag);
		std::string().swap(parent_template);
		if (ml->empty()) { ml->clear(); delete ml; continue; }
		if (ml->size() <= 0) { ml->clear(); delete ml; continue; }
		parse_placeholder_node(*ml, px, html_body, *id_regx, *is_regx, *ie_regx, tr);
		//this->parse_placeholder_node(*ml, px, *id_regx, *is_regx, *ie_regx);
		parent_template = px; ml->clear(); delete ml;
		std::string().swap(px);
	};
	std::string().swap(parent_template);
	delete nrgx; delete id_regx; delete is_regx; delete ie_regx;
	if (tr.is_error == TRUE) {
		std::string().swap(html_body);
		return -1;
	}
	html_body = std::regex_replace(html_body, std::regex("(?:8_r_n_gx_8)"), "\r\n");
	return implimant_attachment(tr, root_dir, html_body);
}
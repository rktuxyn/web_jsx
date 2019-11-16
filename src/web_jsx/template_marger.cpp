/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:17 PM 1/20/2019
#include "template_marger.h"
using namespace sow_web_jsx;
int parse_placeholder_node(std::list<std::string>&ml, std::string& body, std::string& full_body, std::regex&id_regx, std::regex&is_regx, std::regex&ie_regx, template_result& tr) {
	for (auto s = ml.begin(); s != ml.end(); ++s) {
		auto sk = *s;
		if (sk.empty())continue;
		auto tmpl_id = REGEX_MATCH_STR(sk, id_regx);
		if (tmpl_id.empty() || tmpl_id == "INVALID") {
			tr.is_error = true;
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
		free(own_regx);
		std::string().swap(impl_str);
	}
	return tr.is_error == true ? -1 : 1;
}
/*template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
	for (auto& el : vec) {
		os << el << ' ';
	}
	return os;
};*/
int sow_web_jsx::template_marger::implimant_attachment(template_result& tr, const std::string root_dir, std::string&html_body) {
	auto rml = new std::list<std::string>();
	auto attach_regx = new std::regex("#attach (.*)");
	REGEX_MATCH_LIST(*rml, html_body, *attach_regx);
	free(attach_regx);
	if (rml->size() <=0) {
		free(rml);
		return 0;
	}
	int error_foud = -1;
	auto error_msg = new std::string();
	for (auto s = rml->begin(); s != rml->end(); ++s) {
		std::string itr = *s;
		std::string path = std::regex_replace(itr, std::regex("#attach "), "");
		std::string relativePath = root_dir + "\\" + path;
		if (relativePath.size() > _MAX_PATH)continue;
		format__path(relativePath);
		if (__file_exists(relativePath.c_str()) == false) {
			error_msg->append("Unable to found this attachment=> " + path + " & realative full-path =>" + relativePath + ".<br/>");
			error_foud = 1; continue;
		};
		if (error_foud > 0)continue;
		std::string part("");
		size_t ret = read_file(relativePath.c_str(), part, false);
		if (ret < 0L) {
			tr.is_error = true;
			if (ret == -2L) {
				error_msg->append("file=> " + path + " Error:");
				error_msg->append(tr.err_msg += part.c_str());
				error_msg->append("<br/>");
			}
			else {
				error_msg->append("Unable to read this file=> " + path + " & realative full-path =>" + relativePath + ".<br/>");
			}
			goto _ERROR;
		}
		html_body = std::regex_replace(html_body, std::regex("#attach \\" + path), part);
		std::string().swap(part);
		std::string().swap(itr);  std::string().swap(path); std::string().swap(relativePath);
	}
	free(rml);
	if (error_foud > 0)
		goto _ERROR;
	goto _SUCCESS;
_ERROR:
	tr.is_error = true;
	tr.err_msg = error_msg->data();
	free(error_msg);
	return -1;
_SUCCESS:
	free(error_msg);
	return 1;
}
int sow_web_jsx::template_marger::marge_template(
	template_result& tr, 
	std::vector<std::string>& templates,
	std::string&html_body,
	const std::regex start_tag,
	const std::string root_dir
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
	int ret = 0;
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
		if (ml->empty()) { free(ml); continue; }
		if (ml->size() <= 0) { free(ml); continue; }
		ret = parse_placeholder_node(*ml, px, html_body, *id_regx, *is_regx, *ie_regx, tr);
		//this->parse_placeholder_node(*ml, px, *id_regx, *is_regx, *ie_regx);
		parent_template = px;
		free(ml); std::string().swap(px);
	};
	std::string().swap(parent_template);
	free(nrgx); free(id_regx); free(is_regx); free(ie_regx);
	if (tr.is_error == true) {
		std::string().swap(html_body);
		return -1;
	}
	html_body = std::regex_replace(html_body, std::regex("(?:8_r_n_gx_8)"), "\r\n");
	return implimant_attachment(tr, root_dir, html_body);
}
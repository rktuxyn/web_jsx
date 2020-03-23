/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "script_tag_parser.h"
#	include "template_marger.h"
#	include "template_reader.h"
#	include "template_info.h"
#	include "std_wrapper.hpp"
#	include <sstream>
#	include "js_polyfill.h"
using namespace sow_web_jsx;

typedef enum {
	sts,
	ste,
	rts,
	rte
}e_tag;
typedef struct {
	std::string line, tag, start_tage_name;
	bool is_tag_start, is_tag_end, is_last_tag, tag_found;
	e_tag etag;
}parser_info;
typedef struct {
	std::string sts/*sts*/ = "<js::";
	std::string ste/*ste*/ = "::js>";
	std::string rts/*rts*/ = "<js=";
	std::string rte/*rte*/ = "=js>";
}script_tag;
class js_parser {
public:
	explicit js_parser() {
		script_tag *tags = new script_tag();
		this->regx_sts = new std::regex("(" + tags->sts + ")");
		this->regx_ste = new std::regex("(" + tags->ste + ")");
		this->regx_rts = new std::regex("(" + tags->rts + ")");
		this->regx_rte = new std::regex("(" + tags->rte + ")");
		this->rep_str = "88_REP__STR__88";
		this->rep_str_regex = new std::regex("(" + this->rep_str + ")");
		//this->result_key = "_nopr_";
		delete tags;
	}
	js_parser(const js_parser&) = delete;
	js_parser& operator=(const js_parser&) = delete;
	void clear() {
		delete this->regx_sts;
		delete this->regx_ste;
		delete this->regx_rts;
		delete this->regx_rte;
		delete this->rep_str_regex;
		std::string().swap(this->rep_str);
	}
	~js_parser() {
		this->clear();
	}
private:
public:
	std::regex* regx_sts;
	std::regex* regx_ste;
	std::regex* regx_rts;
	std::regex* regx_rte;
	std::string rep_str;
	std::regex* rep_str_regex;
	int start_tag(const std::regex regx, parser_info&info) {
		if (REGEX_IS_MATCH(info.line, regx) == 0) {
			if (info.is_tag_end && info.etag != e_tag::sts) {
				info.line = info.line += " " + this->rep_str + "); context.response.write(" + this->rep_str;
			}
			return 0;
		}
		bool _default = true;
		info.is_tag_start = true;
		switch (info.etag) {
		case e_tag::sts/*<js::*/:
			if (REGEX_IS_MATCH(info.line, *this->regx_ste) == 0) {
				info.is_tag_end = false;
				info.line = std::regex_replace(info.line, regx, " " + this->rep_str + ");");
				break;
			}
			info.is_tag_end = true;
			info.is_tag_start = false;
			//(?:<js::.+?::js>)
			info.line = REGEX_REPLACE_MATCH(info.line, std::regex("(<js::(.+?)::js>)"),
				[&](const std::smatch& m) {
				std::string matchstr(m.str(0));
				if (matchstr.empty()) {
					return std::string("");
				}
				//<js::context.response.write('JS Object==>');::js> BUG
				//<js:: context.response.write("JS Object==>");::js> WORK
				matchstr = std::regex_replace(matchstr, *this->regx_sts, this->rep_str + ", true);\n");
				matchstr = std::regex_replace(matchstr, *this->regx_ste, "\ncontext.response.write(" + this->rep_str);
				return matchstr;
			});
			break;
		case e_tag::rts/*<js=*/:
			if (REGEX_IS_MATCH(info.line, *this->regx_rte) == 0) {
				info.is_tag_end = false;
				//info.line = std::regex_replace(info.line, std::regex("'"), " " + this->rep_str);
				info.line = std::regex_replace(info.line, regx, "");
				info.line.append(this->rep_str + ");\ncontext.response.write(");
				break;
			}
			info.is_tag_end = true;
			info.is_tag_start = false;
			//(?:<js=.+?=js>)
			info.line = REGEX_REPLACE_MATCH(info.line, std::regex("(<js=(.+?)=js>)"),
				[&](const std::smatch& m) {
				std::string matchstr(m.str(0));
				if (matchstr.empty()) {
					return std::string("");
				}
				//matchstr = this->rep_str + ", !1).write_p(" + matchstr + ").write(" + this->rep_str;
				matchstr = std::regex_replace(matchstr, *this->regx_rts, this->rep_str + ", !1).write_p(");
				matchstr = std::regex_replace(matchstr, *this->regx_rte, ").write(" + this->rep_str);
				return matchstr;// std::regex_replace(matchstr, std::regex("'"), " " + this->rep_str);
			});
			//info.line = std::regex_replace(info.line, regx, " " + this->rep_str + "); context.response.write(");
			//info.line = std::regex_replace(info.line, *this->regx_rte, ");context.response.write(" + this->rep_str);
			break;
		default: _default = false; break;
		}
		if (_default == false) {
			throw new std::runtime_error("We got error, Invalid script tag found... :(");
		}
		return 0;
	};
	int end_tag(const std::regex regx, parser_info&info) {
		if (info.is_tag_start == false && info.is_tag_end == true) {
			return 0;
		}
		if (info.is_tag_start != false && info.is_tag_end != true) {
			/** Check End Tag**/
			info.is_tag_start = true;
			if (REGEX_IS_MATCH(info.line, regx) == 0) {
				info.is_tag_end = false;
				return -1;
			}
			switch (info.etag) {
			case e_tag::ste/*::js>*/:
				info.is_tag_end = true;
				info.is_tag_start = false;
				info.line = std::regex_replace(info.line, regx, "\ncontext.response.write(" + this->rep_str);
				break;
			case e_tag::rte/*=js>*/:
				info.is_tag_end = true;
				info.is_tag_start = false;
				info.line = std::regex_replace(info.line, regx, ");\ncontext.response.write(" + this->rep_str);
				break;
			default:break;
			}
			return 0;
		}
		return 0;
	};
};
int script_tag_parser::parse(template_result & tr, std::string&html_body) {
	std::regex*regx_sts = new std::regex("(<js::)");
	std::regex*regx_str = new std::regex("(<js=)");
	if (REGEX_IS_MATCH(html_body, *regx_sts) == 0) {
		delete regx_sts;
		if (REGEX_IS_MATCH(html_body, *regx_str) == 0) {
			if (tr.remove_new_line==1) {
				//Normalize
				html_body = std::regex_replace (html_body, std::regex("\\n\\s*\\n"), "\n");
			}
			tr.is_script_template = 0;
			return 0;
		}
		delete regx_str;
	}
	else {
		delete regx_sts;
		delete regx_str;
	}
	tr.is_script_template = 1;
	std::regex* rgx = new std::regex("(?:\\r\\n|\\r|\\n)");
	std::regex* qu = new std::regex("'");
	parser_info* info = new parser_info();
	js_parser* jsp = new js_parser();
	info->is_tag_start = false;
	info->is_tag_end = true;
	std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	sow_web_jsx::js_write_header(js_stream);
	std::sregex_token_iterator iter(html_body.begin(), html_body.end(), *rgx, -1);
	for (std::sregex_token_iterator end; iter != end; ++iter) {
		info->tag_found = false;
		info->line = iter->str();
		if (info->line.empty()) {
			continue;
		}
		if (info->is_tag_end) {
			info->line = "context.response.write(" + jsp->rep_str + info->line;
			js_stream << "\n";
		}
		info->etag = sts;
		jsp->start_tag(*jsp->regx_sts, *info);/**TAG-1 <js::**/
		info->etag = ste;
		if (jsp->end_tag(*jsp->regx_ste, *info)/**TAG-2 ::js>**/ < 0)
			js_stream << "\n";
		info->etag = rts;
		jsp->start_tag(*jsp->regx_rts, *info);/**TAG-3 <js=**/
		info->etag = rte;
		jsp->end_tag(*jsp->regx_rte, *info);/**TAG-4 =js>**/
		if (info->is_tag_end) {
			info->line = std::regex_replace(info->line, *qu, "\\x27");
			info->line = std::regex_replace(info->line, *jsp->rep_str_regex, "'");
			js_stream << info->line << "');";
			std::string().swap(info->line);
			continue;
		}
		info->line = std::regex_replace(info->line, *jsp->rep_str_regex, "'");
		js_stream << info->line;
		std::string().swap(info->line);
	}
	//std::string().swap(html_body);
	delete info; delete rgx;
	delete qu; delete jsp;
	js_stream.str().swap(html_body);
	std::stringstream().swap(js_stream);
	html_body = std::regex_replace(html_body, std::regex("(?:context.response.write\\(''\\);)"), "");
	html_body = std::regex_replace(html_body, std::regex("(?:context.response.write\\(' '\\);)"), "");
	//html_body = std::regex_replace(html_body, std::regex("(?:context.response.write\\('  '\\);)"), "");
	if (tr.remove_new_line) {
		html_body = std::regex_replace (html_body, std::regex("\\n\\s*\\n"), "\n");
		html_body = std::regex_replace (html_body, std::regex("\\r\\n\\s*\\r\\n"), "\n");
	}
	//sow_web_jsx::js_write_footer(html_body);

	return 1;
};

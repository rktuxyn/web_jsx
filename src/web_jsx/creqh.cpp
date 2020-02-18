/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "core/web_jsx_global.h"
#	include "creqh.h"
void web_jsx::cgi_request::get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, const char*app_path) {
	replace_back_slash(root_dir);
	global["root_dir"] = root_dir;
	global["host"] = get_env_c("HTTP_HOST");
	global["remote_addr"] = get_env_c("REMOTE_ADDR");
	global["server_protocol"] = get_env_c("SERVER_PROTOCOL");
	global["https"] = get_env_c("HTTPS");
	global["app_path"] = app_path;//get_app_path();
	global["env_path"] = get_env_c("path");
}
void web_jsx::cgi_request::get_request_object(
	std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, 
	req_method&method
) {
	request["method"] = method == req_method::GET ? "GET" : "POST";
	if (method == req_method::POST) {
		request["content_length"] = get_env_c("CONTENT_LENGTH");
	}
	else {
		request["pay_load"] = "[]";
		request["content_length"] = "0";
	}
	if (!query_string.empty()) {
		std::string* quer_str = new std::string("");
		json_obj_stringify(query_string, *quer_str);
		request["query_string"] = quer_str->c_str();
		_free_obj(quer_str);
	}
	else {
		request["query_string"] = "{}";
	}
	request["page_path"] = get_env_c("PATH_INFO");
	request["content_type"] = get_content_type();
	request["cookie"] = get_env_c("HTTP_COOKIE");
	request["user_agent"] = get_env_c("HTTP_USER_AGENT");
	request["accept"] = get_env_c("HTTP_ACCEPT");
	request["accept_encoding"] = get_env_c("HTTP_ACCEPT_ENCODING");
}
void web_jsx::cgi_request::not_found_response(const char* content_type) {
	std::cout << "Content-Type:" << content_type << H_N_L;
	std::cout << "Accept-Ranges:bytes" << H_N_L;
	std::cout << "X-Powered-By:safeonline.world" << H_N_L;
	std::cout << "X-Process-By:web_jsx" << H_N_L;
#if defined(FAST_CGI_APP)
	std::cout << "Status: 404 Not found" << H_N_L;
#else
	resp.append(" 404 Not found\n");
	std::cout << get_env_c("SERVER_PROTOCOL");
#endif//FAST_CGI_APP
	std::cout << "\r\n";
}

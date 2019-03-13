#include "creqh.h"
#if !defined(FAST_CGI_APP)
void get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, const char*app_path) {
	replace_back_slash(root_dir);
	global["root_dir"] = root_dir;
	global["host"] = get_env_c("HTTP_HOST");
	global["remote_addr"] = get_env_c("REMOTE_ADDR");
	global["server_protocol"] = get_env_c("SERVER_PROTOCOL");
	global["https"] = get_env_c("HTTPS");
	global["app_path"] = app_path;
	global["env_path"] = get_env_c("path");
};
void get_request_object(std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, req_method&method) {
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
		free(quer_str);
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
};
void not_found_response(const char* content_type) {
	std::string resp(get_env_c("SERVER_PROTOCOL"));
	resp.append(" 404 Not found\n");
	std::cout << resp;
	write_header(content_type);
	std::cout << "\r\n";
};
#endif//!FAST_CGI_APP
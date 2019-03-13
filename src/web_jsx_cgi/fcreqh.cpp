#include "fcreqh.h"
#if defined(FAST_CGI_APP)
const char * freq_env_c(const char* var_name, char **envp) {
	char *env_val = FCGX_GetParam(var_name, envp);
	if (((env_val != NULL) && (env_val[0] == '\0')) || env_val == NULL) {
		return "";
	}
	return const_cast<const char*>(env_val);
};
void get_global_obj(std::map<std::string, std::string>& global, std::string&root_dir, char **envp) {
	replace_back_slash(root_dir);
	global["root_dir"] = root_dir;
	global["host"] = freq_env_c("HTTP_HOST", envp);
	global["remote_addr"] = freq_env_c("REMOTE_ADDR", envp);
	global["server_protocol"] = freq_env_c("SERVER_PROTOCOL", envp);
	global["https"] = freq_env_c("HTTPS", envp);
};
void get_request_object(std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string, req_method&method, const char*content_type, char **envp) {
	request["method"] = method == req_method::GET ? "GET" : "POST";
	if (method == req_method::POST) {
		request["content_length"] = freq_env_c("CONTENT_LENGTH", envp);
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
	request["page_path"] = freq_env_c("PATH_INFO", envp);
	request["content_type"] = content_type;
	request["cookie"] = freq_env_c("HTTP_COOKIE", envp);
	request["user_agent"] = freq_env_c("HTTP_USER_AGENT", envp);
	request["accept"] = freq_env_c("HTTP_ACCEPT", envp);
	request["accept_encoding"] = freq_env_c("HTTP_ACCEPT_ENCODING", envp);
};
void not_found_response(const char* content_type, char **envp) {
	const char*server_protocol = "";
	std::cout << server_protocol << " 404 Not found\n";
	write_header(content_type);
	std::cout << "\r\n";
};
#endif//!FAST_CGI_APP
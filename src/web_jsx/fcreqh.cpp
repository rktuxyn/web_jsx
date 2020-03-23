/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "fcreqh.h"
#	include "core/web_jsx_global.h"
#	include "core/wjsx_env.h"
#	include <fcgi_stdio.h>
#	include <fcgio.h>
#	include <fcgi_config.h>  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF

using namespace sow_web_jsx;
const char* fcgi_request::freq_env_c(const char* var_name, char** envp) {
	char* env_val = FCGX_GetParam(var_name, envp);
	if (env_val == NULL || strlen(env_val) == 0)return "";
	return const_cast<const char*>(env_val);
}
int fcgi_request::freq_env_c(const char* var_name, char** envp, std::string& out_str){
	char* env_val = FCGX_GetParam(var_name, envp);
	if (env_val == NULL)return FALSE;
	size_t len = strlen(env_val);
	int rec = FALSE;
	if (len > 0) {
		out_str = std::string(env_val, len);
		rec = TRUE;
	}
	//delete[]env_val;
	return rec;
}
void read_request_header(char **envp, std::string&out) {
	char **env = envp;
	std::stringstream ss;
	ss << "["; int count = 0;
	while (*(++env)) {
		char* en = *env;
		if (count != 0)ss << ",";
		ss << "\"" << en << "\"";
		count++;
	}
	ss << "]";
	ss.str().swap(out);
	swap_obj(ss);
	return;
}
void fcgi_request::get_global_obj(
	std::map<std::string, std::string>& global,
	std::string&root_dir, 
	const char*app_path,
	const char*env_path,
	char **envp
) {
	//std::cout << root_dir.c_str() << std::endl;
	::replace_back_slash(root_dir);
	global["root_dir"] = std::string(root_dir.c_str());
	freq_env_c("HTTP_HOST", envp, global["host"]);
	freq_env_c("REMOTE_ADDR", envp, global["remote_addr"]);
	freq_env_c("SERVER_PROTOCOL", envp, global["server_protocol"]);
	freq_env_c("HTTPS", envp, global["https"]);
	global["app_path"] = app_path;
	global["env_path"] = env_path;
}
void fcgi_request::get_request_object(
	std::map<std::string, std::string>&request, std::map<std::string, std::string>&query_string,
	const req_method method,
	const char*content_type, char **envp
) {
	request["method"] = method == req_method::GET ? "GET" : "POST";
	if (method == req_method::POST) {
		freq_env_c("CONTENT_LENGTH", envp, request["content_length"]);
	}
	else {
		request["pay_load"] = "[]";
		request["content_length"] = "0";
	}
	if (!query_string.empty()) {
		::json_obj_stringify(query_string, request["query_string"]);
	}
	else {
		request["query_string"] = "{}";
	}
	::read_request_header(envp, request["header"]);
	freq_env_c("LOGON_USER", envp, request["logon_user"]);
	freq_env_c("PATH_INFO", envp, request["page_path"]);
	freq_env_c("HTTP_COOKIE", envp, request["cookie"]);
	freq_env_c("HTTP_USER_AGENT", envp, request["user_agent"]);
	freq_env_c("HTTP_ACCEPT", envp, request["accept"]);
	freq_env_c("HTTP_ACCEPT_ENCODING", envp, request["accept_encoding"]);
	request["content_type"] = content_type;
}
void fcgi_request::not_found_response(
	const char* content_type, 
	char **envp, 
	const char* ex_dir,
	wjsx_env* wj_env
) {
	std::ostream& cout = wj_env->cout();
	cout << "Content-Type:" << content_type << H_N_L;
	cout << "Accept-Ranges:bytes" << H_N_L;
	cout << "X-Powered-By:safeonline.world" << H_N_L;
	cout << "X-Process-By:web_jsx" << H_N_L;
	cout << "Status: 404 Not found" << H_N_L;
	cout << "WebJsx-Error-Code:404" << H_N_L;
	_NEW_STR(path_translated);
	freq_env_c("PATH_TRANSLATED", envp, *path_translated);
	cout << "path_translated:" << path_translated->c_str() << H_N_L;
	_free_obj(path_translated);
	cout << "\r\n";
	std::string* str = new std::string(ex_dir);
	str->append("error\\404.html");
	_NEW_STR(body);
	int ret = ::read_file(str->c_str(), *body);
	if (ret > 0) {
		cout << body->c_str();
	}
	else {
		cout << "File Not Found...";
	}
	_free_obj(body); _free_obj(str);
	cout.flush();
}
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "util.h"
//9:11 PM 11/18/2018
void replace_back_slash(std::string&str) {
	std::size_t found = str.find_first_of("\\");
	while (found != std::string::npos) {
		str[found] = '/';
		found = str.find_first_of("\\", found + 1);
}
};
const char* get_app_dir() {
#if defined(GetModuleFileName)
#else
#error !TODO
#endif
	wchar_t* buf = (wchar_t*)malloc(_MAX_PATH);
	GetModuleFileName(NULL, buf, _MAX_PATH);
	const char* result = reinterpret_cast<const char*>(buf);
	return result;
};
const char* get_current_working_dir(void) {
	static char cwd[FILENAME_MAX];
	if (get_current_dir(cwd, FILENAME_MAX) == NULL)
		*cwd = '\x0';
	return cwd;
};
char *get_env(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	return env_val;
};
const char *get_env_c(const char* var_name) {
	char *env_val;
	size_t len;
	errno_t err = _dupenv_s(&env_val, &len, var_name);
	if (((env_val != NULL) && (env_val[0] == '\0')) || env_val == NULL) {
		return "";
	}
	return const_cast<const char*>(env_val);
};
int get_env_path(std::string&path_str) {
	std::istringstream tokenStream(get_env_c("path"));
	int rec = -1;
	while (std::getline(tokenStream, path_str, ';')) {
		if (path_str.find("web_jsx") != std::string::npos) {
			rec = 1;
			break;
		}
	}
	std::istringstream().swap(tokenStream);
	return rec;
};
void print_envp(char*envp[]) {
	int count = 0;
	for (++envp; *envp; ++envp) {
		char* a = *envp;
		std::cout << a << "<br/>";
		count++;
	}
};
int print_env_var(char* val, const char* env) {
	if (val != nullptr) {
		std::cout << env << "==>" << val << "<br/>";
		return 1;
	}
	std::cout << env << "==>Not Found!!!" << "<br/>";
	return -1;
};
int write___file(const char*path, const char*buffer) {
	FILE*fstream;
	errno_t err;
	err = fopen_s(&fstream, path, "w+");
	if (err != 0) return -1;
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), fstream);
	if (ferror(fstream)) {
		fclose(fstream);
		fstream = NULL;
		return -1;
	}
	fclose (fstream);
	fstream = NULL;
	return static_cast<int>(len);
};
web_extension get_request_extension (const std::string& path_str) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) {
		return web_extension::RAW_SCRIPT;
	}
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jsx") {
		return web_extension::JSX;
	}
	if (str_extension == "jsxh") {
		return web_extension::JSXH;
	}
	return web_extension::UNKNOWN;
};
void request_file_info (const std::string& path_str, std::string&dir, std::string&file_name) {
	size_t found;
	found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
	file_name = path_str.substr(found + 1);
};
void server_physical_path(const std::string& path_str, const std::string& path_info, std::string &root_dir) {
	std::string p_i = std::regex_replace(std::string(path_info), std::regex("(?:/)"), "\\");
	root_dir = path_str.substr(0, path_str.find(p_i));
};
req_method determine_req_method(void) {
	char* REQUEST_METHOD = get_env("REQUEST_METHOD");
	if (REQUEST_METHOD == NULL)
		return req_method::UNSUPPORTED;
	if (strcmp(REQUEST_METHOD, "GET") == 0)
		return req_method::GET;
	if (strcmp(REQUEST_METHOD, "POST") == 0)
		return req_method::POST;
	return req_method::UNSUPPORTED;
};
const char* get_content_type(void) {
	const char* CONTENT_TYPE = get_env_c("CONTENT_TYPE");
	if ((CONTENT_TYPE != NULL) && (CONTENT_TYPE[0] == '\0')) {
		return "text/html";
	}
	return CONTENT_TYPE;
};
void read_query_string(std::map<std::string, std::string>&data) {
	std::string* query= new std::string(get_env("QUERY_STRING"));
	if (query->empty())return;
	std::regex* pattern=new std::regex("([\\w+%]+)=([^&]*)");
	auto words_begin = std::sregex_iterator(query->begin(), query->end(), *pattern);
	auto words_end = std::sregex_iterator();
	for (auto i = words_begin; i != words_end; i++) {
		std::string* key = new std::string((*i)[1].str());
		std::string* value = new std::string((*i)[2].str());
		data["\"" + *key + "\""] = "\"" + (*value) + "\"";
	}
	free(query);
	free(pattern);
	return;
};
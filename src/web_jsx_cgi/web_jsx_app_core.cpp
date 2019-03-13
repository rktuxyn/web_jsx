/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//4:02 PM 2/3/2019
#include "web_jsx_app_core.h"
void get_user_obj(std::map<std::string, std::string>& user_info, std::map<std::string, std::string>& global) {
	user_info["name"] = "";
	user_info["is_authenticated"] = "false";
	user_info["role"] = "";
};
#if defined(FAST_CGI_APP)
void run__js_scrip(const char* content_type, std::string&ex_dir, std::string&root_dir, req_method&method, template_result& tr, char **envp) {
#else
void run__js_scrip(const char* content_type, std::string&ex_dir, std::string&root_dir, req_method&method, template_result& tr, const char*execute_path) {
#endif//FAST_CGI_APP
	auto ctx = new std::map<std::string, std::map<std::string, std::string>>();
	auto req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	auto global_obj = new std::map<std::string, std::string>();
#if defined(FAST_CGI_APP)
	const char* QUERY_STRING = freq_env_c("QUERY_STRING", envp);
	get_global_obj(*global_obj, root_dir, envp);
	read_query_string(*query_string, QUERY_STRING);
#else
	get_global_obj(*global_obj, root_dir, execute_path);
	read_query_string(*query_string, get_env_c("QUERY_STRING"));
#endif//!FAST_CGI_APP
#if defined(FAST_CGI_APP)
	get_request_object(*req_obj, *query_string, method, content_type, envp);
#else
	get_request_object(*req_obj, *query_string, method);
#endif//!FAST_CGI_APP
	auto user_inf = new std::map<std::string, std::string>();
	get_user_obj(*user_inf, *global_obj);
	obj_insert(*req_obj, "request", *ctx);
	obj_insert(*global_obj, "global", *ctx);
	obj_insert(*user_inf, "user", *ctx);
	try {
#if defined(FAST_CGI_APP)
		sow_web_jsx::js_compiler::run_script(*ctx, ex_dir.c_str(), tr);
#else
		sow_web_jsx::js_compiler::run_async(*ctx, ex_dir.c_str(), tr);
		sow_web_jsx::js_compiler::dispose_engine();
#endif//FAST_CGI_APP
	} catch (std::exception&e) {
		write_header(content_type);
		std::cout << "\r\n";
		std::cout << e.what();
	}
	delete req_obj, delete global_obj; delete ctx; delete user_inf; delete query_string;
	std::string().swap(tr.t_source);
};
#if defined(FAST_CGI_APP)
void prepare_response(const char* content_type, const char*path_translated, const char* execute_path, req_method&method, const char* path_info, char **envp) {
#else
void prepare_response(const char* content_type, const char*path_translated, const char* execute_path, req_method&method, const char* path_info) {
#endif//FAST_CGI_APP
	web_extension ext = get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN) {
		write_header(content_type);
		std::cout << "\r\n";
		std::cout << "Unsupported extension....";
		std::cout << path_translated;
		fflush(stdout);
		return;
	}
	auto ex_dir = new std::string();
	auto ex_name = new std::string();
	request_file_info(execute_path, *ex_dir, *ex_name);
	ex_dir->append("\\");
	auto root_dir = new std::string();
	server_physical_path(path_translated, path_info, *root_dir);
	root_dir->append("\\");
	template_result* tr = new template_result();
	tr->remove_new_line = true;
	if (ext == web_extension::JSXH) {
		std::stringstream stream;
		sow_web_jsx::js_write_header(stream);
		if (sow_web_jsx::read_file(path_translated, stream, false) < 0) {
#if defined(FAST_CGI_APP)
			not_found_response(content_type, envp);
#else
			not_found_response(content_type);
#endif//!FAST_CGI_APP
			return;
		};
		sow_web_jsx::js_write_footer(stream);
		tr->t_source = stream.str();
		std::stringstream().swap(stream);
#if defined(FAST_CGI_APP)
		run__js_scrip(content_type, *ex_dir, *root_dir, method, *tr, envp);
#else
		run__js_scrip(content_type, *ex_dir, *root_dir, method, *tr, execute_path);
#endif//FAST_CGI_APP
		if (tr->is_error == true) {
			write_header(content_type);
			std::cout << "\r\n";
			std::cout << tr->err_msg.c_str();
			fflush(stdout);
		}
	}
	else {
		parser_settings* ps = new parser_settings();
		ps->dir = root_dir->data();
		ps->page_path = path_info;
		ntemplate_parse_x(*ps, *tr);
		if (tr->is_error == true) {
			if (tr->err_msg == "_NOT_FOUND_") {
#if defined(FAST_CGI_APP)
				not_found_response(content_type, envp);
#else
				not_found_response(content_type);
#endif//!FAST_CGI_APP
			}
			else {
				write_header(content_type);
				std::cout << "\r\n";
				std::cout << tr->err_msg.c_str();
				fflush(stdout);
			}
		}
		else if (tr->is_script_template == true) {
#if defined(FAST_CGI_APP)
			run__js_scrip(content_type, *ex_dir, *root_dir, method, *tr, envp);
#else
			run__js_scrip(content_type, *ex_dir, *root_dir, method, *tr, execute_path);
#endif//FAST_CGI_APP
			if (tr->is_error == true) {
				write_header(content_type);
				std::cout << "\r\n";
				std::cout << tr->err_msg.c_str();
				fflush(stdout);
			}
		}
		else {
			write_header(content_type);
			std::cout << "\r\n";
			SET_BINARY_MODE_OUT();
			std::cout << tr->t_source.c_str();
			std::string().swap(tr->t_source);
			fflush(stdout);
		}
		delete ps;
	}
	delete tr;
	delete ex_dir; delete ex_name; delete root_dir;

};
void get_args(int argc, char *argv[], std::vector<char*>&args) {
	for (int i = 2; i < argc; i++) {
		args.push_back(argv[i]);
	}
};
// No external changes needed. 
void prepare_console_response(int argc, char *argv[], bool ireq) {
	//4:37 AM 12/13/2018
	auto exec_path = new std::string();
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(*exec_path) < 0) {
		delete exec_path;
		std::cout << "Please add web_jsx bin path into environment variable Path!!!\r\n";
		fflush(stdout);
		return;
	}
#else
	exec_path->append(get_env_c("web_jsx_cgi"));
#endif
	exec_path->append("\\");
	int param_start = 1;
	if (ireq == true) {
		param_start = 2;
	}
	const char*may_be_path = const_cast<const char*>(argv[param_start]);
	web_extension ext = get_request_extension(may_be_path);
	if (ext == web_extension::JSX) {
		std::cout << "This extension not allowed for CLI";
		std::cout << " ==> ";
		std::cout << may_be_path;
		fflush(stdout);
		return;
	}
	auto root_dir = new std::string(get_current_working_dir());
	root_dir->append("\\");
	std::stringstream stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN) {
		sow_web_jsx::js_write_console_header(stream);
		stream << may_be_path;
	}
	else {
		auto scrip_path = new std::string(root_dir->data());
		scrip_path->append(may_be_path);
		const char* scrip_path_c = scrip_path->data();
		if (__file_exists(scrip_path_c) == false) {
			std::cout << "No file found in : " << scrip_path_c;
			delete root_dir; delete scrip_path; delete exec_path;
			fflush(stdout);
			return;
		}
		delete scrip_path;
		sow_web_jsx::js_write_console_header(stream);
		if (sow_web_jsx::read_file(may_be_path, stream, false) < 0) {
			std::cout << "Unable to read file : " << scrip_path_c;
			std::stringstream().swap(stream);
			delete root_dir; delete exec_path;
			fflush(stdout);
			return;
		};
	}

	sow_web_jsx::js_write_console_footer(stream);
	std::vector<char*>*argumnets = new std::vector<char*>();
	get_args(argc, argv, *argumnets);
	auto arg_array = new std::string("");
	json_array_stringify_s(*argumnets, *arg_array);
	auto script_source = new std::string(stream.str());
	std::stringstream().swap(stream);
	auto ctx = new std::map<std::string, std::string>();
	std::map<std::string, std::string>& global = *ctx;
	global["app_path"] = const_cast<const char*>(argv[0]);
	global["app_dir"] = exec_path->data();
	global["path"] = get_env_c("path");
	global["root_dir"] = root_dir->data();
	global["arg"] = arg_array->data();
	if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN)
		global["path_translated"] = "RAW_SCRIPT";
	else {
		global["path_translated"] = may_be_path;
	}
	sow_web_jsx::js_compiler::run_async(exec_path->data(), script_source->data(), *ctx);
	delete ctx;
	delete argumnets; delete script_source;
	delete root_dir; delete arg_array; delete exec_path;
	fflush(stdout);
};

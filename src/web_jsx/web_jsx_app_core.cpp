/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//4:02 PM 2/3/2019
#include "web_jsx_app_core.h"
void web_jsx_cgi::app_core::run__js_scrip(const char * content_type, std::string & root_dir, const app_ex_info aei, req_method & method, template_result & tr) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	web_jsx_cgi::cgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path);
	::read_query_string(*query_string, get_env_c("QUERY_STRING"));
	web_jsx_cgi::cgi_request::get_request_object(*req_obj, *query_string, method);
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	try {
		sow_web_jsx::js_compiler::create_engine(aei.ex_dir->c_str());
		sow_web_jsx::js_compiler::run_async(*ctx, aei.ex_dir->c_str(), tr);
		sow_web_jsx::js_compiler::dispose_engine();
	} catch (std::exception&e) {
		tr.is_error = true;
		tr.err_msg = e.what();
	}
	req_obj->clear(); global_obj->clear(); ctx->clear(); query_string->clear();
	free(req_obj); free(global_obj); free(ctx); free(query_string);
	std::string().swap(tr.t_source);
}
bool is_script_runtime_error(std::string& what) {
	std::size_t found = what.find("Error::");
	return found != std::string::npos;
}
void web_jsx_cgi::app_core::prepare_response(const char* content_type, const char*path_translated, const app_ex_info aei, req_method&method, const char* path_info) {
	web_extension ext = ::get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN
		|| ext == web_extension::RAW_SCRIPT
		|| ext == web_extension::JS
		) {
		::write_header(content_type);
		std::cout << "\r\n";
		std::cout << "Unsupported extension....";
		std::cout << path_translated;
		fflush(stdout);
		return;
	}
	std::string* root_dir = new std::string();
	::server_physical_path(path_translated, path_info, *root_dir);
	root_dir->append("\\");
	template_result* tr = new template_result();
	tr->remove_new_line = true;
	if (ext == web_extension::JSXH || ext == web_extension::WJSXH) {
		std::stringstream stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		sow_web_jsx::js_write_header(stream);
		size_t ret = sow_web_jsx::read_file(path_translated, stream, false);
		if (ret < 0 || ret == std::string::npos) {
			web_jsx_cgi::cgi_request::not_found_response(content_type);
			return;
		};
		sow_web_jsx::js_write_footer(stream);
		tr->t_source = stream.str();
		std::stringstream().swap(stream);
		web_jsx_cgi::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr);
		if (tr->is_error == true) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_header(content_type);
				std::cout << "\r\n";
				std::cout << tr->err_msg.c_str();
			}
			fflush(stdout);
		}
	}
	else {
		parser_settings* ps = new parser_settings();
		ps->dir = root_dir->c_str();
		ps->page_path = path_info;
		::ntemplate_parse_x(*ps, *tr);
		if (tr->is_error == true) {
			if (!is_script_runtime_error(tr->err_msg)) {
				if (tr->err_msg == "_NOT_FOUND_") {
					web_jsx_cgi::cgi_request::not_found_response(content_type);
					if (!tr->t_source.empty()) {
						std::cout << "Error==>\r\n";
						std::cout << tr->t_source;
					}
					fflush(stdout);
				}
				else {
					::write_header(content_type);
					std::cout << "\r\n";
					std::cout << tr->err_msg.c_str();
					fflush(stdout);
				}
			}
			else {
				fflush(stdout);
			}
			
		}
		else if (tr->is_script_template == true) {
			web_jsx_cgi::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr);
			if (tr->is_error == true) {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_header(content_type);
					std::cout << "\r\n";
					std::cout << tr->err_msg.c_str();
				}
				fflush(stdout);
			}
		}
		else {
			::write_header(content_type);
			std::cout << "\r\n";
			SET_BINARY_MODE_OUT();
			std::cout << tr->t_source.c_str();
			std::string().swap(tr->t_source);
			fflush(stdout);
		}
		delete ps;
	}
	std::string().swap(tr->err_msg);
	delete tr;
}
#if defined(FAST_CGI_APP)
void web_jsx_cgi::app_core::run__js_scrip(const char* content_type, std::string&root_dir, const app_ex_info aei, req_method&method, template_result& tr, const char*env_path, char **envp) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	const char* req_query_string = web_jsx_cgi::fcgi_request::freq_env_c("QUERY_STRING", envp);
	web_jsx_cgi::fcgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path, env_path, envp);
	read_query_string(*query_string, req_query_string);
	web_jsx_cgi::fcgi_request::get_request_object(*req_obj, *query_string, method, content_type, envp);
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	try {
		sow_web_jsx::js_compiler::run_async(*ctx, aei.ex_dir->c_str(), tr);
	} catch (std::exception&e) {
		tr.is_error = true;
		tr.err_msg = e.what();
	}
	req_obj->clear(); global_obj->clear(); query_string->clear(); ctx->clear();
	free(req_obj); free(global_obj); free(query_string); free(ctx);
	std::string().swap(tr.t_source);
}
void web_jsx_cgi::app_core::prepare_response(const char* content_type, const char*path_translated, const app_ex_info aei, req_method&method, const char* path_info, const char*env_path, char **envp) {
	web_extension ext = ::get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN
		|| ext == web_extension::RAW_SCRIPT
		|| ext == web_extension::JS
		) {
		::write_header(content_type);
		std::cout << "\r\n";
		std::cout << "Unsupported extension....";
		std::cout << path_translated;
		fflush(stdout);
		return;
	}
	std::string* root_dir = new std::string();
	::server_physical_path(path_translated, path_info, *root_dir);
	root_dir->append("\\");
	template_result* tr = new template_result();
	tr->remove_new_line = true;
	if (ext == web_extension::JSXH || ext == web_extension::WJSXH) {
		std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		sow_web_jsx::js_write_header(js_stream);
		size_t ret = sow_web_jsx::read_file(path_translated, js_stream, false);
		if (ret < 0 || ret == std::string::npos) {
			web_jsx_cgi::fcgi_request::not_found_response(content_type, envp);
			return;
		};
		sow_web_jsx::js_write_footer(js_stream);
		tr->t_source = js_stream.str();
		std::stringstream().swap(js_stream);
		web_jsx_cgi::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr, env_path, envp);
		if (tr->is_error == true) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_header(content_type);
				std::cout << "\r\n";
				std::cout << tr->err_msg.c_str();
			}
			fflush(stdout);
		}
	}
	else {
		parser_settings* ps = new parser_settings();
		ps->dir = root_dir->c_str();
		ps->page_path = path_info;
		ntemplate_parse_x(*ps, *tr);
		if (tr->is_error == true) {
			if (tr->err_msg == "_NOT_FOUND_") {
				web_jsx_cgi::fcgi_request::not_found_response(content_type, envp);
				if (!tr->t_source.empty()) {
					std::cout << "Error==>\r\n";
					std::cout << tr->t_source;
				}
				fflush(stdout);
			}
			else {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_header(content_type);
					std::cout << "\r\n";
					std::cout << tr->err_msg.c_str();
				}
				fflush(stdout);
			}
		}
		else if (tr->is_script_template == true) {
			web_jsx_cgi::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr, env_path, envp);
			//::write_header(content_type);
			//std::cout << "\r\n";
			//std::cout << tr->t_source.c_str();
			//std::string().swap(tr->t_source);
			//fflush(stdout);
			if (tr->is_error == true) {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_header(content_type);
					std::cout << "\r\n";
					std::cout << tr->err_msg.c_str();
				}
				fflush(stdout);
			}
		}
		else {
			::write_header(content_type);
			std::cout << "\r\n";
			SET_BINARY_MODE_OUT();
			std::cout << tr->t_source.c_str();
			std::string().swap(tr->t_source);
			fflush(stdout);
		}
		free(ps);
	}
	std::string().swap(tr->err_msg);
	free(tr);
}
#endif//FAST_CGI_APP
void get_args(int argc, char *argv[], std::vector<char*>&args) {
	for (int i = 2; i < argc; i++) {
		args.push_back(argv[i]);
	}
}
void web_jsx_cgi::app_core::prepare_console_response(int argc, char *argv[], bool ireq) {
	//4:37 AM 12/13/2018
	std::string* exec_path = new std::string();
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(*exec_path) < 0) {
		free(exec_path);
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
		//writ("IREQ");
	}
	const char*may_be_path = const_cast<const char*>(argv[param_start]);
	web_extension ext = get_request_extension(may_be_path);
	//writx(may_be_path);
	//write_argvx(argv, may_be_path);
	if (ext == web_extension::JSX || ext == web_extension::WJSX) {
		//writ("JSX");
		std::cout << "This extension not allowed for CLI";
		std::cout << " ==> ";
		std::cout << may_be_path;
		fflush(stdout);
		return;
	}
	auto root_dir = new std::string(get_current_working_dir());
	root_dir->append("\\");
	//writ(*root_dir);
	std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN) {
		sow_web_jsx::js_write_console_header(js_stream);
		js_stream << may_be_path;
	}
	else {
		auto scrip_path = new std::string(root_dir->c_str());
		scrip_path->append(may_be_path);
		const char* scrip_path_c = scrip_path->c_str();
		if (__file_exists(scrip_path_c) == false) {
			std::cout << "No file found in : " << scrip_path_c;
			free(root_dir); free(scrip_path); free(exec_path);
			fflush(stdout);
			return;
		}
		free(scrip_path);
		sow_web_jsx::js_write_console_header(js_stream);
		size_t ret = sow_web_jsx::read_file(may_be_path, js_stream, false);
		if (ret < 0 || ret == std::string::npos) {
			std::cout << "Unable to read file : " << scrip_path_c;
			std::stringstream().swap(js_stream);
			free(root_dir); free(exec_path);
			fflush(stdout);
			return;
		};
	}
	try {
		sow_web_jsx::js_write_console_footer(js_stream);
		std::vector<char*>*argumnets = new std::vector<char*>();
		get_args(argc, argv, *argumnets);
		auto arg_array = new std::string("");
		json_array_stringify_s(*argumnets, *arg_array);
		auto script_source = new std::string(js_stream.str());
		std::stringstream().swap(js_stream);
		auto ctx = new std::map<std::string, std::string>();
		std::map<std::string, std::string>& global = *ctx;
		global["app_path"] = const_cast<const char*>(argv[0]);
		global["app_dir"] = exec_path->c_str();
		global["is_interactive"] = ireq == true ? "0" : "1";
		global["path"] = get_env_c("path");
		global["root_dir"] = root_dir->c_str();
		global["arg"] = arg_array->c_str();
		if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN)
			global["path_translated"] = "RAW_SCRIPT";
		else {
			global["path_translated"] = may_be_path;
		}
		sow_web_jsx::js_compiler::run_async(exec_path->c_str(), script_source->c_str(), *ctx);
		ctx->clear(); argumnets->clear();
		free(ctx); free(argumnets); free(script_source);
		free(root_dir); free(arg_array); free(exec_path);
		fflush(stdout);
	} catch (std::exception&e) {
		std::cout << e.what();
		//writx(e.what());
	}  catch (...) {
		std::cout << "Unknown";
		//writx("Unknown");
	}
}
void web_jsx_cgi::app_core::free_app_info(app_ex_info * aei) {
	free(aei->ex_dir);
	free(aei->ex_name);
	free(aei);
}

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//4:02 PM 2/3/2019
#	include "core/web_jsx_global.h"
#	include "web_jsx_app_core.h"
#	include "fcreqh.h"
#	include "creqh.h"
#	include "core/script_tag_parser.h"
#	include "core/js_compiler.h"
#	include "core/web_jsx_exp.h"
#	include "core/js_polyfill.h"
void web_jsx::app_core::run__js_scrip(
	const char * content_type, 
	std::string & root_dir, const app_ex_info aei, 
	req_method & method, template_result & tr
) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	web_jsx::cgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path);
	::read_query_string(*query_string, get_env_c("QUERY_STRING"));
	web_jsx::cgi_request::get_request_object(*req_obj, *query_string, method);
	(*global_obj)["app_dir"] = aei.ex_dir->c_str();
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	sow_web_jsx::js_compiler::create_engine(aei.ex_dir->c_str());
	sow_web_jsx::js_compiler::run_async(*ctx, aei.ex_dir->c_str(), tr);
	sow_web_jsx::js_compiler::dispose_engine();
	_free_obj(req_obj); _free_obj(global_obj); _free_obj(ctx); _free_obj(query_string);
	std::string().swap(tr.t_source);
}
bool is_script_runtime_error(std::string& what) {
	std::size_t found = what.find("Error::");
	return found != std::string::npos;
}
void web_jsx::app_core::prepare_response(const char* content_type,
	const char*path_translated, 
	const app_ex_info aei, 
	req_method&method, 
	const char* path_info
) {
	web_extension ext = ::get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN
		|| ext == web_extension::RAW_SCRIPT
		|| ext == web_extension::JS
		) {
		::write_internal_server_error(
			/*content_type*/content_type,
			/*execute_path*/aei.ex_dir->c_str(),
			/*error_code*/500,
			/*error_msg*/"Unsupported extension...."
		);
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
		if (ret == std::string::npos) {
			web_jsx::cgi_request::not_found_response(content_type);
			_free_obj(root_dir); delete tr;
			return;
		}
		//sow_web_jsx::js_write_footer(stream);
		tr->t_source = stream.str();
		std::stringstream().swap(stream);
		web_jsx::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr);
		if (tr->is_error == true) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_internal_server_error(
					/*content_type*/content_type,
					/*execute_path*/aei.ex_dir->c_str(),
					/*error_code*/500,
					/*error_msg*/tr->err_msg.c_str()
				);
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
					web_jsx::cgi_request::not_found_response(content_type);
					/*write_header("text/plain");
					std::cout << H_N_L;
					std::cout << tr->err_msg.c_str() << H_N_L;
					std::cout << tr->t_source.c_str() << H_N_L;*/
					tr->err_msg.clear();
					tr->t_source.clear();
					fflush(stdout);
				}
				else {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str()
					);
				}
			}
			else {
				fflush(stdout);
			}
			
		}
		else if (tr->is_script_template == true) {
			web_jsx::app_core::run__js_scrip(content_type, *root_dir, aei, method, *tr);
			if (tr->is_error == true) {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str()
					);
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
	delete tr; _free_obj(root_dir);
}
#if defined(FAST_CGI_APP)
void web_jsx::app_core::run__js_scrip(
	const char* content_type,
	std::string&root_dir,
	const app_ex_info aei,
	req_method&method,
	template_result& tr,
	const char*env_path, char **envp
) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	const char* req_query_string = web_jsx::fcgi_request::freq_env_c("QUERY_STRING", envp);
	web_jsx::fcgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path, env_path, envp);
	read_query_string(*query_string, req_query_string);
	web_jsx::fcgi_request::get_request_object(*req_obj, *query_string, method, content_type, envp);
	(*global_obj)["app_dir"] = aei.ex_dir->c_str();
	(*global_obj)["app_name"] = aei.ex_name->c_str();
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	sow_web_jsx::js_compiler::run_async(*ctx, aei.ex_dir->c_str(), tr);
	_free_obj(req_obj); _free_obj(global_obj); _free_obj(ctx); _free_obj(query_string);
	std::string().swap(tr.t_source);
}
void web_jsx::app_core::prepare_response(
	const char* content_type,
	const char*path_translated,
	const app_ex_info aei, 
	req_method&method,
	const char* path_info,
	const char*env_path, 
	char **envp
) {
	web_extension ext = ::get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN
		|| ext == web_extension::RAW_SCRIPT
		|| ext == web_extension::JS
		) {
		::write_internal_server_error(
			/*content_type*/content_type,
			/*execute_path*/aei.ex_dir->c_str(),
			/*error_code*/500,
			/*error_msg*/"Unsupported extension...."
		);
		return;
	}
	std::string* root_dir = new std::string(web_jsx::fcgi_request::freq_env_c("DOCUMENT_ROOT", envp));
	root_dir->append("\\");
	template_result* tr = new template_result();
	tr->remove_new_line = true;
	const char* ct = web_jsx::fcgi_request::freq_env_c("CONTENT_TYPE", envp);
	if (ext == web_extension::JSXH || ext == web_extension::WJSXH) {
		std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		sow_web_jsx::js_write_header(js_stream);
		size_t ret = sow_web_jsx::read_file(path_translated, js_stream, false);
		if (ret == std::string::npos) {
			web_jsx::fcgi_request::not_found_response(content_type, envp, aei.ex_dir->c_str());
			_free_obj(root_dir); delete tr;
			return;
		}
		tr->t_source = js_stream.str();
		std::stringstream().swap(js_stream);
		web_jsx::app_core::run__js_scrip(ct, *root_dir, aei, method, *tr, env_path, envp);
		if (tr->is_error == true) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_internal_server_error(
					/*content_type*/content_type,
					/*execute_path*/aei.ex_dir->c_str(),
					/*error_code*/500,
					/*error_msg*/tr->err_msg.c_str()
				);
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
				web_jsx::fcgi_request::not_found_response(content_type, envp, aei.ex_dir->c_str());
				fflush(stdout);
			}
			else {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str()
					);
				}
				fflush(stdout);
			}
		}
		else if (tr->is_script_template == true) {
			web_jsx::app_core::run__js_scrip(ct, *root_dir, aei, method, *tr, env_path, envp);
			if (tr->is_error == true) {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str()
					);
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
	delete tr; _free_obj(root_dir);
}
#endif//FAST_CGI_APP
void get_args(int argc, char *argv[], std::vector<char*>&args) {
	for (int i = 2; i < argc; i++) {
		args.push_back(argv[i]);
	}
}
void web_jsx::app_core::prepare_console_response(int argc, char *argv[], int is_internal_request) {
	//4:37 AM 12/13/2018
	std::string* exec_path = new std::string();
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(*exec_path) < 0) {
		_free_obj(exec_path);
		std::cout << "Please add web_jsx bin path into environment variable Path!!!\r\n";
		fflush(stdout);
		return;
	}
#else
	exec_path->append(get_env_c("web_jsx"));
#endif
	if (exec_path->find_last_of("\\") == std::string::npos) {
		exec_path->append("\\");
	}
	exec_path->append("web_jsx.exe");
	int param_start = 1;
	if (is_internal_request == TRUE) {
		param_start = 2;
	}
	const char*may_be_path = const_cast<const char*>(argv[param_start]);
	web_extension ext = get_request_extension(may_be_path);
	if (ext == web_extension::JSX || ext == web_extension::WJSX) {
		std::cout << "This extension not allowed for CLI ==> " << may_be_path;
		_free_obj(exec_path);
		fflush(stdout);
		return;
	}
	std::string* root_dir = new std::string(get_current_working_dir());
	root_dir->append("\\");
	std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN) {
		sow_web_jsx::js_write_console_header(js_stream);
		js_stream << may_be_path;
	}
	else {
		std::string* scrip_path = new std::string(root_dir->c_str());
		scrip_path->append(may_be_path);
		const char* scrip_path_c = scrip_path->c_str();
		if (__file_exists(scrip_path_c) == false) {
			std::cout << "No file found in : " << scrip_path_c;
			_free_obj(root_dir); _free_obj(scrip_path); _free_obj(exec_path);
			fflush(stdout);
			return;
		}
		sow_web_jsx::js_write_console_header(js_stream);
		size_t ret = sow_web_jsx::read_file(scrip_path_c, js_stream, false);
		if (ret == std::string::npos) {
			std::cout << "Unable to read file : " << scrip_path_c;
			std::stringstream().swap(js_stream);
			_free_obj(root_dir); _free_obj(exec_path); _free_obj(scrip_path);
			fflush(stdout);
			return;
		}
		_free_obj(scrip_path);
	}
	//sow_web_jsx::js_write_console_footer(js_stream);
	std::vector<char*>* argumnets = new std::vector<char*>();
	get_args(argc, argv, *argumnets);
	std::string* arg_array = new std::string();
	json_array_stringify_s(*argumnets, *arg_array);
	std::string* script_source = new std::string(js_stream.str());
	std::stringstream().swap(js_stream);
	auto ctx = new std::map<std::string, std::string>();
	std::map<std::string, std::string>& global = *ctx;

	std::string*ex_dir = new std::string();
	std::string*ex_name = new std::string();
	request_file_info(*exec_path, *ex_dir, *ex_name);
	ex_dir->append("\\");
	global["app_path"] = exec_path->c_str();
	global["app_dir"] = ex_dir->c_str();
	global["app_name"] = ex_name->c_str();
	global["is_interactive"] = is_internal_request == TRUE ? "0" : "1";
	global["path"] = get_env_c("path");
	global["root_dir"] = root_dir->c_str();
	global["arg"] = arg_array->c_str();
	if (ext == web_extension::RAW_SCRIPT || ext == web_extension::UNKNOWN)
		global["path_translated"] = "RAW_SCRIPT";
	else {
		global["path_translated"] = may_be_path;
	}
	sow_web_jsx::js_compiler::run_async(exec_path->c_str(), script_source->c_str(), *ctx);
	_free_obj(ctx); _free_obj(argumnets); _free_obj(script_source);
	_free_obj(root_dir); _free_obj(arg_array); _free_obj(exec_path);
	_free_obj(ex_dir); _free_obj(ex_name);
	fflush(stdout);
}
void web_jsx::app_core::free_app_info(app_ex_info * aei) {
	_free_obj(aei->ex_dir);
	_free_obj(aei->ex_name);
	aei->execute_path = NULL;
	delete aei; aei = NULL;
}

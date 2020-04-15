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

using namespace sow_web_jsx;
void app_core::run__js_scrip(
	v8::Isolate* isolate,
	const char * content_type, 
	std::string& root_dir, const app_ex_info aei, 
	const req_method method, template_result & tr,
	wjsx_env* wj_env, js_script_inf* jsi
) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	::cgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path);
	_NEW_STR(query_str);
	if (get_env_c("QUERY_STRING", *query_str) == TRUE) {
		::read_query_string(*query_string, *query_str);
	}
	_free_obj(query_str);
	::cgi_request::get_request_object(*req_obj, *query_string, method);
	(*global_obj)["app_dir"] = aei.ex_dir->c_str();
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	jsi->is_raw_script = FALSE;
	::js_compiler::run_script_async(
		isolate, *ctx, tr, wj_env, *jsi
	);
	swap_obj(tr.t_source);
	_free_obj(req_obj); _free_obj(global_obj); _free_obj(ctx); _free_obj(query_string);
}
bool is_script_runtime_error(std::string& what) {
	std::size_t found = what.find("Error::");
	return found != std::string::npos;
}
void wjsx_write_cache(
	const char* path_translated, template_result* tr
) {
	if (tr->is_error == FALSE) {
		if (tr->has_wjsx_template == TRUE || tr->is_script_template == TRUE) {
			::write_file(path_translated, tr->t_source.c_str(), tr->t_source.size());
		}
	}
}
int wjsx_serve_from_cached(
	const char* mpath, const char* cpath, template_result* tr,
	int check_file_state = FALSE
) {
	if (__file_exists(cpath) == false)return FALSE;
	if (check_file_state == TRUE) {
		if (file_has_changed(mpath, cpath) == TRUE) {
			std::remove(cpath);
			return FALSE;
		}
	}
	
	_NEW_STR(body);
	int ret = ::read_file(cpath, *body);
	if (ret == TRUE) {
		size_t index = body->find_first_of("\n");
		if (index == std::string::npos) {
			tr->is_script_template = 0;
		}
		else {
			tr->is_script_template = body->substr(0, index).find("__web_jsx_script__") != std::string::npos;
		}
		body->swap(tr->t_source);
	}
	_free_obj(body);
	return ret;
}
void app_core::prepare_response(
	v8::Isolate* isolate,
	const char* content_type,
	const char*path_translated, 
	const app_ex_info aei, 
	const req_method method,
	const char* path_info,
	wjsx_env* wj_env
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
			/*error_msg*/"Unsupported extension....",
			/*wjsx_env*/wj_env
		);
		return;
	}
	_NEW_STR(root_dir);
	::server_physical_path(path_translated, path_info, *root_dir);
	root_dir->append("\\");
	template_result* tr = new template_result;
	tr->remove_new_line = TRUE; tr->is_error = FALSE;
	wj_env->set_root_dir(root_dir->c_str());
	wj_env->set_app_dir(aei.ex_dir->c_str());
	if (ext == web_extension::JSXH || ext == web_extension::WJSXH) {
		std::stringstream stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		sow_web_jsx::js_write_header(stream);
		int ret = ::read_file(path_translated, stream);
		if (ret == FALSE) {
			swap_obj(stream);
			::cgi_request::not_found_response(content_type);
			_free_obj(root_dir); delete tr;
			return;
		}
		tr->t_source = std::string(stream.str().c_str());
		swap_obj(stream);
		js_script_inf* jsi = new js_script_inf;
		jsi->abs_path = path_translated;
		jsi->path_info = path_info;
		::app_core::run__js_scrip(
			isolate, content_type, *root_dir, aei, method, *tr, wj_env, jsi
		);
		if (tr->is_error == TRUE) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_internal_server_error(
					/*content_type*/content_type,
					/*execute_path*/aei.ex_dir->c_str(),
					/*error_code*/500,
					/*error_msg*/tr->err_msg.c_str(),
					/*wjsx_env*/wj_env
				);
			}
			_WFLUSH();
		}
	}
	else {
		_NEW_STRA(ppage_path, path_translated);
		ppage_path->append("p");
		int is_cached = ::wjsx_serve_from_cached(path_translated, ppage_path->c_str(), tr, wj_env->check_file_state);
		if (is_cached == FALSE) {
			parser_settings* ps = new parser_settings();
			ps->dir = root_dir->c_str();
			ps->page_path = path_info;
			::ntemplate_parse_x(*ps, *tr);
			delete ps;
			::wjsx_write_cache(ppage_path->c_str(), tr);
		}
		_free_obj(ppage_path);
		if (tr->is_error == TRUE) {
			if (!::is_script_runtime_error(tr->err_msg)) {
				if (tr->err_msg == "_NOT_FOUND_") {
					::cgi_request::not_found_response(content_type);
					tr->err_msg.clear();
					tr->t_source.clear();
					_WFLUSH();
				}
				else {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str(),
						/*wjsx_env*/wj_env
					);
				}
			}
			else {
				_WFLUSH();
			}
			
		}
		else if (tr->is_script_template == TRUE) {
			js_script_inf* jsi = new js_script_inf;
			jsi->abs_path = path_translated;
			jsi->path_info = path_info;
			::app_core::run__js_scrip(
				isolate, content_type, *root_dir, aei, method, *tr, wj_env, jsi
			);
			if (tr->is_error == TRUE) {
				if (!is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str(),
						/*wjsx_env*/wj_env
					);
				}
				_WFLUSH();
			}
		}
		else {
			::write_header(content_type, wj_env);
			_WCOUT << "\r\n";
			_WCOUT << tr->t_source.c_str();
			swap_obj(tr->t_source);
			_WFLUSH();
		}
	}
	swap_obj(tr->err_msg);
	delete tr; _free_obj(root_dir);
}

void app_core::run__js_scrip(
	v8::Isolate* isolate,
	const char* content_type,
	std::string&root_dir,
	const app_ex_info aei,
	const req_method method,
	template_result& tr,
	const char*env_path, char **envp,
	wjsx_env* wj_env,
	js_script_inf* jsi
) {
	std::map<std::string, std::map<std::string, std::string>>* ctx = new std::map<std::string, std::map<std::string, std::string>>();
	std::map<std::string, std::string>* req_obj = new std::map<std::string, std::string>();
	std::map<std::string, std::string>*query_string = new std::map<std::string, std::string>();
	std::map<std::string, std::string>* global_obj = new std::map<std::string, std::string>();
	_NEW_STR(req_query_string);
	if (::fcgi_request::freq_env_c("QUERY_STRING", envp, *req_query_string) == TRUE) {
		::read_query_string(*query_string, *req_query_string);
	}
	_free_obj(req_query_string);
	::fcgi_request::get_global_obj(*global_obj, root_dir, aei.execute_path, env_path, envp);
	::fcgi_request::get_request_object(*req_obj, *query_string, method, content_type, envp);
	(*global_obj)["app_dir"] = aei.ex_dir->c_str();
	(*global_obj)["app_name"] = aei.ex_name->c_str();
	::obj_insert(*req_obj, "request", *ctx);
	::obj_insert(*global_obj, "global", *ctx);
	jsi->is_raw_script = FALSE;
	if (wj_env->is_thread_req() == FALSE) {
		js_compiler::run_script_async(
			isolate, *ctx, tr, wj_env, *jsi
		);
	}
	else {
		js_compiler::run_script(
			isolate, *ctx, tr, wj_env, *jsi
		);
	}
	_free_obj(req_obj); _free_obj(global_obj); _free_obj(ctx); _free_obj(query_string);
	swap_obj(tr.t_source);
}
void app_core::prepare_response(
	v8::Isolate* isolate,
	const char* content_type,
	const char*path_translated,
	const app_ex_info aei, 
	const req_method method,
	const char* path_info,
	const char*env_path, 
	char **envp,
	wjsx_env* wj_env
) {
	//v8::internal::max
	web_extension ext = ::get_request_extension(path_translated);
	if (ext == web_extension::UNKNOWN
		|| ext == web_extension::RAW_SCRIPT
		|| ext == web_extension::JS
		) {
		::write_internal_server_error(
			/*content_type*/content_type,
			/*execute_path*/aei.ex_dir->c_str(),
			/*error_code*/500,
			/*error_msg*/"Unsupported extension....",
			/*wjsx_env*/wj_env
		);
		return;
	}
	_NEW_STR(root_dir);
	if (::fcgi_request::freq_env_c("DOCUMENT_ROOT", envp, *root_dir) == FALSE) {
		_free_obj(root_dir);
		::write_internal_server_error(
			/*content_type*/content_type,
			/*execute_path*/aei.ex_dir->c_str(),
			/*error_code*/500,
			/*error_msg*/"Unable to read DOCUMENT_ROOT environment variable...",
			/*wjsx_env*/wj_env
		);
		return;
	}
	root_dir->append("\\");
	template_result* tr = new template_result();
	tr->remove_new_line = true;
	_NEW_STR(ct);
	if (::fcgi_request::freq_env_c("CONTENT_TYPE", envp, *ct) == FALSE) {
		ct->append("text/html");
	}
	wj_env->set_root_dir(root_dir->c_str());
	wj_env->set_app_dir(aei.ex_dir->c_str());
	if (ext == web_extension::JSXH || ext == web_extension::WJSXH) {
		std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		sow_web_jsx::js_write_header(js_stream);
		int ret = ::read_file(path_translated, js_stream);
		if (ret == FALSE) {
			swap_obj(js_stream);
			::fcgi_request::not_found_response(
				content_type, envp, aei.ex_dir->c_str(), wj_env
			);
			_free_obj(root_dir); delete tr; _free_obj(ct);
			return;
		}
		tr->t_source = std::string(js_stream.str().c_str());
		swap_obj(js_stream);
		js_script_inf* jsi = new js_script_inf;
		jsi->abs_path = path_translated;
		jsi->path_info = path_info;
		::app_core::run__js_scrip(
			isolate, ct->c_str(), *root_dir, aei, method, *tr, env_path, envp, wj_env, jsi
		);
		if (tr->is_error == TRUE) {
			if (!is_script_runtime_error(tr->err_msg)) {
				::write_internal_server_error(
					/*content_type*/content_type,
					/*execute_path*/aei.ex_dir->c_str(),
					/*error_code*/500,
					/*error_msg*/tr->err_msg.c_str(),
					/*wjsx_env*/wj_env
				);
			}
			_WFLUSH();
		}
	}
	else {
		_NEW_STRA(ppage_path, path_translated);
		ppage_path->append("p");
		int is_cached = ::wjsx_serve_from_cached(path_translated, ppage_path->c_str(), tr, wj_env->check_file_state);
		if (is_cached == FALSE) {
			parser_settings* ps = new parser_settings();
			ps->dir = root_dir->c_str();
			ps->page_path = path_info;
			::ntemplate_parse_x(*ps, *tr);
			delete ps;
			::wjsx_write_cache(ppage_path->c_str(), tr);
		}
		_free_obj(ppage_path);
		if (tr->is_error == TRUE) {
			if (tr->err_msg == "_NOT_FOUND_") {
				::fcgi_request::not_found_response(
					content_type, envp, aei.ex_dir->c_str(), wj_env
				);
				_WFLUSH();
			}
			else {
				if (!::is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str(),
						/*wjsx_env*/wj_env
					);
				}
				_WFLUSH();
			}
		}
		else if (tr->is_script_template == TRUE) {
			js_script_inf* jsi = new js_script_inf;
			jsi->abs_path = path_translated;
			jsi->path_info = path_info;
			::app_core::run__js_scrip(
				isolate, ct->c_str(), *root_dir, aei, method, *tr, env_path, envp, wj_env, jsi
			);
			if (tr->is_error == TRUE) {
				if (!::is_script_runtime_error(tr->err_msg)) {
					::write_internal_server_error(
						/*content_type*/content_type,
						/*execute_path*/aei.ex_dir->c_str(),
						/*error_code*/500,
						/*error_msg*/tr->err_msg.c_str(),
						/*wjsx_env*/wj_env
					);
				}
				_WFLUSH();
			}
		}
		else {
			::write_header(content_type, wj_env);
			_WCOUT << "\r\n";
			_WCOUT << tr->t_source.c_str();
			swap_obj(tr->t_source);
			_WFLUSH();
		}
	}
	swap_obj(tr->err_msg); delete tr; 
	_free_obj(root_dir); _free_obj(ct);
}
void get_args(int argc, char *argv[], std::vector<char*>&args) {
	for (int i = 2; i < argc; i++) {
		args.push_back(argv[i]);
	}
}
#include "core/v8_engine.h"
void app_core::prepare_console_response(
	int argc, char *argv[], int is_internal_request
) {
	//4:37 AM 12/13/2018
	_NEW_STR(exec_path);
	if (get_env_path(*exec_path) < 0) {
		_free_obj(exec_path);
		fprintf_s(_stdout, "Please add web_jsx bin path into environment variable Path!!!\n");
		fflush(stdout);
		return;
	}
	exec_path->append("\\");
	exec_path->append("web_jsx.exe");
	int param_start = 1;
	if (is_internal_request == TRUE) {
		param_start = 2;
	}
	const char*may_be_path = const_cast<const char*>(argv[param_start]);
	web_extension ext = ::get_request_extension(may_be_path);
	if (ext == JSX || ext == WJSX) {
		fprintf_s(_stdout, "This extension not allowed for CLI ==>%s ", may_be_path);
		_free_obj(exec_path);
		fflush(stdout);
		return;
	}
	_NEW_STR(root_dir);
	if (::get_current_working_dir(*root_dir) == FALSE) {
		FATAL("Unable to find current working directory....");
	}
	root_dir->append("\\");
	std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	_NEW_STR(scrip_path);
	if (ext == RAW_SCRIPT || ext == UNKNOWN) {
		::js_write_console_header(js_stream);
		js_stream << may_be_path;
	}
	else {
		scrip_path->append(root_dir->c_str());
		scrip_path->append(may_be_path);
		const char* scrip_path_c = scrip_path->c_str();
		if (__file_exists(scrip_path_c) == false) {
			fprintf_s(_stdout, "No file found in : %s", scrip_path_c);
			_free_obj(root_dir); _free_obj(scrip_path); _free_obj(exec_path);
			fflush(stdout);
			return;
		}
		::js_write_console_header(js_stream);
		int ret = ::read_file(scrip_path_c, js_stream);
		if (ret == std::string::npos) {
			fprintf_s(_stdout, "Unable to read file : %s", scrip_path_c);
			swap_obj(js_stream);
			_free_obj(root_dir); _free_obj(exec_path); _free_obj(scrip_path);
			fflush(stdout);
			return;
		}
	}
	std::vector<char*>* argumnets = new std::vector<char*>();
	::get_args(argc, argv, *argumnets);
	_NEW_STR(arg_array);
	::json_array_stringify_s(*argumnets, *arg_array);
	template_result* tr = new template_result;
	tr->t_source = std::string(js_stream.str());
	tr->is_error = FALSE; ::swap_obj(js_stream);
	auto ctx = new std::map<std::string, std::string>();
	std::map<std::string, std::string>& global = *ctx;
	_NEW_STR(ex_dir); _NEW_STR(ex_name);
	::request_file_info(*exec_path, *ex_dir, *ex_name);
	ex_dir->append("\\");
	global["app_path"] = exec_path->c_str();
	global["app_dir"] = ex_dir->c_str();
	global["app_name"] = ex_name->c_str();
	global["is_interactive"] = is_internal_request == TRUE ? "0" : "1";
	get_env_c("path", global["path"]);
	global["root_dir"] = root_dir->c_str();
	global["arg"] = arg_array->c_str();
	js_script_inf* jsi = new js_script_inf;
	jsi->is_raw_script = TRUE;
	if (ext == RAW_SCRIPT || ext == UNKNOWN)
		global["path_translated"] = "RAW_SCRIPT";
	else {
		global["path_translated"] = may_be_path;
		jsi->path_info = may_be_path;
		jsi->abs_path = scrip_path->c_str();
		jsi->is_raw_script = FALSE;
	}
	::v8_engine::create_engine(exec_path->c_str());
	v8::Isolate* isolate = ::v8_engine::create_isolate();
	wjsx_env* wj_env = new wjsx_env(is_internal_request == TRUE ? FALSE : TRUE, FALSE);
	wj_env->compiled_cached = FALSE; //is_compiled_cached();
	wj_env->check_file_state = FALSE; //is_check_file_state();
	wj_env->set_root_dir(root_dir->c_str());
	wj_env->set_app_dir(ex_dir->c_str());
	::js_compiler::run_script_async(
		isolate, *ctx, *tr, wj_env, *jsi
	);
	swap_obj(tr->t_source);
	if (tr->is_error == TRUE) {
		fprintf_s(_stdout, tr->err_msg.c_str());
	}
	delete wj_env; delete tr;
	_free_obj(ctx); _free_obj(argumnets); //_free_obj(script_source);
	_free_obj(root_dir); _free_obj(arg_array); _free_obj(exec_path);
	_free_obj(ex_dir); _free_obj(ex_name); _free_obj(scrip_path);
	fflush(stdout);
}
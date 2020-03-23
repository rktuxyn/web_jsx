/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:09 AM 11/18/2018 START
//4:11 AM 11/18/2018 END
#	include "web_jsx_cgi.h"
#	include "core/web_jsx_global.h"
#	include "web_jsx_app_core.h"
#	include "creqh.h"
#	include "core/wjsx_env.h"
#	include "core/v8_engine.h"
using namespace sow_web_jsx;
int cgi_request::request_handler(const char*execute_path) {
	_NEW_STR(content_type);
	if (get_env_c("CONTENT_TYPE", *content_type) == FALSE) {
		content_type->append("text/html");
	}
	_NEW_STR(path_translated);
	if (get_env_c("PATH_TRANSLATED", *path_translated) == FALSE) {
		::cgi_request::not_found_response(content_type->c_str());
		_free_obj(content_type); _free_obj(path_translated);
		fflush(stdout);
		return EXIT_FAILURE;
	}
	if (__file_exists(path_translated->c_str()) == false) {
		::cgi_request::not_found_response(content_type->c_str());
		_free_obj(content_type); _free_obj(path_translated);
		fflush(stdout);
		return EXIT_FAILURE;
	}
	wjsx_env* wj_env = new wjsx_env(&std::cin, &std::cout, &std::cerr);
	req_method method = determine_req_method();
	if (method == req_method::UNSUPPORTED) {
		write_header(content_type->c_str(), wj_env);
		_free_obj(content_type); _free_obj(path_translated);
		std::cout << "\r\n";
		std::cout << "This method " << get_env("REQUEST_METHOD") << "not supported!!!" << "\r\n";
		fflush(stdout);
		_free_obj(wj_env);
		return EXIT_FAILURE;
	}
	app_ex_info* aei = new app_ex_info();//(app_ex_info*)malloc(sizeof(app_ex_info));
	aei->ex_dir = new std::string();
	aei->ex_name = new std::string();
	aei->execute_path = execute_path;
	request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
	aei->ex_dir->append("\\");
	::v8_engine::create_engine(aei->ex_dir->c_str());
	v8::Isolate* isolate = ::v8_engine::create_isolate();
	wj_env->compiled_cached = is_compiled_cached();
	_NEW_STR(path_info);
	get_env_c("PATH_INFO", *path_info);
	::app_core::prepare_response(
		isolate, content_type->c_str(), 
		path_translated->c_str(), *aei, method, 
		path_info->c_str(),
		wj_env
	);
	_free_obj(wj_env); _free_app_info(aei); _free_obj(path_info);
	_free_obj(content_type); _free_obj(path_translated);
	_dispose_isolate(isolate);
	::v8_engine::dispose_engine();
	return EXIT_SUCCESS;
}
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
int web_jsx::cgi_request::request_handler(const char*execute_path) {
	const char* content_type = get_content_type();
	const char* path_translated = get_env_c("PATH_TRANSLATED");
	if (__file_exists(path_translated) == false) {
		web_jsx::cgi_request::not_found_response(content_type);
		fflush(stdout);
		return EXIT_FAILURE;
	}
	req_method method = determine_req_method();
	if (method == req_method::UNSUPPORTED) {
		write_header(content_type);
		std::cout << "\r\n";
		std::cout << "This method " << get_env("REQUEST_METHOD") << "not supported!!!" << "\r\n";
		fflush(stdout);
		return EXIT_FAILURE;
	}
	app_ex_info* aei = new app_ex_info();
	aei->ex_dir = new std::string();
	aei->ex_name = new std::string();
	aei->execute_path = execute_path;
	//std::string ex_dir;
	//std::string ex_name;
	request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
	//ex_dir.append("\\");
	if (aei->ex_dir->find_last_of("\\") == std::string::npos) {
		aei->ex_dir->append("\\");
	}
	web_jsx::app_core::prepare_response(content_type, path_translated, *aei, method, get_env_c("PATH_INFO"));
	_free_app_info(aei);
	return EXIT_SUCCESS;
}
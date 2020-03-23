/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//#	include "core/web_jsx_global.h"
#if defined(FAST_CGI_APP)
#	include "web_jsx_fcgi.h"
#	define SOCKET_ARG	"fcgi"
#else
#	include "web_jsx_cgi.h"
#endif//FAST_CGI_APP
#	include <iostream>
#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE
#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj

int handle_request(const char* path, int is_spath, const char* web_app_root, int is_interactive);
#	define _handle_request handle_request

#if !defined(_console_request)
#	define _console_request sow_web_jsx::app_core::prepare_console_response
#endif//!_console_request
int _tmain() {
	std::cout << "I'm here....";
}
int main(int argc, char* argv[], char* envp[]) {
	try {
		if (is_user_interactive() == TRUE) {
			std::ios::sync_with_stdio(false);
			if (argc > 1) {
#if defined(FAST_CGI_APP)
				const char* f_parm = const_cast<const char*>(argv[1]);
				if (strcmp(f_parm, SOCKET_ARG) == 0) {
					return _handle_request(const_cast<const char*>(argv[2]), TRUE, const_cast<const char*>(argv[3]), TRUE);
				}
#endif//!FAST_CGI_APP
				_console_request(argc, argv, FALSE);
				return EXIT_SUCCESS;
			}
			print_info();
			return EXIT_SUCCESS;
		}
		if (argc > 1) {
			const char* f_parm = const_cast<const char*>(argv[1]);
			if (strcmp(f_parm, "internal_request") == 0) {
#if defined(FAST_CGI_APP)
				f_parm = const_cast<const char*>(argv[2]);
				if (strcmp(f_parm, SOCKET_ARG) == 0) {
					return _handle_request(const_cast<const char*>(argv[3]), TRUE, const_cast<const char*>(argv[4]), FALSE);
				}
#endif//!FAST_CGI_APP
				_console_request(argc, argv, TRUE);
				return EXIT_SUCCESS;
			}
#if defined(FAST_CGI_APP)
			if (strcmp(f_parm, SOCKET_ARG) == 0) {
				return _handle_request(const_cast<const char*>(argv[2]), TRUE, const_cast<const char*>(argv[3]), FALSE);
			}
#endif//!FAST_CGI_APP
		}
		return _handle_request(NULL, FALSE, NULL, FALSE);
	}
	catch (...) {
		exit(0);
		return EXIT_SUCCESS;
	}
}

int handle_request(
	const char* path, int is_spath, 
	const char* web_app_root, int is_interactive
) {
	int ret = EXIT_FAILURE;
	if (is_spath == TRUE) {
		if (path == NULL || strlen(path) == 0) {
			FATAL("Host address should not left blank!!!\n");
		}
	}
	std::string* exec_path = new std::string();
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(*exec_path) < 0) {
		_free_obj(exec_path);
		FATAL("Please add web_jsx bin path into environment variable Path!!!\n");
	}
#else
	const char* exec_path_c = get_env_c("web_jsx");
	if (strlen(exec_path_c) == 0) {
		_free_obj(exec_path);
		FATAL("Please add web_jsx bin path into environment variable Path!!!\n");
	}
	exec_path->append(exec_path_c);
#endif//!__WEB_JSX_PUBLISH
#if defined(__WEB_JSX_PUBLISH)
	else {
#endif//!__WEB_JSX_PUBLISH
		if (exec_path->find_last_of("\\") == std::string::npos) {
			exec_path->append("\\");
		}
		exec_path->append("web_jsx.exe");
#if defined(FAST_CGI_APP)
		ret = sow_web_jsx::fcgi_request::request_handler(exec_path->c_str(), path, is_spath, web_app_root, is_interactive);
#else
		sow_web_jsx::cgi_request::request_handler(exec_path->c_str());
		ret = EXIT_SUCCESS;
#endif
#if defined(__WEB_JSX_PUBLISH)
	}
#endif//!__WEB_JSX_PUBLISH
	_free_obj(exec_path);
	return ret;
}
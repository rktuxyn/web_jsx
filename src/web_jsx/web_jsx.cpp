/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(FAST_CGI_APP)
#	include "web_jsx_fcgi.h"
#endif//FAST_CGI_APP
#	include "web_jsx_cgi.h"

int handle_request(const char* path, int is_spath);

#if defined(FAST_CGI_APP)
#	define _handle_request handle_request
#else
#	define _handle_request web_jsx::cgi_request::request_handler
#endif//!FAST_CGI_APP

#if !defined(_console_request)
#	define _console_request web_jsx::app_core::prepare_console_response
#endif//!_console_request

int main(int argc, char* argv[], char* envp[]) {
	std::ios::sync_with_stdio(false);
	if (is_user_interactive() == TRUE) {
		if (argc > 1) {
			const char* f_parm = const_cast<const char*>(argv[1]);
			if (strcmp(f_parm, "fcgi") == 0) {
				return _handle_request(const_cast<const char*>(argv[2]), TRUE);
			}
			_console_request(argc, argv, FALSE);
			return EXIT_SUCCESS;
		}
		print_info();
		return EXIT_SUCCESS;
	}
	if (argc > 1) {
		const char* f_parm = const_cast<const char*>(argv[1]);
		if (strcmp(f_parm, "internal_request") == 0) {
			f_parm = const_cast<const char*>(argv[2]);
			if (strcmp(f_parm, "fcgi") == 0) {
				return _handle_request(const_cast<const char*>(argv[3]), TRUE);
			}
			_console_request(argc, argv, TRUE);
			return EXIT_SUCCESS;
		}
		if (strcmp(f_parm, "fcgi") == 0) {
			return _handle_request(const_cast<const char*>(argv[2]), TRUE);
		}
	}
	return _handle_request(NULL, FALSE);
}
int handle_request(const char* path, int is_spath) {
	int ret = EXIT_FAILURE;
	if (is_spath == TRUE) {
		if (path == NULL || strlen(path) == 0) {
			std::cout << "Host address should not left blank!!!\r\n";
			fflush(stdout);
			return ret;
		}
	}
	std::string* exec_path = new std::string();
#if defined(__WEB_JSX_PUBLISH)
	if (get_env_path(*exec_path) < 0) {
		std::cout << "Please add web_jsx bin path into environment variable Path!!!\r\n";
		fflush(stdout);
	}

#else
	const char* exec_path_c = get_env_c("web_jsx");
	if (strlen(exec_path_c) == 0) {
		_free_obj(exec_dir);
		std::cout << "Please add web_jsx bin path into environment variable Path!!!\r\n";
		fflush(stdout);
		return ret;
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
		ret = web_jsx::fcgi_request::request_handler(exec_path->c_str(), path, is_spath);
#if defined(__WEB_JSX_PUBLISH)
	}
#endif//!__WEB_JSX_PUBLISH
	_free_obj(exec_path);
	return ret;
}
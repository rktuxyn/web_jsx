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

int pass_request(const char* path);

#if defined(FAST_CGI_APP)
#	define _handle_request pass_request
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
				return _handle_request(const_cast<const char*>(argv[2]));
				//return _handle_request(const_cast<const char*>(argv[0]), const_cast<const char*>(argv[2]));
			}
			_console_request(argc, argv, false);
			return EXIT_SUCCESS;
		}
		print_info();
		return EXIT_SUCCESS;
	}
	if (argc > 1) {
		const char* f_parm = const_cast<const char*>(argv[1]);
		if (strcmp(f_parm, "I_REQ") == 0) {
			f_parm = const_cast<const char*>(argv[2]);
			if (strcmp(f_parm, "fcgi") == 0) {
				return _handle_request(const_cast<const char*>(argv[3]));
			}
			_console_request(argc, argv, true);
			return EXIT_SUCCESS;
		}
		if (strcmp(f_parm, "fcgi") == 0) {
			return _handle_request(const_cast<const char*>(argv[2]));
		}
	}
	return _handle_request(NULL);
	//return _handle_request(const_cast<const char*>(argv[0]), NULL);
}
int pass_request(const char* path) {
	int ret = EXIT_FAILURE;
	std::string* exec_path = new std::string();
	if (get_env_path(*exec_path) < 0) {
		std::cout << "Please add web_jsx bin path into environment variable Path!!!\r\n";
		fflush(stdout);
	}
	else {
		ret = web_jsx::fcgi_request::request_handler(exec_path->c_str(), path);
	}
	exec_path->clear(); delete exec_path;
	return ret;
}
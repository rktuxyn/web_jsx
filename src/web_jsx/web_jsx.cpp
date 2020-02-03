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

#if defined(FAST_CGI_APP)
#	define handle_request web_jsx_cgi::fcgi_request::request_handler
#else
#	define handle_request web_jsx_cgi::cgi_request::request_handler
#endif//!FAST_CGI_APP

int main(int argc, char *argv[], char*envp[]) {
	std::ios::sync_with_stdio(false);
	if (is_user_interactive() == TRUE) {
		if (argc > 1) {
			try {
				web_jsx_cgi::app_core::prepare_console_response(argc, argv, false);
			} catch (std::exception&e) {
				std::cout << "\r\n";
				std::cout << e.what() << "\r\n";
			}
			return EXIT_SUCCESS;
		}
		print_info();
		return EXIT_SUCCESS;
	}
	if (argc > 1) {
		const char*interactive_req = const_cast<const char*>(argv[1]);
		if (strcmp(interactive_req, "I_REQ") == 0) {
			web_jsx_cgi::app_core::prepare_console_response(argc, argv, true);
			return EXIT_SUCCESS;
		}
	}
	return handle_request(const_cast<const char*>(argv[0]));
}
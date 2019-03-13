/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:09 AM 11/18/2018 START
//4:11 AM 11/18/2018 END
#include "web_jsx_cgi.h"
#if !defined(FAST_CGI_APP)
int request_handler(const char*execute_path) {
	try {
		const char* CONTENT_TYPE = get_content_type();
		const char*PATH_TRANSLATED = get_env_c("PATH_TRANSLATED");
		if (__file_exists(PATH_TRANSLATED) == false) {
			not_found_response(CONTENT_TYPE);
			fflush(stdout);
			return EXIT_FAILURE;
		};
		req_method method = determine_req_method();
		if (method == req_method::UNSUPPORTED) {
			write_header(CONTENT_TYPE);
			std::cout << "\r\n";
			std::cout << "This method " << get_env("REQUEST_METHOD") << "not supported!!!" << "\r\n";
			fflush(stdout);
			return EXIT_FAILURE;
		}
		prepare_response(CONTENT_TYPE, PATH_TRANSLATED, execute_path, method, get_env_c("PATH_INFO"));
		return EXIT_SUCCESS;
	} catch (std::exception&e) {
		write_header(get_content_type());
		std::cout << "\r\n";
		std::cout << e.what() << "\r\n";
		fflush(stdout);
		return EXIT_FAILURE;
	}
};
int main(int argc, char *argv[], char*envp[]) {
	std::ios::sync_with_stdio(false);
	if (is_user_interactive() == TRUE) {
		if (argc > 1) {
			try {
				prepare_console_response(argc, argv, false);
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
			prepare_console_response(argc, argv, true);
			return EXIT_SUCCESS;
		}
	}
	return request_handler(const_cast<const char*>(argv[0]));
};
#endif//!FAST_CGI_APP
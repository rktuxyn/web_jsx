/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:46 PM 2/3/2019
#include "web_jsx_fcgi.h"
#if defined(FAST_CGI_APP)
int request_process(const char*execute_path, char **envp) {
	const char* CONTENT_TYPE = "text/html";
	try {
		const char*PATH_TRANSLATED = freq_env_c("PATH_TRANSLATED", envp);
		if (__file_exists(PATH_TRANSLATED) == false) {
			not_found_response(CONTENT_TYPE, envp);
			return EXIT_FAILURE;
		};
		const char* reqm = freq_env_c("REQUEST_METHOD", envp);
		req_method method = determine_req_method(reqm);
		if (method == req_method::UNSUPPORTED) {
			write_header(CONTENT_TYPE);
			std::cout << "\r\n";
			std::cout << "This method " << reqm << "not supported!!!" << "\r\n";
			return EXIT_FAILURE;
		}
		const char* PATH_INFO = freq_env_c("PATH_INFO", envp);

		prepare_response(
			CONTENT_TYPE,
			PATH_TRANSLATED, execute_path,
			method, PATH_INFO,
			envp
		);
		return EXIT_SUCCESS;
	} catch (std::exception&e) {
		write_header(CONTENT_TYPE);
		std::cout << "\r\n";
		std::cout << e.what() << "\r\n";
		return EXIT_FAILURE;
	} catch (...) {
		std::cout << "Content-type: text/html\r\n\r\n" << "Unknown error!";
		return EXIT_FAILURE;
	}
};
int request_handler(const char*execute_path) {
	sow_web_jsx::js_compiler::create_engine(execute_path);
	streambuf * cin_streambuf = cin.rdbuf();
	streambuf * cout_streambuf = cout.rdbuf();
	streambuf * cerr_streambuf = cerr.rdbuf();
	FCGX_Request request;
	FCGX_Init();
	FCGX_InitRequest(&request, 10, FCGI_FAIL_ACCEPT_ON_INTR);
	int count = 0;
	while (FCGX_Accept_r(&request) == 0) {
		// Note that the default bufsize (0) will cause the use of iostream
		// methods that require positioning (such as peek(), seek(),
		// unget() and putback()) to fail (in favour of more efficient IO).
		fcgi_streambuf cin_fcgi_streambuf(request.in);
		fcgi_streambuf cout_fcgi_streambuf(request.out);
		fcgi_streambuf cerr_fcgi_streambuf(request.err);
#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
		cin = &cin_fcgi_streambuf;
		cout = &cout_fcgi_streambuf;
		cerr = &cerr_fcgi_streambuf;
#else
		cin.rdbuf(&cin_fcgi_streambuf);
		cout.rdbuf(&cout_fcgi_streambuf);
		cerr.rdbuf(&cerr_fcgi_streambuf);
#endif
		count++;
		request_process(execute_path, request.envp);
		FCGX_Finish_r(&request);
		if (count > 10)break;
	};
	sow_web_jsx::js_compiler::dispose_engine();
	return EXIT_SUCCESS;
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
	const char*interactive_req = const_cast<const char*>(argv[1]);
	if (strcmp(interactive_req, "I_REQ") == 0) {
		prepare_console_response(argc, argv, true);
		return EXIT_SUCCESS;
	}
	//12:28 AM 1/28/2019
	return request_handler(const_cast<const char*>(argv[0]));
};
#endif//FAST_CGI_APP
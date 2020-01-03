/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:46 PM 2/3/2019
#include "web_jsx_fcgi.h"
#if defined(FAST_CGI_APP)
int web_jsx_cgi::fcgi_request::request_process(const app_ex_info aei, const char*env_path, char **envp) {
	const char* content_type = "text/html";// FCGX_GetParam("CONTENT_TYPE", envp);
	try {
		const char*path_translated = web_jsx_cgi::fcgi_request::freq_env_c("PATH_TRANSLATED", envp);
		if (__file_exists(path_translated) == false) {
			web_jsx_cgi::fcgi_request::not_found_response(content_type, envp, aei.ex_dir->c_str());
			fflush(stdout);
			return EXIT_FAILURE;
		};
		const char* reqm = web_jsx_cgi::fcgi_request::freq_env_c("REQUEST_METHOD", envp);
		req_method method = determine_req_method(reqm);
		if (method == req_method::UNSUPPORTED) {
			write_header(content_type);
			std::cout << "\r\n";
			std::cout << "This method " << reqm << "not supported!!!" << "\r\n";
			fflush(stdout);
			return EXIT_FAILURE;
		}
		const char* path_info = web_jsx_cgi::fcgi_request::freq_env_c("PATH_INFO", envp);
		web_jsx_cgi::app_core::prepare_response(
			content_type,
			path_translated, aei,
			method, path_info,
			env_path,
			envp
		);
		return EXIT_SUCCESS;
	} catch (std::exception&e) {
		write_internal_server_error("text/html", aei.ex_dir->c_str(), 500, e.what());
		/*write_header(content_type);
		std::cout << "\r\n";
		std::cout << e.what() << "\r\n";*/
		return EXIT_FAILURE;
	} catch (...) {
		std::cout << "Content-type: text/html\r\n\r\n" << "Unknown error!";
		return EXIT_FAILURE;
	}
}
// Debug https://blogs.msdn.microsoft.com/friis/2012/09/24/easy-debugging-of-cgi-fastcgi-application/
int web_jsx_cgi::fcgi_request::request_handler(const char*execute_path) {
	streambuf * cin_streambuf = cin.rdbuf();
	streambuf * cout_streambuf = cout.rdbuf();
	streambuf * cerr_streambuf = cerr.rdbuf();
	FCGX_Request request;
	{
		int err = -1, count = 0, max = 10;
		while (err != 0) {
			err = FCGX_Init_x(&request, FCGI_LISTENSOCK_FILENO, FCGI_FAIL_ACCEPT_ON_INTR);
			count++;
			if (count >= max)break;
		}
		if (err != 0) {
			app_ex_info* aei = new app_ex_info();
			aei->ex_dir = new std::string();
			aei->ex_name = new std::string();
			aei->execute_path = execute_path;
			request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
			write_internal_server_error("text/html", aei->ex_dir->c_str(), 500, "Unable to initialize FastCGI module!!!");
			web_jsx_cgi::app_core::free_app_info(aei);
			//write_header("text/html");
			////write_internal_server_error("text/html",)
			//std::cout << "Status: 500 Internal Server Error" << H_N_L;
			//std::cout << "\r\n\r\n";
			//std::cout << "Unable to initialize FastCGI module!!!";
			fflush(stdout);
			return EXIT_SUCCESS;
		}
	}
	
	if (FCGX_IsCGI() == TRUE) {
		web_jsx_cgi::cgi_request::request_handler(execute_path);
		sow_web_jsx::free_resource();
		return EXIT_SUCCESS;
	}
	app_ex_info *aei = new app_ex_info();
	aei->ex_dir = new std::string();
	aei->ex_name = new std::string();
	aei->execute_path = execute_path;
	request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
	aei->ex_dir->append("\\");
	sow_web_jsx::js_compiler::create_engine(aei->ex_dir->c_str());
	//count = 0;
	const char* env_path = get_env_c("path");
	while (FCGX_Accept_r(&request) == 0) {
		// Note that the default bufsize (0) will cause the use of iostream
		// methods that require positioning (such as peek(), seek(),
		// unget() and putback()) to fail (in favour of more efficient IO).
		fcgi_streambuf cin_fcgi_streambuf(request.in);
		fcgi_streambuf cout_fcgi_streambuf(request.out);
		fcgi_streambuf cerr_fcgi_streambuf(request.err);
#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
		std::cin = &cin_fcgi_streambuf;
		std::cout = &cout_fcgi_streambuf;
		std::cerr = &cerr_fcgi_streambuf;
#else
		std::cin.rdbuf(&cin_fcgi_streambuf);
		std::cout.rdbuf(&cout_fcgi_streambuf);
		std::cerr.rdbuf(&cerr_fcgi_streambuf);
#endif
		//std::cout.rdbuf()->pubsetbuf(0, 0);
		try {
			//count++;
			web_jsx_cgi::fcgi_request::request_process(*aei, env_path, request.envp);
			if (request.out->isClosed) {
				sow_web_jsx::wrapper::clear_cache();
			}
		}
		catch (...) {
			sow_web_jsx::wrapper::clear_cache();
			fflush(stdout);
		}
		//FCGX_Finish_r(&request);
		//if (count > 500)break;
	}
	FCGX_Free(&request, 1);
	sow_web_jsx::js_compiler::dispose_engine();
	// restore stdio streambufs
	cin.rdbuf(cin_streambuf);
	cout.rdbuf(cout_streambuf);
	cerr.rdbuf(cerr_streambuf);
	web_jsx_cgi::app_core::free_app_info(aei);
	sow_web_jsx::free_resource();
	return EXIT_SUCCESS;
}
#endif//FAST_CGI_APP
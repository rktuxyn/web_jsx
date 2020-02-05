/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:46 PM 2/3/2019
#	include "web_jsx_fcgi.h"
#if defined(FAST_CGI_APP)
#	include "core/glb_r.h"
#	include <fastcgi.h>
#	include <fcgi_stdio.h>
#	include <fcgio.h>
#	include "web_jsx_cgi.h"
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
using namespace std;
using namespace sow_web_jsx::js_compiler;
int web_jsx::fcgi_request::request_process(
	const app_ex_info aei, 
	const char*env_path, char **envp
) {
	const char* content_type = "text/html";// FCGX_GetParam("CONTENT_TYPE", envp);
	try {
		//if (aei.is_fserver == TRUE) {
		//	//REQUEST_URI
		//}
		const char*path_translated = web_jsx::fcgi_request::freq_env_c("PATH_TRANSLATED", envp);
		if (__file_exists(path_translated) == false) {
			web_jsx::fcgi_request::not_found_response(
				content_type, 
				envp, aei.ex_dir->c_str()
			);
			fflush(stdout);
			return EXIT_FAILURE;
		}
		const char* reqm = web_jsx::fcgi_request::freq_env_c("REQUEST_METHOD", envp);
		req_method method = determine_req_method(reqm);
		if (method == req_method::UNSUPPORTED) {
			write_header(content_type);
			std::cout << "\r\n";
			std::cout << "This method " << reqm << "not supported!!!" << "\r\n";
			fflush(stdout);
			return EXIT_FAILURE;
		}
		const char* path_info = web_jsx::fcgi_request::freq_env_c("PATH_INFO", envp);
		web_jsx::app_core::prepare_response(
			content_type,
			path_translated, aei,
			method, path_info,
			env_path,
			envp
		);
		return EXIT_SUCCESS;
	} catch (std::exception&e) {
		write_internal_server_error("text/html", aei.ex_dir->c_str(), 500, e.what());
		return EXIT_FAILURE;
	} catch (...) {
		std::cout << "Content-type: text/html\r\n\r\n" << "Unknown error!";
		return EXIT_FAILURE;
	}
}
//void print_fcgi_envp(char* envp[]) {
//	std::cout << "Content-Type:text/html" << H_N_L;
//	std::cout << "Accept-Ranges:bytes" << H_N_L;
//	std::cout << "X-Powered-By:safeonline.world" << H_N_L;
//	std::cout << "X-Process-By:web_jsx" << H_N_L;
//	std::cout << "Status: 404 Not found" << H_N_L;
//	std::cout << "WebJsx-Error-Code:404" << H_N_L;
//	std::cout << "\r\n";
//	print_envp(envp);
//}
// Debug https://blogs.msdn.microsoft.com/friis/2012/09/24/easy-debugging-of-cgi-fastcgi-application/
int web_jsx::fcgi_request::request_handler(const char*execute_path, const char* path) {
	streambuf * cin_streambuf = cin.rdbuf();
	streambuf * cout_streambuf = cout.rdbuf();
	streambuf * cerr_streambuf = cerr.rdbuf();
	int is_self_req = path != NULL && strlen(path) != 0;
	if (is_self_req == TRUE) {
		std::cout << "Execute dir:" << execute_path << std::endl;
	}
	FCGX_Request request;
	{
		int err = -1, count = 0, max = 10;
		while (err != 0) {
			err = FCGX_Init_x(&request, FCGI_LISTENSOCK_FILENO, FCGI_FAIL_ACCEPT_ON_INTR);
			count++;
			if (count >= max)break;
		}
		if (err != 0) {
			if (is_self_req == FALSE) {
				app_ex_info* aei = new app_ex_info();
				aei->ex_dir = new std::string();
				aei->ex_name = new std::string();
				aei->execute_path = execute_path;
				aei->ex_dir->append("\\");
				::request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
				::write_internal_server_error("text/html", aei->ex_dir->c_str(), 500, "Unable to initialize FastCGI module!!!");
				web_jsx::app_core::free_app_info(aei);
			}
			else {
				fprintf(stderr, "%", "Unable to initialize FastCGI module!!!");
			}
			fflush(stdout);
			return EXIT_FAILURE;
		}
	}
	if (is_self_req == TRUE) {
		if (FCGX_OpenSocket(path, 1000) < 0) {
			fprintf(stderr, "%=>%", "Unable to open socket to requested", path);
			return EXIT_FAILURE;
		}
	}
	else {
		if (FCGX_IsCGI() == TRUE) {
			web_jsx::cgi_request::request_handler(execute_path);
			sow_web_jsx::free_resource();
			return EXIT_SUCCESS;
		}
	}
	
	app_ex_info *aei = new app_ex_info();
	aei->ex_dir = new std::string();
	aei->ex_name = new std::string();
	aei->execute_path = execute_path;
	aei->is_fserver = is_self_req;
	request_file_info(aei->execute_path, *aei->ex_dir, *aei->ex_name);
	aei->ex_dir->append("\\");
	sow_web_jsx::js_compiler::create_engine(aei->ex_dir->c_str());
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
		//print_fcgi_envp(request.envp);
		web_jsx::fcgi_request::request_process(*aei, env_path, request.envp);
		if (request.out->isClosed) {
			sow_web_jsx::wrapper::clear_cache();
		}
	}
	FCGX_Free(&request, 1);
	sow_web_jsx::js_compiler::dispose_engine();
	// restore stdio streambufs
	cin.rdbuf(cin_streambuf);
	cout.rdbuf(cout_streambuf);
	cerr.rdbuf(cerr_streambuf);
	web_jsx::app_core::free_app_info(aei);
	sow_web_jsx::free_resource();
	return EXIT_SUCCESS;
}
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#endif//FAST_CGI_APP
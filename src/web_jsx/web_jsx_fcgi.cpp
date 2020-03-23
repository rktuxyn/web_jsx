/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:46 PM 2/3/2019
#if !defined(WEB_JSX_MULTI_THREAD)
#	include "core/web_jsx_global.h"
#	include "web_jsx_fcgi.h"
#	include "fcreqh.h"
#	include "core/v8_engine.h"
#	include "core/glb_r.h"
#	include <fastcgi.h>
#	include <fcgi_stdio.h>
#	include <fcgio.h>
#	include "web_jsx_cgi.h"
#	include "core/native_wrapper.h"
#	include "core/wjsx_env.h"
#	include "core/module_store.h"
#	include <thread>
using namespace sow_web_jsx;
typedef struct {
	const char* env_path;
	int is_fserver;
	int fcgi_flag;
	int thread_id;
	int is_concurrent;
}listener__context;
static int _socket_id = FCGI_LISTENSOCK_FILENO;
int request_process(
	v8::Isolate* isolate,
	const app_ex_info aei,
	const char*env_path, char **envp,
	wjsx_env* wj_env
) {
	const char* content_type = "text/html";// FCGX_GetParam("CONTENT_TYPE", envp);
	_NEW_STR(path_translated);
	::fcgi_request::freq_env_c("PATH_TRANSLATED", envp, *path_translated);
	if (path_translated->empty() || __file_exists(path_translated->c_str()) == false) {
		::fcgi_request::not_found_response(
			content_type,
			envp, aei.ex_dir->c_str(),
			wj_env
		);
		_WFLUSH(); _free_obj(path_translated);
		return EXIT_FAILURE;
	}
	req_method method = UNSUPPORTED;
	_NEW_STR(reqm);
	if (::fcgi_request::freq_env_c("REQUEST_METHOD", envp, *reqm) == TRUE) {
		method = ::determine_req_method(reqm->c_str());
	}
	_free_obj(reqm);
	if (method == req_method::UNSUPPORTED) {
		_free_obj(path_translated);
		write_header(content_type, wj_env);
		_WCOUT << "\r\n";
		_WCOUT << "This method " << reqm << "not supported!!!" << "\r\n";
		_WFLUSH();
		return EXIT_FAILURE;
	}
	_NEW_STR(path_info);
	::fcgi_request::freq_env_c("PATH_INFO", envp, *path_info);
	::app_core::prepare_response(
		/*v8::Isolate* isolate*/isolate,
		/*const char* content_type*/content_type,
		/*const char*path_translated*/path_translated->c_str(),
		/*const app_ex_info aei*/aei,
		/*const req_method method*/method,
		/*const char* path_info*/path_info->c_str(),
		/*const char*env_path*/env_path,
		/*char **envp*/envp,
		/*wjsx_env* wj_env*/wj_env
	);
	_free_obj(path_translated); _free_obj(path_info);
	return EXIT_SUCCESS;
}
void print_fcgi_envp(wjsx_env&wj_env, char* envp[]) {
	wj_env << "Content-Type:text/html" << H_N_L;
	wj_env << "Accept-Ranges:bytes" << H_N_L;
	wj_env << "X-Powered-By:safeonline.world" << H_N_L;
	wj_env << "X-Process-By:web_jsx" << H_N_L;
	wj_env << "Status: 200 Ok" << H_N_L;
	wj_env << "\r\n";
	print_envp(wj_env, envp);
}

int listen_request(listener__context* ctx) {
	_NEW_STR(exec_path);
	if (get_env_path(*exec_path) < 0) {
		FATAL("Please add web_jsx bin path into environment variable Path!!!\n");
	}
	exec_path->append("web_jsx.exe");
	FCGX_Request request;
	if (FCGX_InitRequest(&request, _socket_id, ctx->fcgi_flag) != 0) {
		_free_obj(exec_path);
		return EXIT_FAILURE;
	}
	app_ex_info* app_info = new app_ex_info;
	ASSERT(app_info != NULL);
	app_info->ex_dir = new std::string();
	app_info->ex_name = new std::string();
	app_info->execute_path = exec_path->c_str();
	app_info->is_fserver = ctx->is_fserver;
	::request_file_info(app_info->execute_path, *app_info->ex_dir, *app_info->ex_name);
	app_info->ex_dir->append("\\");
	v8::Isolate* isolate = ::v8_engine::create_isolate();
	int compiled_cached = is_compiled_cached();
	int check_file_state = is_check_file_state();
	native_data_structure* native_data = NULL;
	for (;;) {
#if defined(_IS_DEBUG)
		if (ctx->is_fserver == TRUE) {
			handle_request++;
		}
#endif//!_IS_DEBUG
		int code = FCGX_Accept_r(&request);
		if (code < 0)continue;
		// Note that the default bufsize (0) will cause the use of iostream
		// methods that require positioning (such as peek(), seek(),
		// unget() and putback()) to fail (in favour of more efficient IO).
		fcgi_streambuf cin_fcgi_streambuf(request.in);
		fcgi_streambuf cout_fcgi_streambuf(request.out);
		fcgi_streambuf cerr_fcgi_streambuf(request.err);
		std::istream* in_stream = new std::istream(&cin_fcgi_streambuf, true);//, 0x01 | 0x20
		std::ostream* out_stream = new std::ostream(&cout_fcgi_streambuf, true);//, 0x02 | 0x20
		std::ostream* err_stream = new std::ostream(&cerr_fcgi_streambuf, true);//, 0x02 | 0x20
		wjsx_env* wj_env = new wjsx_env(in_stream, out_stream, err_stream);
		wj_env->compiled_cached = compiled_cached;
		wj_env->check_file_state = check_file_state;
		if (native_data != NULL) {
			wj_env->set_native_data_structure(*native_data);
		}
		try {
			::request_process(
					/*v8::Isolate* isolate*/isolate,
					/*const app_ex_info aei*/*app_info,
					/*const char*env_path*/ctx->env_path,
					/*char **envp*/request.envp,
					/*wjsx_env* wj_env*/wj_env
			);
		}
		catch (...) {
			_NEW_STRA(err, "Internal server error occured...");
#if defined(_IS_DEBUG)
			err->append("in "); err->append(__FILE__); err->append(" on line ");
			err->append(std::to_string(__LINE__)); err->append(".");
#endif//!_IS_DEBUG
			::write_internal_server_error(
				/*content_type*/"text/html",
				/*execute_path*/app_info->ex_dir->c_str(),
				/*error_code*/500,
				/*error_msg*/err->c_str(),
				/*wjsx_env*/wj_env
			);
			_free_obj(err);
		}
		//print_fcgi_envp(*wj_env, request.envp);
		if (native_data == NULL && wj_env->has_native_data_structure() == TRUE) {
			native_data = wj_env->get_native_data_structure(FALSE);
		}
		if (not request.out->isClosed) {
			FCGX_Finish_r(&request);	
		}
		::wrapper::clear_cache(*wj_env);
		_free_obj(wj_env);
	}
	swjsx_module::clean_native_module(native_data);
	delete native_data; native_data = NULL;
	_dispose_isolate(isolate);
	FCGX_Free(&request, TRUE);
	_free_app_info(app_info);
	_free_obj(exec_path);
	return EXIT_SUCCESS;
}
int get_thread_id() {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	uint64_t id = std::stoull(ss.str());
	return (int)id;
}
void start_engine(const char* execute_path) {
	_NEW_STR(ex_dir);
	::get_dir_path(execute_path, *ex_dir);
	ex_dir->append("\\");
	::v8_engine::create_engine(ex_dir->c_str());
	_free_obj(ex_dir);
	return;
}
int prepare_listener(const char* execute_path, int is_fserver) {
	start_engine(execute_path);
	listener__context* ctx = new listener__context;
	_NEW_STR(path_str);
	get_env_c("path", *path_str);
	ctx->env_path = path_str->c_str();
	ctx->fcgi_flag = FCGI_FAIL_ACCEPT_ON_INTR;
	ctx->thread_id = get_thread_id();
	ctx->is_fserver = is_fserver;
	ctx->is_concurrent = FALSE;
	int ret = listen_request(ctx);
	_free_obj(path_str);
	return ret;
}
// Debug https://blogs.msdn.microsoft.com/friis/2012/09/24/easy-debugging-of-cgi-fastcgi-application/
int fcgi_request::request_handler(
	const char* execute_path,
	const char* path, int is_spath,
	const char* web_app_root,
	int is_interactive
) {
#if defined(_IS_DEBUG)
	if ( is_spath == TRUE && is_interactive == TRUE )fprintf_s(_stdout, "Execute dir %s\n", execute_path);
#endif//!_IS_DEBUG
	{
		int err = -1, count = 0, max = 10;
		while (err != 0) {
			err = ::FCGX_Init();
			count++;
			if (count >= max)break;
		}
		if (err != 0) {
			fprintf_s(_stderr, "Unable to initialize FastCGI module!!!\n");
			return EXIT_FAILURE;
		}
	}
	if (is_spath == TRUE) {
		_socket_id = ::FCGX_OpenSocket(path, 3000);
		if (_socket_id < 0) {
			fprintf_s(_stderr, "Unable to open socket on %s\n", path);
			return EXIT_FAILURE;
		}
	}
	else {
		if (::FCGX_IsCGI() == TRUE) {
			::cgi_request::request_handler(execute_path);
			::free_resource();
			return EXIT_SUCCESS;
		}
	}
	return prepare_listener(execute_path, is_spath);
}
#endif//!WEB_JSX_MULTI_THREAD
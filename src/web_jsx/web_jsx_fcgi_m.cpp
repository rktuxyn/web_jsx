/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//3:46 PM 2/3/2019
#if defined(WEB_JSX_MULTI_THREAD)
#	include "web_jsx_fcgi.h"
#	include "core/web_jsx_global.h"
#	include <uv.h>
#	include "fcreqh.h"
#	include "core/v8_engine.h"
#	include "core/glb_r.h"
#	include "web_jsx_cgi.h"
#	include "core/native_wrapper.h"
#	include "core/wjsx_env.h"
/*[multiplex]*/
#	include "core/shared_queue.hpp"
#	include "core/module_store.h"
#	include "core/js_compiler.h"
/*[/multiplex]*/
#	include <fastcgi.h>
#	include <fcgi_stdio.h>
#	include <fcgio.h>
using namespace sow_web_jsx;
typedef struct {
	const char* env_path;
	int is_interactive;
	int is_fserver;
	const char* web_app_root;
	int fcgi_flag;
	int thread_id;
	int app_thread_id;
	int is_concurrent;
	int compiled_cached;
	int check_file_state;
	int socket_id;
	int total_processed;
	std::shared_ptr<std::mutex>	mutex;
}thread__context;
typedef struct {
	FCGX_Request* request;
	native_data_structure* native_data;
}handler__context;
int handle_request(
	v8::Isolate* isolate,
	const app_ex_info aei,
	const char* env_path, char** envp,
	wjsx_env* wj_env
);
void print_fcgi_envp(wjsx_env& wj_env, char* envp[]);
void after_queue_complete(wjsx_env* wj_env, handler__context* hctx);
void queue_forward_request(
	v8::Isolate* isolate,
	handler__context* hctx,
	app_ex_info* app_info,
	thread__context* ctx
);
native_data_structure* create_native_data_structure(
	v8::Isolate* isolate, const app_ex_info& app_info,
	thread__context* ctx
);
void start_engine(const char* execute_path);
int listen_blocking_request(thread__context* ctx);
thread__context* get_ctx(void* args);
void listen_nonblocking_request(void* args);
int handle_nonblocking_request(void* args);
int create_nonblocking_thread(
	const char* execute_path, int is_fserver, 
	const char* web_app_root,
	int is_interactive, int socket_id
);
/*[----------------]*/
int handle_request(
	v8::Isolate* isolate,
	const app_ex_info aei,
	const char* env_path, char** envp,
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
		_free_obj(path_translated);
		return EXIT_FAILURE;
	}
	req_method method = UNSUPPORTED;
	_NEW_STR(reqm);
	if (::fcgi_request::freq_env_c("REQUEST_METHOD", envp, *reqm) == TRUE) {
		method = ::determine_req_method(reqm->c_str());
	}
	if (method == req_method::UNSUPPORTED) {
		_free_obj(path_translated);
		write_header(content_type, wj_env);
		_WCOUT << "\r\n";
		_WCOUT << "This method " << reqm << "not supported!!!" << "\r\n";
		_WFLUSH(); _free_obj(reqm);
		return EXIT_FAILURE;
	}
	_free_obj(reqm);
	::_NEW_STR(path_info);
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
void print_fcgi_envp(wjsx_env& wj_env, char* envp[]) {
	std::ostream& cout = wj_env.cout();
	cout << "Content-Type:text/html" << H_N_L;
	cout << "Accept-Ranges:bytes" << H_N_L;
	cout << "X-Powered-By:safeonline.world" << H_N_L;
	cout << "X-Process-By:web_jsx" << H_N_L;
	cout << "Status: 404 Not found" << H_N_L;
	cout << "WebJsx-Error-Code:404" << H_N_L;
	cout << "\r\n";
	print_envp(wj_env, envp);
}
int listen_blocking_request(thread__context* ctx) {
	_NEW_STR(exec_path);
	if (get_env_path(*exec_path) < 0) {
		FATAL("Please add web_jsx bin path into environment variable Path!!!\n");
	}
	exec_path->append("web_jsx.exe");
	FCGX_Request request;
	if (FCGX_InitRequest(&request, ctx->socket_id, ctx->fcgi_flag) != 0) {
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
	ctx->total_processed = 0;
	native_data_structure* native_data = NULL;
	for (;;) {
		int code = FCGX_Accept_r(&request);
		if (code < 0)continue;
		// Note that the default bufsize (0) will cause the use of iostream
		// methods that require positioning (such as peek(), seek(),
		// unget() and putback()) to fail (in favour of more efficient IO).
		fcgi_streambuf cin_fcgi_streambuf(request.in);
		fcgi_streambuf cout_fcgi_streambuf(request.out);
		fcgi_streambuf cerr_fcgi_streambuf(request.err);
		wjsx_env* wj_env = new wjsx_env(
			/*in_stream*/new std::istream(&cin_fcgi_streambuf, true),
			/*out_stream*/new std::ostream(&cout_fcgi_streambuf, true),
			/*err_stream*/new std::ostream(&cerr_fcgi_streambuf, true),
			/*thread_req*/FALSE,
			/*thread_id*/ctx->thread_id
		);
		wj_env->compiled_cached = compiled_cached;
		wj_env->check_file_state = check_file_state;
		if (native_data != NULL) {
			wj_env->set_native_data_structure(*native_data);
		}
		wj_env->set_total_handled_req(ctx->total_processed);
		try {
			::handle_request(
				/*v8::Isolate* isolate*/isolate,
				/*const app_ex_info aei*/*app_info,
				/*const char*env_path*/ctx->env_path,
				/*char **envp*/request.envp,
				/*wjsx_env* wj_env*/wj_env
			);
			ctx->total_processed++;
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
/*[----------------]*/
std::shared_ptr<shared_queue<FCGX_Request*>> _thread_safe;
CRITICAL_SECTION _gw_accept_mutex;
void after_queue_complete(wjsx_env* wj_env, handler__context* hctx) {
	if (wj_env->is_runtime_error() == FALSE && wj_env->is_script_mood() == TRUE) {
		::wrapper::response_body_flush(*wj_env, false);
	}
	if (hctx->native_data == NULL && wj_env->has_native_data_structure() == TRUE) {
		hctx->native_data = wj_env->get_native_data_structure(FALSE);
	}
	if (not hctx->request->out->isClosed) {
		FCGX_Finish_r(hctx->request);
	}
	::wrapper::clear_cache(*wj_env);
	_free_obj(wj_env);
	delete hctx->request; hctx->request = NULL;
}
void queue_forward_request(
	v8::Isolate* isolate,
	handler__context* hctx,
	app_ex_info* app_info,
	thread__context* ctx
) {
	fcgi_streambuf cin_fcgi_streambuf(hctx->request->in);
	fcgi_streambuf cout_fcgi_streambuf(hctx->request->out);
	fcgi_streambuf cerr_fcgi_streambuf(hctx->request->err);
	wjsx_env* wj_env = new wjsx_env(
		/*_istream* in*/new std::istream(&cin_fcgi_streambuf, true), 
		/*_ostream* out*/new std::ostream(&cout_fcgi_streambuf, true), 
		/*_ostream* err*/new std::ostream(&cerr_fcgi_streambuf, true),
		/*int thread_req*/TRUE,
		/*int thread_id*/ctx->thread_id,
		/*int app_thread_id*/ctx->app_thread_id
	);
	wj_env->check_file_state = TRUE;//ctx->check_file_state;
	wj_env->compiled_cached = FALSE;// ctx->compiled_cached;
	if (hctx->native_data != NULL) {
		wj_env->set_native_data_structure(*hctx->native_data);
	}
	wj_env->set_total_handled_req(ctx->total_processed);
	try {
		::handle_request(
			/*v8::Isolate* isolate*/isolate,
			/*const app_ex_info aei*/*app_info,
			/*const char*env_path*/ctx->env_path,
			/*char **envp*/hctx->request->envp,
			/*wjsx_env* wj_env*/wj_env
		);
	}
	catch (...) {
		std::string* err = new std::string("Internal server error occured...");
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
	::after_queue_complete(wj_env, hctx);
}
native_data_structure* create_native_data_structure(
	v8::Isolate* isolate, const app_ex_info& app_info,
	thread__context* ctx
) {
	std::lock_guard<std::mutex> lock(*ctx->mutex);
	wjsx_env* wj_env = new wjsx_env(FALSE, TRUE);
	wj_env->set_app_dir(app_info.ex_dir->c_str());
	if (ctx->is_interactive == TRUE)
		fprintf_s(_stdout, "------------------------------\n");
	if (ctx->web_app_root == NULL || strlen(ctx->web_app_root) == 0) {
		if (ctx->is_interactive == TRUE)
			fprintf_s(_stdout, "Web Application root dir not found!!!\n");
		_NEW_STR(cwd);
		if (::get_current_working_dir(*cwd) == TRUE) {
			cwd->append("\\");
			if (ctx->is_interactive == TRUE)
				fprintf_s(_stdout, "We are trying to load `module.cfg`\nfrom current working directory %s\nIf not found in current working directory\nThen we will try to load from Application root directory\n%s\n", cwd->c_str(), app_info.ex_dir->c_str());
			wj_env->set_root_dir(cwd->c_str());
		}
		else {
			if (ctx->is_interactive == TRUE)
				fprintf_s(_stdout, "We are trying to load `module.cfg`\nfrom %s\n", app_info.ex_dir->c_str());
			wj_env->set_root_dir(app_info.ex_dir->c_str());
		}
		_free_obj(cwd);
	}
	else {
		if (ctx->is_interactive == TRUE)
			fprintf_s(_stdout, "We are trying to load `module.cfg`\nfrom web_app root directory %s\nIf not found in web_app root directory\nThen we will try to load from Application root directory\n%s\n", ctx->web_app_root, app_info.ex_dir->c_str());
		wj_env->set_root_dir(ctx->web_app_root);
	}
	if (ctx->is_interactive == TRUE)
		fprintf_s(_stdout, "------------------------------\n");
	::wrap_wjsx_env(isolate, wj_env);
	::js_compiler::compile_raw_script(isolate, "__implimant_native_module( ); delete this[\"__implimant_native_module\"];");
	::wrap_wjsx_env(isolate, NULL);
	native_data_structure* nds = wj_env->get_native_data_structure(FALSE);
	_free_obj(wj_env);
	if (nds == NULL) {
		FATAL("Unable to load Native Data Structure....");
	}
	return nds;
}
int handle_nonblocking_request(void* args) {
	_NEW_STR(exec_path);
	if (::get_env_path(*exec_path) < 0) {
		FATAL("Please add web_jsx bin path into environment variable Path!!!\n");
	}
	exec_path->append("web_jsx.exe");
	thread__context* ctx = get_ctx(args);
	ctx->is_concurrent = TRUE;
	_NEW_STR(env_path);
	::get_env_c("path", *env_path);
	ctx->env_path = env_path->c_str();
	if (ctx->is_interactive == TRUE)
		fprintf_s(_stdout, "handle_request thread#%d\n", ctx->thread_id);
	v8::Isolate* isolate = ::v8_engine::create_isolate(*ctx->mutex);
	app_ex_info* app_info = new app_ex_info;
	ASSERT(app_info != NULL);
	app_info->ex_dir = new std::string();
	app_info->ex_name = new std::string();
	app_info->execute_path = exec_path->c_str();
	app_info->is_fserver = ctx->is_fserver;
	::request_file_info(app_info->execute_path, *app_info->ex_dir, *app_info->ex_name);
	app_info->ex_dir->append("\\");
	handler__context* hctx = (handler__context*)malloc(sizeof(handler__context));
	ASSERT(hctx != NULL);
	hctx->native_data = ::create_native_data_structure(isolate, *app_info, ctx);
	int handled_req = 0; ctx->total_processed = 0;
	_thread_safe->try_deque([&](FCGX_Request *request) {
		if (request == NULL) return;
		if ((request->in == NULL || request->out == NULL) || (request->out->isClosed)) {
			FCGX_Finish_r(request); return;
		}
		hctx->request = request;
		::queue_forward_request(isolate, hctx, app_info, ctx);
		handled_req++; ctx->total_processed++;
		if (handled_req >= 50) {
			isolate->LowMemoryNotification(); handled_req = 0;
		}
	});
	_free_obj(env_path);
	::swjsx_module::clean_native_module(hctx->native_data);
	_free_obj(hctx->native_data); _dispose_isolate(isolate);
	_free_app_info(app_info); _free_obj(exec_path); free(hctx);
	return EXIT_SUCCESS;
}
thread__context* get_ctx(void* args) {
	thread__context* ctx = args == NULL ? new thread__context : reinterpret_cast<thread__context*>(args);
	ctx->fcgi_flag = FCGI_FAIL_ACCEPT_ON_INTR;
	ctx->thread_id = ::get_thread_id();
	ctx->compiled_cached = ::is_compiled_cached();
	ctx->check_file_state = ::is_check_file_state();
	return ctx;
}
void start_engine(const char* execute_path) {
	_NEW_STR(ex_dir);
	::get_dir_path(execute_path, *ex_dir);
	ex_dir->append("\\");
	::v8_engine::create_engine(ex_dir->c_str());
	_free_obj(ex_dir);
	return;
}
void listen_nonblocking_request(void* args) {
	thread__context* ctx = (thread__context*)args;
	if (ctx->is_interactive == TRUE)
		fprintf_s(_stdout, "listen_nonblocking_request thread#%d\n", ::get_thread_id());
	for (;;) {
		FCGX_Request* request = new FCGX_Request;
		int ret = ::FCGX_InitRequest(request, ctx->socket_id, FCGI_FAIL_ACCEPT_ON_INTR);
		if (ret != 0) FATAL("FCGX_InitRequest failed!!!\n");
		::EnterCriticalSection(&_gw_accept_mutex);
		ret = FCGX_Accept_r(request);
		::LeaveCriticalSection(&_gw_accept_mutex);
		if (ret < 0)continue;
		_thread_safe->try_queue(std::move(request));
	}
	return;
}

#define MAX_CONCURRENCY 16

int create_nonblocking_thread(
	const char* execute_path, int is_fserver,
	const char* web_app_root, int is_interactive,
	int socket_id
) {
	::start_engine(execute_path);
	if (is_fserver == FALSE ) {
		thread__context* ctx = get_ctx(NULL);
		ctx->is_fserver = is_fserver;
		ctx->is_concurrent = FALSE;
		ctx->socket_id = socket_id;
		ctx->app_thread_id = ::get_thread_id(); //App thread id
		ctx->thread_id = ctx->app_thread_id;
		_NEW_STR(env_path);
		::get_env_c("path", *env_path);
		ctx->env_path = env_path->c_str();
		int ret = ::listen_blocking_request(ctx);
		_free_obj(env_path);
		return ret;
	}
	static unsigned int concurrency = std::thread::hardware_concurrency();
	if (concurrency > MAX_CONCURRENCY)concurrency = MAX_CONCURRENCY;
	ASSERT(concurrency <= _MAX_ISOLATE);
	_thread_safe = std::shared_ptr<shared_queue<FCGX_Request*>>(new shared_queue<FCGX_Request*>(1, concurrency));
	char* cores_string = new char[10];
	sprintf(cores_string, "%d", concurrency);
#ifdef _WIN32
	wchar_t* lpName = ::ccr2ws("UV_THREADPOOL_SIZE");
	wchar_t* lpValue = ::ccr2ws(cores_string);
	SetEnvironmentVariable(const_cast<LPCWSTR>(lpName), const_cast<LPCWSTR>(lpValue));
	delete[]lpName; delete[]lpValue;
#else
	setenv("UV_THREADPOOL_SIZE", cores_string, 1);
#endif//
	delete[]cores_string;
	::InitializeCriticalSection(&_gw_accept_mutex);
	uv_loop_t* loop = uv_default_loop();
	uv_loop_init(loop);
	uv_update_time(loop);
	int app_thread_id = ::get_thread_id(); //App thread id
	{
		concurrency = concurrency - 1;
		/*[Creating request handler]*/
		if (is_interactive == TRUE)
			fprintf_s(_stdout, "Start %d concurrency handle_request\n", concurrency);
		std::shared_ptr<std::mutex>smutex = std::make_shared<std::mutex>();
		for (unsigned int i = 0; i < concurrency; i++) {
			uv_work_t* queue_work = (uv_work_t*)malloc(sizeof(uv_work_t));
			ASSERT(queue_work != NULL);
			thread__context* ctx = new thread__context;//(thread__context*)malloc(sizeof(thread__context));
			ASSERT(ctx != NULL);
			ctx->is_fserver = TRUE; ctx->mutex = smutex;
			ctx->web_app_root = web_app_root; ctx->is_interactive = is_interactive;
			ctx->app_thread_id = app_thread_id;
			queue_work->data = (void*)ctx;
			uv_queue_work(loop, queue_work, /*[task]*/[](uv_work_t* req) {
				::handle_nonblocking_request(req->data);
			}, /*on_task_end*/[](uv_work_t* req, int status) {
				delete req;
			});
		}
	}
	{
		/*[Creating listener thread]*/
		unsigned int total_listener = 3;
		for (unsigned int i = 0; i < total_listener; i++) {
			uv_thread_t* listener_ctx = (uv_thread_t*)malloc(sizeof(uv_thread_t));
			ASSERT(listener_ctx != NULL);
			thread__context* ctx = new thread__context;//(thread__context*)malloc(sizeof(thread__context));
			ASSERT(ctx != NULL);
			ctx->socket_id = socket_id; ctx->is_fserver = is_fserver;
			ctx->is_interactive = is_interactive; ctx->app_thread_id = app_thread_id;
			uv_thread_create(listener_ctx, ::listen_nonblocking_request, (void*)ctx);
			free(listener_ctx);
		}
		if (is_interactive == TRUE) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			fprintf_s(_stdout, "Total %d nonbloking listener registred.\n", total_listener);
			fprintf_s(_stdout, "All thread registered....\n");
		}
	}
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
	::v8_engine::dispose_engine();
	::DeleteCriticalSection(&_gw_accept_mutex);
	return EXIT_SUCCESS;
}
// Debug https://blogs.msdn.microsoft.com/friis/2012/09/24/easy-debugging-of-cgi-fastcgi-application/
int fcgi_request::request_handler(
	const char* execute_path,
	const char* path, int is_spath,
	const char* web_app_root,
	int is_interactive
) {
	if (is_spath == TRUE && is_interactive == TRUE )
		fprintf_s(_stdout, "Execute dir %s\n", execute_path);
	{
		int err = -1, count = 0, max = 10;
		while (err != 0) {
			err = ::FCGX_Init();
			count++;
			if (count >= max)break;
		}
		if (err != 0) {
			FATAL("Unable to initialize FastCGI module!!!\n");
		}
	}
	int socket_id = FCGI_LISTENSOCK_FILENO;
	if (is_spath == TRUE) {
		socket_id = ::FCGX_OpenSocket(path, 3000);
		if (socket_id < 0) {
			std::string error("Unable to open socket on ");
			error.append(path).append("\n");
			FATAL(error.c_str());
		}
	}
	else {
		if (::FCGX_IsCGI() == TRUE) {
			::cgi_request::request_handler(execute_path);
			::free_resource();
			return EXIT_SUCCESS;
		}
	}
	return ::create_nonblocking_thread(
		execute_path, is_spath, web_app_root, is_interactive, socket_id
	);
}
#endif//!WEB_JSX_MULTI_THREAD
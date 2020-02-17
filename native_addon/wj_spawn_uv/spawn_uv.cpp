/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:00 PM 2/10/2020
#	include "spawn_uv.h"
#	include <uv.h>
#	include <string>
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2
struct Lambda {
	//[ptr_uv_exit_cb]
	template<typename Tret, typename T>
	static Tret lambda_ptr_uv_exit_cb_exec(uv_process_t* req, int exit_status, int term_signal) {
		return (Tret)(*(T*)fn<T>())(req, exit_status, term_signal);
	}
	template<typename Tret = void, typename Tfp = Tret(*)(uv_process_t*, int, int), typename T>
	static Tfp ptr_uv_exit_cb(T& t) {
		fn<T>(&t);
		return (Tfp)lambda_ptr_uv_exit_cb_exec<Tret, T>;
	}
	//[/ptr_uv_exit_cb]
	//[ptr_uv_alloc_cb]
	template<typename Tret, typename T>
	static Tret lambda_ptr_uv_alloc_cb_exec(uv_handle_t* handle, size_t len) {
		return (Tret)(*(T*)fn<T>())(handle, len);
	}
	template<typename Tret = void, typename Tfp = Tret(*)(uv_handle_t*, size_t), typename T>
	static Tfp ptr_uv_alloc_cb(T& t) {
		fn<T>(&t);
		return (Tfp)lambda_ptr_uv_alloc_cb_exec<Tret, T>;
	}
	//[/ptr_uv_alloc_cb]
	//[ptr_uv_read_cb]
	template<typename Tret, typename T>
	static Tret lambda_ptr_uv_read_cb_exec(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
		return (Tret)(*(T*)fn<T>())(stream, nread, buf);
	}
	template<typename Tret = void, typename Tfp = Tret(*)(uv_stream_t*, ssize_t, uv_buf_t), typename T>
	static Tfp ptr_uv_read_cb(T& t) {
		fn<T>(&t);
		return (Tfp)lambda_ptr_uv_read_cb_exec<Tret, T>;
	}
	//[/ptr_uv_read_cb]
	template<typename T>
	static void* fn(void* new_fn = nullptr) {
		static void* fn;
		if (new_fn != nullptr)
			fn = new_fn;
		return fn;
	}
};

int spawn_uv_child_process(
	const char* process_path, 
	const char* start_in, 
	const char* arg
) {
	uv_loop_t* loop; uv_pipe_t channel;
	loop = uv_default_loop();
	uv_pipe_init(loop, &channel, 1);
	//uv_pipe_open(&channel, 1);
	uv_process_t child_req;
	uv_process_options_t options;
	uv_stdio_container_t child_stdio[3]; // {stdin, stdout, stderr} 
	child_stdio[STDIN_FILENO].flags = UV_IGNORE;
	child_stdio[STDOUT_FILENO].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
	child_stdio[STDOUT_FILENO].data.stream = (uv_stream_t*)&channel;
	child_stdio[STDERR_FILENO].flags = UV_IGNORE;
	options.stdio = child_stdio;
	options.stdio_count = sizeof(child_stdio) / sizeof(child_stdio[0]);
	auto on_child_exit = [&](uv_process_t* req, int exit_status, int term_signal) -> void {
		fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
		uv_close((uv_handle_t*)req, NULL);
	};
	//void (*fp)(void*) = lambda_ptr(on_child_exit);
	options.exit_cb = (uv_exit_cb)Lambda::ptr_uv_exit_cb(on_child_exit);
	const char* args[3];
	args[0] = process_path;
	args[1] = arg;
	args[2] = NULL;
	options.cwd = start_in;
	options.file = args[0];
	options.args = (char**)args;
	int r = 0;
	if ((r = uv_spawn(loop, &child_req, &options))) {
		fprintf(stderr, "ERROR: uv_spawn->%s\n", uv_strerror(r));
		return FALSE;
	}
	auto on_alloc_buffer = [&](uv_handle_t* handle, size_t len) -> uv_buf_t {
		printf("alloc_buffer called, requesting a lu byte buffer\n");
		uv_buf_t buf;
		buf.base = new char[len];
		buf.len = static_cast<ULONG>(len);
		return buf;
	};
	auto on_read_pipe = [&](uv_stream_t* stream, ssize_t nread, uv_buf_t buf) -> void {
		//printf("read %s li bytes in a %s lu byte buffer\n", nread, buf.len);
		if (nread + 1 > buf.len) return;
		buf.base[nread] = '\0'; // turn it into a cstring
		printf("read: |%s|", buf.base);
	};
	r = uv_read_start(
		(uv_stream_t*)&channel,
		(uv_alloc_cb)Lambda::ptr_uv_alloc_cb(on_alloc_buffer),
		(uv_read_cb)Lambda::ptr_uv_read_cb(on_read_pipe)
	);
	//return uv_run(loop, UV_RUN_DEFAULT);
	return TRUE;
}
using namespace sow_web_jsx;
void spawn_uv_child_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args.Length() <= 2) {
		throw_js_error(isolate, "argument length should be 3");
		return;
	}
	native_string process_path(isolate, args[0]);
	native_string start_in(isolate, args[0]);
	native_string arg(isolate, args[0]);
	spawn_uv_child_process(
		process_path.c_str(), 
		start_in.c_str(),
		arg.c_str()
	);
	process_path.clear(); start_in.clear(); arg.clear();
}
void export_uv(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	v8::Handle<v8::Object> uv_object = v8::Object::New(isolate);
	uv_object->Set(isolate->GetCurrentContext(), v8_str(isolate, "version"), v8_str(isolate, uv_version_string()));
	wjsx_set_method(isolate, uv_object, "spawn_child_process", spawn_uv_child_process);
	wjsx_set_object(isolate, target, "uv", uv_object);
}

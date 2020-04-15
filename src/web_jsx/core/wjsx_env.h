/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:00 PM 2/21/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _wjsx_env_h)
#	define _wjsx_env_h
#	include <map>
#	include <vector>
#	include <list>
#	include <string>
#	include <sstream>
#	include <iostream>
#	include <fstream>
#	include <v8.h>
#	include <mutex>
#if !defined(__file_exists)
#	include <io.h> 
#	define __file_exists(fname) _access(fname, 0)!=-1
#endif//!__file_exists

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE

#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj

#if !defined(_free_char)
#	define _free_char(obj)\
while(obj){\
	delete[] obj; obj = NULL;\
}
#endif//!_free_char

#if !defined(_NEW_STR)
#	define _NEW_STR(name) std::string* name = new std::string()
#endif//!_NEW_STR

#if !defined(_NEW_STRA)
#	define _NEW_STRA(name, data) std::string* name = new std::string(data)
#endif//!_NEW_STRA

#if !defined(_WCOUT)
#	define _WCOUT wj_env->cout()
#endif//!_WCOUT
#if !defined(_WFLUSH)
#	define _WFLUSH() wj_env->cout().flush()
#endif//!_WFLUSH
#if !defined(_WCERR)
#	define _WCERR wj_env->cerr()
#endif//!_WCERR
#if !defined(_WCIN)
#	define _WCIN wj_env->cin()
#endif//!_WCIN

#if !defined(_stdout)
#	define _stdout __acrt_iob_func(1)
#endif//!_stdout

#if !defined(_stderr)
#	define _stderr __acrt_iob_func(2)
#endif//!_stderr

#if !defined(READ_CHUNK)
#	define READ_CHUNK		16384
#endif//!READ_CHUNK

#if !defined(v8_str)
#define v8_str(isolate, x)\
	v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked()
#endif//!v8_str

#if !defined(typeof_native_obj)
#	define typeof_native_obj v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>
#endif//!typeof_native_obj

/* Die with fatal error. */
#define FATAL(msg)                                        \
  do {                                                    \
    fprintf_s(_stderr,                                    \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    _fflush_nolock(_stderr);							  \
    abort();                                              \
  } while (0)

/* Have our own assert, so we are sure it does not get optimized away in
 * a release build.
 */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf_s(_stderr,                         \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
	  _fflush_nolock(_stderr);							  \
	   abort();                                           \
  }                                                       \
 } while (0)

#define _DELETE DELETE
#undef DELETE

typedef enum {
	NATIVE = 1,
	_JS = 2,
	NO_EXT = 3,
	_UNKNOWN = -1
}typeof_module;

typedef enum {
	GET = 1,
	HEAD = 2,
	POST = 3,
	PUT = 4,
	DELETE = 5,
	CONNECT = 6,
	OPTIONS = 7,
	TRACE = 8,
	UNSUPPORTED = -1
}req_method;

typedef enum {
	JSX = 1,
	JSXH = 2,
	WJSX = 3,
	WJSXH = 4,
	JS = 5,
	RAW_SCRIPT = 6,
	UNKNOWN = -1
}web_extension;

class wjsx_env;
EXPORT_WJSX wjsx_env* unwrap_wjsx_env(v8::Isolate* isolate);
void wrap_wjsx_env(v8::Isolate* isolate, wjsx_env* wj_env);
EXPORT_WJSX void v8_object_extend_internal(
	v8::Isolate * isolate, v8::Local<v8::Context> ctx,
	const v8::Handle<v8::Object> source,
	v8::Local<v8::Object> dest
);

EXPORT_WJSX std::string* concat_str(const char* a, const char* b);
EXPORT_WJSX int load_file_to_vct(std::ifstream & file_stream, std::vector<char> & dest);
EXPORT_WJSX int read_file(const char* path, std::stringstream & stream);
EXPORT_WJSX int read_file(const char* path, std::string & out_put);
EXPORT_WJSX int write_file(const char* path, const char* data, size_t len);
EXPORT_WJSX int file_has_changed(const char* a, const char* b);
void get_file_name(const std::string path_str, std::string& name);
template<class _stream>
int stream_is_available(_stream & strm) {
	return (strm.good() == false || strm.fail() == true) ? FALSE : TRUE;
}
template<class _stream>
size_t get_stream_length(_stream & stream) {
	stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = stream.tellg();
	stream.seekg(0, std::ios::beg);//Back to begain of stream
	return (size_t)totalSize;
}
template<typename T>
T* create_file_stream(const char* path, std::string& error) {
	T* file = new T(path, std::ios_base::ate | std::ios_base::binary);
	if (!file->is_open()) {
		error.append("Unable to open or create file. #");
		error.append(path); delete file;
		return NULL;
	}
	return file;
}
template<class _swap_obj>
void swap_obj(_swap_obj & obj) {
	obj.clear();
	_swap_obj().swap(obj);
}
typedef struct native_modules {
	struct native_modules* next;  /* pointer to next member*/
	void* wj_module;
	const char* name;
}wj_native_modules;
//#pragma warning(push)
//#pragma warning(disable:4251)
class native_data_structure {
private:
	int											_is_disposed;
	std::map<std::string, typeof_native_obj>*	_lib_data;
	std::map<std::string, typeof_native_obj>*	_request_data;
	typeof_native_obj							_global_context;
	wj_native_modules*							_native_modules;
	wj_native_modules*							_working_modules;
	std::shared_ptr<std::mutex>					_mutex;
public:
	native_data_structure();
	native_data_structure(native_data_structure&&) = default;
	native_data_structure& operator=(native_data_structure&&) = delete;
	void set_mutex(std::shared_ptr<std::mutex> mutex);
	std::shared_ptr<std::mutex> get_mutex();
	int store_native_module(void* wj_module, const char* name);
	wj_native_modules* get_native_module();
	int store_working_module(void* wj_module, const char* name);
	wj_native_modules* get_working_module();
	std::map<std::string, typeof_native_obj>& get_lib_obj();
	void add_native_obj(const char* key, typeof_native_obj& value);
	std::map<std::string, typeof_native_obj>& get_request_obj();
	void add_working_obj(const char* key, typeof_native_obj value);
	void* get_lib(const char* key);
	v8::Local<v8::Object>get_module_obj(v8::Isolate* isolate, const char* key);
	int exists_module(const char* key);
	void clear();
	~native_data_structure();
};
//#pragma warning(pop)
/*
wjsx_env& wj = *wj_env;
wj << "rajib";
wj << 1 << "" << "rajib" << wj_env;
wj >> (*out_str) >> (*out_str) >> (*abs_path) >> (*out_str);
*/
/*[function pointers]*/
typedef void (*on_async_complete)(wjsx_env* wj_env, v8::Isolate* isolate);
/*[/function pointers]*/
class EXPORT_WJSX wjsx_env {
private:
	int									_is_cli;
	int									_is_flush;
	int									_is_disposed;
	int									_is_thread_req;
	int									_is_script;
	int									_is_runtime_error;
	int									_is_interactive;
	int									_my_thread_id;
	int									_app_thread_id;
	int									_total_processed;
	void*								_user_data;
	std::string*						_root_dir;
	std::string*						_app_dir;
	std::ostream*						_cout;
	std::istream*						_cin;
	std::ostream*						_cerr;
	std::stringstream*					_body;
	native_data_structure*				_native_data;
	std::map<std::string, std::string>* _headers;
	std::vector<std::string>*			_cookies;
	std::vector<std::string>*			_http_status;
	on_async_complete					_on_async_complete;
public:
	int compiled_cached;
	int check_file_state;
	explicit wjsx_env(int is_interactive, int is_thread_req);
	template<class _istream, class _ostream>
	explicit wjsx_env(
		_istream* in, _ostream* out, _ostream* err,
		int thread_req = FALSE, int my_thread_id = 0,
		int app_thread_id = 0
	) {
		_body = new std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		_headers = new std::map<std::string, std::string>();
		_cookies = new std::vector<std::string>();
		_http_status = new std::vector<std::string>();
		_is_flush = FALSE; _is_disposed = FALSE; _my_thread_id = my_thread_id;
		compiled_cached = TRUE; check_file_state = TRUE; _app_thread_id = app_thread_id;
		_native_data = NULL; _root_dir = NULL; _app_dir = NULL; _is_cli = FALSE;
		_is_thread_req = thread_req; _is_script = FALSE; _is_interactive = FALSE;
		_is_runtime_error = TRUE; _user_data = NULL; _on_async_complete = NULL;
		_cin = in; _cout = out; _cerr = err;
		_cout->sync_with_stdio(false); _total_processed = 1;
		_body->setf(std::ios_base::binary);
		_cin->setf(std::ios_base::binary);
		_cout->setf(std::ios_base::binary);
		_cerr->setf(std::ios_base::binary);
	}
	wjsx_env(wjsx_env&&) = default;
	wjsx_env& operator=(wjsx_env&&) = delete;
	/*wjsx_env(const wjsx_env&wj_env) {
		*this = wj_env;
	};
	__forceinline wjsx_env& operator=(const wjsx_env&) {
		return *this;
	}*/
	template<class T>
	T* get_user_data() {
		if (_user_data == NULL)return NULL;
		return static_cast<T*>(_user_data);
	}
	std::shared_ptr<std::mutex> get_mutex();
	void set_user_data(void* data);
	void set_on_async_complete(on_async_complete callback);
	void on_task_complete(v8::Isolate* isolate);
	int get_my_thread_id()const;
	int get_app_thread_id()const;
	int is_interactive()const;
	int is_thread_req()const;
	void set_total_handled_req(int processed);
	int get_total_handled_req()const;
	int is_cli()const;
	int is_available_out_stream();
	int is_available_in_stream();
	void set_runtime_error(int is_error);
	int is_runtime_error()const;
	void set_mood_script();
	int is_script_mood()const;
	void set_root_dir(const char* rdir);
	const char* get_root_dir();
	void set_app_dir(const char* adir);
	const char* get_app_dir();
	std::ostream& cout();
	std::istream& cin();
	std::ostream& cerr();
	std::stringstream& body();
	size_t write_b(const char* data);
	void clear_body(int create_new);
	int flush();
	int is_flush()const;
	void set_flush();
	std::map<std::string, std::string>& get_http_header();
	std::vector<std::string>& get_http_cookies();
	std::vector<std::string>& get_http_status();
	/*Write to output stream*/
	template<class T>
	friend std::ostream& operator<<(wjsx_env& env, T obj) {
		return env.cout() << obj;
	}
	/*Read from input stream*/
	template<class T>
	friend std::istream& operator>>(wjsx_env& env, T obj){
		return env.cin() >> obj;
	}
	void clear();
	void free_native_data_structure();
	void set_native_data_structure(native_data_structure& nds);
	int has_native_data_structure();
	native_data_structure* get_native_data_structure(int create);
	~wjsx_env();
};
#endif//!_wjsx_env_h
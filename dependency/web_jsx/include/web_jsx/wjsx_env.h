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
#	include <string>
#	include <sstream>
#	include <iostream>
#	include <fstream>
#	include <v8.h>
#	include <mutex>
#	include <memory>
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

#if !defined(WJSX_API)
#	define WJSX_API(type) type
#endif//!WJSX_API

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

/*
wjsx_env& wj = *wj_env;
wj << "rajib";
wj << 1 << "" << "rajib" << wj_env;
wj >> (*out_str) >> (*out_str) >> (*abs_path) >> (*out_str);
*/
class wjsx_env;
WJSX_API(wjsx_env*) unwrap_wjsx_env(v8::Isolate * isolate);
WJSX_API(void) v8_object_extend_internal(
	v8::Isolate * isolate, v8::Local<v8::Context> ctx,
	const v8::Handle<v8::Object> source,
	v8::Local<v8::Object> dest
);
WJSX_API(std::string*) concat_str(const char* a, const char* b);
WJSX_API(int) load_file_to_vct(std::ifstream & file_stream, std::vector<char> & dest);
WJSX_API(int) read_file(const char* path, std::string & out_put);
WJSX_API(int) read_file(const char* path, std::stringstream & stream);
WJSX_API(int) write_file(const char* path, const char* data, size_t len);
WJSX_API(int) file_has_changed(const char* a, const char* b);

template<class _stream>
int stream_is_available(_stream & strm) {
	return (strm.good() == false || strm.fail() == true) ? FALSE : TRUE;
}
template<class _stream>
size_t get_stream_length(_stream& stream) {
	stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = stream.tellg();
	stream.seekg(0, std::ios::beg);//Back to begain of stream
	return (size_t)totalSize;
}
template<class _swap_obj>
void swap_obj(_swap_obj& obj) {
	obj.clear();
	_swap_obj().swap(obj);
}

class wjsx_env {
public:
	int compiled_cached;
	int check_file_state;
	std::shared_ptr<std::mutex> get_mutex();
	int get_total_handled_req()const;
	int is_thread_req()const;
	int get_my_thread_id()const;
	int get_app_thread_id()const;
	int is_interactive()const;
	int is_cli()const;
	int is_available_out_stream();
	int is_available_in_stream();
	std::ostream& cout();
	std::istream& cin();
	std::ostream& cerr();
	std::stringstream& body();
	size_t write_b(const char* data);
	void clear_body(int create_new);
	int flush();
	int is_flush()const;
	void set_flush();
	const char* get_root_dir();
	const char* get_app_dir();
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
	friend std::istream& operator>>(wjsx_env& env, T obj) {
		return env.cin() >> obj;
	}
	void free();
	~wjsx_env();
};
#endif//!_wjsx_env_h
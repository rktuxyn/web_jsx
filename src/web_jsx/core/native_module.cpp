/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "native_module.h"
#	include "v8_util.h"
#	include "web_jsx_global.h"
//2:15 PM 1/14/2020

#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
typedef void* h_module;
#else
typedef HMODULE h_module;
#endif//!_WIN32
typedef void(*web_jsx_native_module)(v8::Handle<v8::Object>target);

typedef enum {
	LOCAL = 1,
	WORKING = 2
}typeof_storage;

typedef struct native_modules {
	struct native_modules* next;  /* pointer to next member*/
	h_module wj_module;
}wj_native_modules;

#if !defined(n_wjm)
#	define n_wjm new wj_native_modules
#endif//!n_wjm

#if !defined(_free_module)
#	define _free_module(module_obj)\
while(module_obj){\
wj_native_modules* wjm = module_obj;\
module_obj = module_obj->next;\
FreeLibrary(wjm->wj_module);\
delete wjm; wjm = NULL;\
}
#endif//!_free_module

#if !defined(_store_module)
#	define _store_module(module_obj, wj_module)\
if(module_obj==NULL){\
module_obj = n_wjm;\
module_obj->wj_module = wj_module;\
module_obj->next = NULL;\
}else{\
wj_native_modules* anm = n_wjm;\
anm->wj_module = wj_module;\
anm->next = module_obj;\
module_obj = anm;\
}
#endif//!_store_module

wj_native_modules* _working_modules = NULL;
wj_native_modules* _local_modules = NULL;

void store_module(h_module wj_module, typeof_storage st = LOCAL) {
	if (st == LOCAL) {
		_store_module(_local_modules, wj_module);
	}
	else {
		_store_module(_working_modules, wj_module);
	}
}
void sow_web_jsx::free_working_module() {
	_free_module(_working_modules);
}
void sow_web_jsx::free_native_modules() {
	_free_module(_local_modules);
	_free_module(_working_modules);
}
//wchar_t* s2ws(const char* mbstr) {
//	mbstate_t state;
//	memset(&state, 0, sizeof state);
//	size_t len = sizeof(wchar_t) + mbsrtowcs(NULL, &mbstr, 0, &state);
//	wchar_t* buf = new wchar_t[len];
//	mbsrtowcs(buf, &mbstr, len, &state);
//	return buf;
//}
//UNICODE
void* get_proc_address(h_module module_name, const char* name) {
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Not Implimented
#else
	return ::GetProcAddress(module_name, name);
#endif//!_WIN32||_WIN64
}
void js_throw_error(v8::Isolate* isolate, const char* reason, const char* module_name) {
	std::string error = reason; error += " Module#"; error += module_name;
	isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, error.c_str(), v8::NewStringType::kNormal).ToLocalChecked()));
	error.clear(); std::string().swap(error);
}
typeof_module sow_web_jsx::get_module_type(const std::string& path_str) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return NO_EXT;
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jn") return NATIVE;
	if (str_extension == "dll") return NATIVE;
	if (str_extension == "js") return _JS;
	return _UNKNOWN;
}

void sow_web_jsx::require_native(
	const v8::FunctionCallbackInfo<v8::Value>& args, 
	const char* abs_path,
	const char* app_dir,
	const char* module_name
){
	v8::Isolate* isolate = args.GetIsolate();
	typeof_module ext = get_module_type(module_name);
	if (ext != typeof_module::NATIVE) {
		js_throw_error(isolate, "This is not valid web_jsx module.. Supported extension *.jn, *.dll.", module_name);
		return;
	}
	const char* path = abs_path;
	if (__file_exists(path) == false) {
		js_throw_error(isolate, "Native module not found...", module_name);
		return;
	}
	h_module wj_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	wj_module = dlopen(path, RTLD_NOW);
#else
	SetDllDirectoryA(app_dir);
	wchar_t* wpath = sow_web_jsx::ccr2ws(path);
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[]wpath;
#endif//!_WIN32||_WIN64
	if (wj_module == NULL) {
		js_throw_error(isolate, "Unable to load this native module..", module_name);
		return;
	}
	void* wjnef = get_proc_address(wj_module, "web_jsx_native_module");
	if (wjnef == NULL) {
		js_throw_error(isolate, "This is not valid web_jsx module..", module_name);
	}
	else {
		web_jsx_native_module func = (web_jsx_native_module)wjnef;
		v8::Handle<v8::Object> target = v8::Object::New(isolate);
		func(target);
		args.GetReturnValue().Set(target);
		func = NULL; wjnef = NULL; target.Clear();
	}
	store_module(wj_module, WORKING);
	return;
}

int sow_web_jsx::load_native_module(
	v8::Isolate* isolate,
	v8::Handle<v8::Object> target, 
	const char* path,
	const char* app_dir
){
	int result = FALSE;
	h_module wj_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	wj_module = dlopen(path, RTLD_NOW);
#else
	SetDllDirectoryA(app_dir);
	wchar_t* wpath = sow_web_jsx::ccr2ws(path);
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[]wpath;
#endif//!_WIN32||_WIN64
	if (wj_module == NULL)return result;
	void* wjnef = get_proc_address(wj_module, "web_jsx_native_module");
	if (wjnef != NULL) {
		web_jsx_native_module func = (web_jsx_native_module)wjnef;
		func(target);
		func = NULL; wjnef = NULL;
		result = TRUE;
	}
	store_module(wj_module);
	return result;
}
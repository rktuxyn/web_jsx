/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "native_module.h"
//2:15 PM 1/14/2020
typedef enum {
	LOCAL = 1,
	WORKING = 2
}typeof_storage;

typedef struct native_modules {
	struct native_modules* next;  /* pointer to next member*/
	h_module wj_module;
}wj_native_modules;

#	define n_wjm new wj_native_modules

wj_native_modules* _working_modules = NULL;
wj_native_modules* _local_modules = NULL;
void _store_module(wj_native_modules*storage, h_module wj_module) {
	if (storage == NULL) {
		storage = n_wjm;
		storage->wj_module = wj_module;
		storage->next = NULL;
		return;
	}
	wj_native_modules* anm = n_wjm;
	anm->wj_module = wj_module;
	anm->next = storage;
	storage = anm;
}
void store_module(h_module wj_module, typeof_storage st = LOCAL) {
	_store_module(st == LOCAL ? _local_modules : _working_modules, wj_module);
}
void _free_modules(wj_native_modules* storage) {
	if (storage == NULL)return;
	wj_native_modules* wjm;
	while (storage) {
		wjm = storage;
		storage = storage->next;
		FreeLibrary(wjm->wj_module);
		delete wjm; wjm = NULL;
	}
	if (storage != NULL)
		delete storage;
	storage = NULL;
}
void sow_web_jsx::free_working_module() {
	_free_modules(_working_modules);
}
void sow_web_jsx::free_native_modules() {
	_free_modules(_local_modules);
	_free_modules(_working_modules);
}

wchar_t* s2ws(const char* s) {
	wchar_t* buf = new wchar_t[_MAX_PATH]();
	mbsrtowcs(buf, &s, _MAX_PATH, NULL);
	return buf;
}
//UNICODE
void* get_proc_address(HMODULE module_name, const char* name) {
	return ::GetProcAddress(module_name, name);
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
	const std::string abs_path,
	const std::string app_dir, 
	const char* module_name
){
	v8::Isolate* isolate = args.GetIsolate();
	typeof_module ext = get_module_type(module_name);
	if (ext != typeof_module::NATIVE) {
		js_throw_error(isolate, "This is not valid web_jsx module.. Supported extension *.jn, *.dll.", module_name);
		return;
	}
	const char* path = abs_path.c_str();
	if (__file_exists(path) == false) {
		js_throw_error(isolate, "Native module not found...", module_name);
		return;
	}
	h_module wj_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	wj_module = dlopen(path, RTLD_NOW);
#else
	SetDllDirectoryA(app_dir.c_str());
	wchar_t* wpath = s2ws(path);
	//wj_module = LoadLibraryEx(wpath, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	//wj_module = LoadLibraryW(wpath);
	//wj_module = LoadLibraryEx(wpath, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[]wpath;
#endif
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
	const std::string app_dir
){
	int result = FALSE;
	h_module wj_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	wj_module = dlopen(path, RTLD_NOW);
#else
	//"D:\\Node\\Projects\\win\\x64\\Release\\"
	SetDllDirectoryA(app_dir.c_str());
	wchar_t* wpath = s2ws(path);
	//LOAD_WITH_ALTERED_SEARCH_PATH
	//wj_module = LoadLibraryEx(wpath, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
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
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

#if !(defined(_WIN32)||defined(_WIN64))
typedef void* h_module;
#else
typedef HMODULE h_module;
#endif//!_WIN32
typedef void(*web_jsx_native_module)(v8::Handle<v8::Object>target);

//UNICODE
void* get_proc_address(h_module module_name, const char* name) {
#if !(defined(_WIN32)||defined(_WIN64))
#error Not Implimented
#else
	return ::GetProcAddress(module_name, name);
#endif//!_WIN32||_WIN64
}
void js_throw_error(v8::Isolate* isolate, const char* reason, const char* module_name) {
	std::string *error = new std::string(reason);
	error->append(" Module#");
	error->append(module_name);
	throw_js_error(isolate, error->c_str());
	_free_obj(error);
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
	const char* file_name
) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string module_name;
	::get_file_name(file_name, module_name);
	typeof_module ext = get_module_type(module_name);
	if (ext != typeof_module::NATIVE) {
		js_throw_error(isolate, "This is not valid web_jsx module.. Supported extension *.jn, *.dll.", module_name.c_str());
		return;
	}
	const char* path = abs_path;
	if (__file_exists(path) == false) {
		js_throw_error(isolate, "Native module not found...", module_name.c_str());
		return;
	}
	v8::Handle<v8::Object> target = v8::Object::New(isolate);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	int has_lib = FALSE; h_module wj_module = NULL;
	native_data_structure* data_struct = NULL;
	if (wj_env->has_native_data_structure() == TRUE) {
		data_struct = wj_env->get_native_data_structure(FALSE);
		target = data_struct->get_module_obj(isolate, module_name.c_str());
		if (!target->IsNullOrUndefined()) {
			args.GetReturnValue().Set(target);
			return;
		}
		wj_module = (h_module)data_struct->get_lib(module_name.c_str());
		has_lib = wj_module != NULL;
	}
	
#if !(defined(_WIN32)||defined(_WIN64))
	wj_module = dlopen(path, RTLD_NOW);
#else
	SetDllDirectoryA(app_dir);
	wchar_t* wpath = sow_web_jsx::ccr2ws(path);
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[]wpath;
#endif//!_WIN32||_WIN64
	if (wj_module == NULL) {
		js_throw_error(isolate, "Unable to load this native module..", module_name.c_str());
		return;
	}
	void* wjnef = get_proc_address(wj_module, "web_jsx_native_module");
	if (wjnef == NULL) {
		js_throw_error(isolate, "This is not valid web_jsx module..", module_name.c_str());
		FreeLibrary(wj_module);
	}
	else {
		web_jsx_native_module func = (web_jsx_native_module)wjnef;
		func(target);
		if (has_lib == FALSE && data_struct != NULL) {
			data_struct->store_working_module(wj_module, module_name.c_str());
		}
		if (data_struct != NULL) {
			typeof_native_obj pobj(isolate, target);
			data_struct->add_working_obj(module_name.c_str(), pobj);
		}
		args.GetReturnValue().Set(target);
		func = NULL; wjnef = NULL;
	}
	return;
}

void* sow_web_jsx::load_native_module(
	v8::Isolate* isolate,
	v8::Handle<v8::Object> target, 
	const char* path,
	const char* app_dir
){
	h_module wj_module;
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	wj_module = dlopen(path, RTLD_NOW);
#else
	SetDllDirectoryA(app_dir);
	wchar_t* wpath = sow_web_jsx::ccr2ws(path);
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[]wpath;
#endif//!_WIN32||_WIN64
	if (wj_module == NULL)return NULL;
	void* wjnef = get_proc_address(wj_module, "web_jsx_native_module");
	if (wjnef != NULL) {
		web_jsx_native_module func = (web_jsx_native_module)wjnef;
		func(target);
		func = NULL; wjnef = NULL;
		return wj_module;
	}
	FreeLibrary(wj_module);
	return NULL;
}
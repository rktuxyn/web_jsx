/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "native_module.h"
//2:15 PM 1/14/2020
wchar_t* s2ws(const char* s) {
	wchar_t* buf = new wchar_t[_MAX_PATH]();
	mbsrtowcs(buf, &s, _MAX_PATH, NULL);
	//LPWSTR& ptr = buf;
	//return ptr;
	return buf;
}
//UNICODE
void* get_proc_address(HMODULE module, const char* name) {
	return ::GetProcAddress(module, name);
}
void js_throw_error(v8::Isolate* isolate, const char* reason, const char* module_name) {
	std::string error = reason; error += " Module#"; error += module_name;
	isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, error.c_str(), v8::NewStringType::kNormal).ToLocalChecked()));
	error.clear(); std::string().swap(error);
}
sow_web_jsx::typeof_module sow_web_jsx::get_module_type(const std::string& path_str) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return typeof_module::NO_EXT;
	std::string str_extension = path_str.substr(found + 1);
	if (str_extension == "jn") return typeof_module::NATIVE;
	if (str_extension == "dll") return typeof_module::NATIVE;
	if (str_extension == "js") return typeof_module::JS;
	return typeof_module::UNKNOWN;
}
void sow_web_jsx::require_native(const v8::FunctionCallbackInfo<v8::Value>& args, const std::string abs_path, const std::string app_dir, const char* module_name){
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
	//"D:\\Node\\Projects\\win\\x64\\Release\\"
	SetDllDirectoryA(app_dir.c_str());
	wchar_t* wpath = s2ws(path);
	//LOAD_WITH_ALTERED_SEARCH_PATH
	wj_module = LoadLibraryEx(wpath, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
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
	FreeLibrary(wj_module);
	wj_module = NULL;
	return;
}

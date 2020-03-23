/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:27 PM 1/28/2020 ==> 11:36 PM 1/28/2020

#	include "module_store.h"
#	include "v8_util.h"
#	include "native_module.h"
#	include "wjsx_env.h"
//#include	<regex>
#if !defined(FALSE)
#	define FALSE               0
#endif

#if !defined(TRUE)
#	define TRUE                1
#endif

#if !defined(_ERROR)
#	define _ERROR             -1
#endif

using namespace sow_web_jsx;
//std::shared_ptr<std::vector<typeof_native_obj>> _module_map = NULL;

int is_active_module(const std::string module_info) {
	if (module_info.empty())return FALSE;
	if (module_info.find("#") != std::string::npos)return FALSE;//comented module...
	return TRUE;
}
std::istream& get_line(
	std::ifstream& is, std::string& t
) {
	t.clear();
	std::streambuf* sb = is.rdbuf();
	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			c = sb->sgetc();
			if (c == '\n')sb->sbumpc();
			return is;
		case EOF:
			if (t.empty())is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}
void get_module_name(const std::string& path_str, std::string& name) {
	size_t found = path_str.find_last_of("/\\");
	name = path_str.substr(found, path_str.size());
}
int prepare_native_module(
	v8::Isolate* isolate,
	native_data_structure* native_data,
	std::map<std::string, typeof_native_obj>& data,
	const char* app_dir,
	const char* root_dir
){
	int is_loaded = FALSE;
	std::string ex_path(root_dir);
	ex_path.append("module.cfg");
	if (__file_exists(ex_path.c_str()) == false) {
		swap_obj(ex_path);
		ex_path = std::string(app_dir);
		ex_path.append("module.cfg");
		if (__file_exists(ex_path.c_str()) == false) {
			is_loaded = _ERROR;
			std::string err("module.cfg not found in web_jsx app directory... App Dir:");
			err.append(app_dir);
			throw_js_error(isolate, err.c_str());
			swap_obj(ex_path); swap_obj(err);
			return is_loaded;
		}
	}
	
	std::ifstream* file = new std::ifstream(ex_path, std::ios::out);
	swap_obj(ex_path);
	std::string line, path;
	do {
		get_line(*file, line);
		if (is_active_module(line) == FALSE)continue;
		path = app_dir;
		path.append(line);
		if (__file_exists(path.c_str()) == true) {
			v8::Handle<v8::Object> target = v8::Object::New(isolate);
			if (native_data->store_native_module(::load_native_module(isolate, target, path.c_str(), app_dir)) == FALSE) {
				is_loaded = _ERROR;
				path = "Unable to load native module-> " + line;
				throw_js_error(isolate, path.c_str());
				file->close(); delete file; swap_obj(path); swap_obj(line);
				return is_loaded;
			}
			is_loaded = TRUE;
			typeof_native_obj pobj(isolate, target);
			_NEW_STR(module_name);
			get_module_name(path, *module_name);
			data[module_name->c_str()] = pobj;
		}
		else {
			file->close(); delete file;
			is_loaded = _ERROR;
			swap_obj(path);
			path = "native module-> ";
			path.append(line.c_str());
			path.append(" Not found.");
			path.append("Root dir-> ");
			path.append(app_dir);
			throw_js_error(isolate, path.c_str());
			swap_obj(path); swap_obj(line);
			return FALSE;
		}
		path.clear();
	} while (!file->eof());
	file->close();
	delete file;
	swap_obj(path); swap_obj(line);
	return is_loaded;
}
void _scope_to_js_global(
	v8::Isolate* isolate,
	v8::Local<v8::Context>context,
	native_data_structure& native_data
) {
	v8::Local<v8::Object> js_this = context->Global()->GetPrototype().As<v8::Object>();
	auto lib_obj = native_data.get_lib_obj();
	for (auto itr = lib_obj.begin(); itr != lib_obj.end(); ++itr) {
		v8::Local<v8::Object> source = v8::Local<v8::Object>::New(isolate, itr->second);
		::v8_object_extend_internal(isolate, context, source, js_this);
	}
	auto req_obj = native_data.get_request_obj();
	for (auto itr = req_obj.begin(); itr != req_obj.end(); ++itr) {
		v8::Local<v8::Object> source = v8::Local<v8::Object>::New(isolate, (itr->second));
		::v8_object_extend_internal(isolate, context, source, js_this);
	}
}
void get_module_by_name(
	const v8::FunctionCallbackInfo<v8::Value>& args
) {
	if (args.Length() == 0) {
		throw_js_error(args.GetIsolate(), "Module name required...");
		return;
	}
	if (!args[0]->IsString()) {
		throw_js_type_error(args.GetIsolate(), "Module name should be string...");
		return;
	}
	v8::Isolate* isolate = args.GetIsolate();
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	native_string module_name(isolate, args[0]);
	native_data_structure* native_data = wj_env->get_native_data_structure(FALSE);
	auto lib_obj = native_data->get_lib_obj();//
	bool is_request_module = false;
	is_request_module = to_boolean(isolate, args[1]);
	//!TODO
}
void swjsx_module::scope_to_js_global(
	v8::Isolate* isolate, v8::Local<v8::Context>context,
	wjsx_env* wj_env
) {
	native_data_structure* data = wj_env->get_native_data_structure(FALSE);
	if (data == NULL)return;
	_scope_to_js_global(isolate, context, *data);
}
void swjsx_module::implimant_native_module(
	const v8::FunctionCallbackInfo<v8::Value>& args, 
	const char* app_dir,
	const char* root_dir
) {
	v8::Isolate* isolate = args.GetIsolate();
	v8::Locker locker(isolate);
	v8::Isolate::Scope isolate_scope(isolate);
	v8::HandleScope handleScope(isolate);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	native_data_structure* native_data = wj_env->get_native_data_structure(FALSE);//::unwrap_isolate_data<native_data_structure>(isolate, 1, FALSE);
	if (native_data != NULL) {
		_scope_to_js_global(isolate, isolate->GetCurrentContext(), *native_data);
		v8::Unlocker unlocker(isolate);
		return;
	}
	native_data = wj_env->get_native_data_structure(TRUE);//new native_data_structure();
	int ret = prepare_native_module(isolate, native_data, native_data->get_lib_obj(), app_dir, root_dir);
	if (ret == FALSE || ret == _ERROR) {
		_free_obj(native_data);
		v8::Unlocker unlocker(isolate);
		return;
	}
	_scope_to_js_global(isolate, isolate->GetCurrentContext(), *native_data);
	v8::Unlocker unlocker(isolate);
}
#if (defined(_WIN32)||defined(_WIN64))
	typedef HMODULE h_module;
	#define _free_library ::FreeLibrary
#else
	typedef void* h_module;
	#define _free_library FreeLibrary
#endif//!_WIN32

#if !defined(_free_module)
#	define _free_module(module_obj)					\
while( module_obj ) {								\
	wj_native_modules* wjm = module_obj;			\
	module_obj = module_obj->next;					\
	_free_library( ( h_module )wjm->wj_module );	\
}
#endif//!_free_module
void swjsx_module::clean_working_module(native_data_structure* native_data) {
	if (native_data == NULL)return;
	wj_native_modules* wnm = native_data->get_working_module();
	_free_module(wnm);
}
void swjsx_module::clean_native_module(native_data_structure* native_data) {
	if (native_data == NULL)return;
	wj_native_modules* wnm = native_data->get_native_module();
	_free_module(wnm);
	clean_working_module(native_data);
}
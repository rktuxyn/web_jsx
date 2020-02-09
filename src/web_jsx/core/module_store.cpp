/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:27 PM 1/28/2020 ==> 11:36 PM 1/28/2020
#include	"module_store.h"
#include	"v8_util.h"
#include	"native_module.h"
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

#define _swap_str(str)\
str.clear();\
std::string().swap(str)

using namespace sow_web_jsx;
std::shared_ptr<std::vector<v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>>> _module_map = NULL;
//std::shared_ptr<std::vector<v8::Persistent<v8::Object>>>_module_map = NULL;
int _is_loaded = FALSE;
int is_active_module(std::string module_info) {
	if (module_info.empty())return FALSE;
	if (module_info.find("#") != std::string::npos)return FALSE;//comented module...
	return TRUE;
}
std::istream& get_line(std::ifstream& is, std::string& t) {
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
int prepare_native_module(v8::Isolate* isolate, 
	const char* app_dir,
	const char* root_dir
){
	std::string ex_path(root_dir);
	ex_path.append("module.cfg");
	if (__file_exists(ex_path.c_str()) == false) {
		_swap_str(ex_path);
		ex_path = std::string(app_dir);
		ex_path.append("module.cfg");
		if (__file_exists(ex_path.c_str()) == false) {
			_is_loaded = _ERROR;
			std::string err("module.cfg not found in web_jsx app directory... App Dir:");
			err.append(app_dir);
			throw_js_error(isolate, err.c_str());
			err.clear(); std::string().swap(err);
			return FALSE;
		}
	}
	
	std::ifstream file;
	file.open(ex_path, std::ofstream::out | std::ofstream::binary);
	ex_path.clear();
	_swap_str(ex_path);
	std::string line, path;
	//const char* module_dirc = app_dir.c_str();
	do {
		get_line(file, line);
		if (is_active_module(line) == FALSE)continue;
		path = app_dir;
		path.append(line);
		if (__file_exists(path.c_str()) == true) {
			v8::Handle<v8::Object> target = v8::Object::New(isolate);
			if (::load_native_module(isolate, target, path.c_str(), app_dir) == FALSE) {
				if (_is_loaded == TRUE) {
					swjsx_module::clean_native_module();
				}
				path = "Unable to load native module-> " + line;
				throw_js_error(isolate, path.c_str());
				file.close(); std::string().swap(path);
				return FALSE;
			}
			if (_is_loaded == FALSE) {
				_module_map = std::unique_ptr<std::vector<v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>>>(new std::vector<v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>>());
				_is_loaded = TRUE;
			}
			v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, target);
			_module_map->push_back(pobj);
		}
		else {
			path = "native module-> ";
			path.append(line.c_str());
			path.append(" Not found.");
			path.append("Root dir-> ");
			path.append(app_dir);
			throw_js_error(isolate, path.c_str());
			file.close(); _swap_str(path); _swap_str(line);
			if (_is_loaded == TRUE) {
				swjsx_module::clean_native_module();
			}
			return FALSE;
		}
		path.clear();
	} while (!file.eof());
	_swap_str(path); _swap_str(line);
	return _is_loaded;
}
void add_to_js_global_internal(
	v8::Isolate* isolate,
	v8::Local<v8::Context>ctx,
	v8::Local<v8::Object> js_this, 
	const v8::Handle<v8::Object>target
) {
	v8::Local<v8::Array> property_names = target->GetOwnPropertyNames(ctx).ToLocalChecked();
	uint32_t length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		v8::Local<v8::Value> key = property_names->Get(ctx, i).ToLocalChecked();
		if (!key->IsString())continue;
		v8::Local<v8::Value> value = target->Get(ctx, key).ToLocalChecked();
		if (value->IsNullOrUndefined())continue;
		js_this->Set(ctx, key, value);
	}
}
void swjsx_module::scope_to_js_global(v8::Isolate* isolate, v8::Local<v8::Context>context) {
	if (_is_loaded == FALSE || _is_loaded == _ERROR)return;
	v8::Local<v8::Object> js_this = context->Global()->GetPrototype().As<v8::Object>();
	for (auto itr = _module_map->begin(); itr != _module_map->end(); ++itr) {
		v8::Local<v8::Object> target = v8::Local<v8::Object>::New(isolate, (*itr));
		add_to_js_global_internal(isolate, context, js_this, target);
	}
}
void swjsx_module::implimant_native_module(
	const v8::FunctionCallbackInfo<v8::Value>& args, 
	const char* app_dir,
	const char* root_dir
) {
	if (_is_loaded == _ERROR)return;
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_loaded == TRUE) {
		scope_to_js_global(isolate, isolate->GetCurrentContext());
		return;
	}
	if (prepare_native_module(isolate, app_dir, root_dir) == FALSE)return;
	scope_to_js_global(isolate, isolate->GetCurrentContext());
}

void swjsx_module::clean_native_module() {
	sow_web_jsx::free_native_modules();
	if (_is_loaded == FALSE || _is_loaded == _ERROR)return;
	_module_map.reset(); _module_map = NULL; _is_loaded = FALSE;
}
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:46 AM 11/21/2018
#	include "native_wrapper.h"
#	include "v8_util.h"
#	include "zgzip.hpp"
#	include "n_help.h"
#	include "native_module.h"
#	include "module_store.h"
#	include "js_polyfill.h"
using namespace sow_web_jsx;
void __clear_cache(wjsx_env& wj_env) {
	swjsx_module::clean_working_module(wj_env.get_native_data_structure(FALSE));
}
int _write_http_status(wjsx_env& wj_env) {
	wj_env.clear_body(FALSE);
	n_help::write_http_status(wj_env, false);
	n_help::write_header(wj_env);
	n_help::write_cookies(wj_env);
	wj_env << "\r\n";
	__clear_cache(wj_env);
	return FALSE;
}
int write_http_status(wjsx_env& wj_env) {
	response_status status_code = n_help::get_http_response_status(wj_env.get_http_status());
	switch (status_code)
	{
	case response_status::OK: return TRUE;
	case response_status::MOVED:
	case response_status::REDIRECT:
	case response_status::SEEOTHER:
	case response_status::NOTMODIFIED: return _write_http_status(wj_env);
	case response_status::UNAUTHORIZED:
	case response_status::FORBIDDEN:
	case response_status::NOT_FOUND:
	case response_status::INTERNAL_SERVER_ERROR:
	case response_status::NOT_IMPLEMENTED:
		if (wj_env.is_available_out_stream() == FALSE)return FALSE;
		n_help::error_response(
			/*const char* server_root*/wj_env.get_root_dir(),
			/*response_status status_code*/status_code,
			/*const std::string error_msg*/wj_env.body().str().c_str(),
			/*wjsx_env* wj_env*/wj_env
		);
		__clear_cache(wj_env);
		return FALSE;
	default:
		throw new std::runtime_error("Invalid Http Response Status defined...");
	}
}
/*[zgip]*/
void gzip_compress_write(wjsx_env&wj_env) {
	if (write_http_status(wj_env) == FALSE)return;
	//if (_set_binary_output() == FALSE)return;
	n_help::write_http_status(wj_env, false);
	n_help::write_header(wj_env);
	n_help::write_cookies(wj_env);
	__clear_cache(wj_env);
	wj_env << "\r\n";
	gzip::compress_gzip(wj_env.body(), wj_env.cout());
	wj_env.clear_body(FALSE);
	wj_env.cout().flush();
}
/*[/zgip]*/
void add_gscript(std::string& out_script) {
	std::stringstream js_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	sow_web_jsx::create_wj_core_extend_script(js_stream, TRUE);
	out_script.append(js_stream.str());
	swap_obj(js_stream);
	out_script +=
		"if ( typeof( this[\"__wj_core\"] ) !== \"object\" ) throw new Error(\"Please add wj_core.dll\");\n"
		"__extend( this, __wj_core.global, true );\n";
}
std::string* load_script(
	v8::Isolate* isolate, const char* path,
	v8::Local<v8::Function> converter
) {
	_NEW_STR(error);
	std::ifstream* file = ::create_file_stream<std::ifstream>(path, *error);
	if (file == NULL) {
		throw_js_error(isolate, error->c_str()); _free_obj(error);
		return NULL;
	}
	_free_obj(error);
	std::vector<char>* dest = new std::vector<char>();
	int ret = ::load_file_to_vct(*file, *dest);
	file->close(); delete file;
	if (ret == FALSE) {
		_free_obj(dest);
		throw_js_error(isolate, "Unable to read script source...");
		return NULL;
	}
	if (!converter.IsEmpty()) {
		std::string*out_script = new std::string(dest->data(), dest->size());
		_free_obj(dest);
		v8::Local<v8::Context>context = isolate->GetCurrentContext();
		v8::Handle<v8::Value> arg[1] = {
			v8_str(isolate, out_script->c_str())
		};
		_free_obj(out_script);
		v8::MaybeLocal<v8::Value>result = converter->Call(context, context->Global(), 1, arg);
		arg->Clear(); converter.Clear();
		if (result.IsEmpty()) {
			throw_js_error(isolate, "Script conversion failed...");
			return NULL;
		}
		out_script = new std::string();
		::add_gscript(*out_script);
		native_string rs(isolate, result.ToLocalChecked());
		out_script->append(rs.c_str()); rs.clear();
		return out_script;
	}
	_NEW_STR(out_script);
	::add_gscript(*out_script);
	out_script->append(dest->data(), dest->size());
	_free_obj(dest);
	return out_script;
}
//int load_script(
//	const char* path, std::string& out_script
//) {
//	std::ifstream* file = new std::ifstream(path, std::ifstream::ate | std::ifstream::binary);
//	if (!file->is_open()) {
//		out_script.append("File not found in#");
//		out_script.append(path);
//		return FALSE;
//	}
//	std::vector<char>* dest = new std::vector<char>();
//	int ret = ::load_file_to_vct(*file, *dest);
//	file->close(); delete file;
//	if (ret == TRUE) {
//		::add_gscript(out_script);
//		out_script.append(dest->data(), dest->size());
//	}
//	_free_obj(dest);
//	return ret;
//}
//9:32 PM 11/22/2018
V8_JS_METHOD(require) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "File absolute path required!!!");
		return;
	}
	native_string utf_abs_path_str(isolate, args[0]);
	const char* path_info = utf_abs_path_str.c_str();
	typeof_module ext = ::get_module_type(path_info);
	if (ext == typeof_module::_UNKNOWN) {
		throw_js_error(isolate, "Unsupported module...");
		utf_abs_path_str.clear();
		return;
	}
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	std::string* abs_path = new std::string();
	if (ext == typeof_module::NATIVE) {
		bool is_full_path = false;
		if (args.Length() > 1) {
			if (args[1]->IsBoolean()) {
				is_full_path = ::to_boolean(isolate, args[1]);
			}
		}
		if (is_full_path == false) {
			abs_path->append(wj_env->get_root_dir());
			::get_server_map_path(path_info, *abs_path);
		}
		else {
			::get_server_map_path(path_info, *abs_path);
		}
		require_native(args, abs_path->c_str(), wj_env->get_app_dir(), path_info);
		_free_obj(abs_path); utf_abs_path_str.clear();
		return;
	}
	abs_path->append(wj_env->get_root_dir());
	::get_server_map_path(path_info, *abs_path);
	if (ext == typeof_module::NO_EXT) {
		abs_path->append(".js");
	}
	if (__file_exists(abs_path->c_str()) == false) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Module not found. Module# ", utf_abs_path_str.c_str())));
		_free_obj(abs_path); utf_abs_path_str.clear();
		return;
	}
	v8::Local<v8::Function> converter;
	if (args.Length() > 1 && args[1]->IsFunction()) {
		converter = v8::Local<v8::Function>::Cast(args[1]);
	}
	std::string* source_str = ::load_script(isolate, abs_path->c_str(), converter);
	if (source_str == NULL) {
		_free_obj(abs_path); utf_abs_path_str.clear();
		return;
	}
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);//sow_web_jsx::wrapper::create_v8_context_object(isolate);
	v8::Local<v8::ObjectTemplate> module_object = v8::ObjectTemplate::New(isolate);
	module_object->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8_global->Set(isolate, "module", module_object);
	wjsx_assign_js_func(isolate, v8_global, "require", ::require);
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8_global);
	v8::Context::Scope context_scope(context);
	::swjsx_module::scope_to_js_global(isolate, context, wj_env);
	// Compile the source code.
	std::unique_ptr<v8::ScriptCompiler::CachedData> cache;
	_NEW_STR(c_script_path);
	int compiled_cached = FALSE;
	if (wj_env->compiled_cached == TRUE) {
		c_script_path->append(abs_path->c_str());
		c_script_path->append("c");
		compiled_cached = wj_env->compiled_cached == FALSE ? FALSE : (__file_exists(c_script_path->c_str()) ? TRUE : FALSE);
	}
	if (compiled_cached == TRUE) {
		cache = ::read_script_cached(abs_path->c_str(), c_script_path->c_str(), wj_env->check_file_state);
		if (cache == nullptr) {
			compiled_cached = FALSE;
		}
	}
	_free_obj(abs_path);
	_NEW_STR(origin_str); utf_abs_path_str.clear();
	::get_script_origin(path_info, *origin_str); utf_abs_path_str.clear();
	v8::ScriptOrigin					script_origin(v8_str(isolate, origin_str->c_str()));
	_free_obj(origin_str);
	v8::ScriptCompiler::Source			sources(v8_str(isolate, source_str->c_str()), script_origin, cache.release());
	_free_obj(source_str);
	v8::MaybeLocal<v8::UnboundScript>	unbound_script;
	v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;
	if (compiled_cached == TRUE) {
		options = v8::ScriptCompiler::kConsumeCodeCache;
	}
	unbound_script = v8::ScriptCompiler::CompileUnboundScript(
		isolate, &sources, options
	);
	//v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, v8_str(isolate, source_str.c_str()));
	if (unbound_script.IsEmpty()) {
		isolate->ThrowException(v8::Exception::Error(::concat_msg(isolate, "Unable to compile script. Check your script than try again. Path: ", utf_abs_path_str.c_str())));
		context.Clear();
		v8_global.Clear();
		return;
	}
	// Run the script to get the result.
	v8::TryCatch trycatch(isolate);
	unbound_script.ToLocalChecked()->BindToCurrentContext()->Run(context);
	if (trycatch.HasCaught()) {
		_free_obj(c_script_path);
		_NEW_STR(error_str);
		::set__exception(isolate, &trycatch, *error_str);
		throw_js_error(isolate, error_str->c_str());
		_free_obj(error_str);
		context->DetachGlobal();
		context.Clear(); v8_global.Clear();
		return;
	}
	if (compiled_cached == FALSE) {
		if (wj_env->compiled_cached == TRUE) {
			::create_script_cached_data(unbound_script, c_script_path->c_str());
		}
	}
	_free_obj(c_script_path);
	v8::Local<v8::Object> jsGlobal =
		context->Global()->GetPrototype().As<v8::Object>();
	v8::Local<v8::Object> modules = v8::Handle<v8::Object>::Cast(jsGlobal->Get(context, v8_str(isolate, "module")).ToLocalChecked());
	args.GetReturnValue().Set(modules->Get(context, v8_str(isolate, "exports")).ToLocalChecked());
	modules.Clear(); jsGlobal.Clear(); context.Clear(); v8_global.Clear();
	return;
}

v8::Local<v8::ObjectTemplate> sow_web_jsx::wrapper::get_context(
	v8::Isolate* isolate, 
	const std::map<std::string, std::map<std::string, std::string>> ctx
) {
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (const auto itr : ctx) {
		auto key = itr.first;
		auto obj = itr.second;
		if (key == "global") {
			for (const auto gitr : obj) {
				if (gitr.first == "https") {
					ctx_object->Set(v8_str(isolate, (gitr.first).c_str()), v8::Boolean::New(isolate, gitr.second == "on" ? true : false), v8::PropertyAttribute::ReadOnly);
					continue;
				}
				ctx_object->Set(v8_str(isolate, (gitr.first).c_str()), v8_str(isolate, gitr.second.c_str()), v8::PropertyAttribute::ReadOnly);
			}
			continue;
		}
		v8::Local<v8::ObjectTemplate> object = v8::ObjectTemplate::New(isolate);
		if (key == "request") {
			for (const auto oitr : obj) {
				if (oitr.first == "header" || oitr.first == "query_string" || oitr.first == "content_length") {
					object->Set(v8_str(isolate, (oitr.first).c_str()), v8_str(isolate, oitr.second.c_str()), v8::PropertyAttribute::DontDelete);
					continue;
				}
				object->Set(v8_str(isolate, (oitr.first).c_str()), v8_str(isolate, oitr.second.c_str()), v8::PropertyAttribute::ReadOnly);
			}
		}
		else {
			for (const auto oitr : obj) {
				object->Set(v8_str(isolate, (oitr.first).c_str()), v8_str(isolate, oitr.second.c_str()), v8::PropertyAttribute::ReadOnly);
			}
		}
		wjsx_assign_js_obj(isolate, ctx_object, key.c_str(), object);
	}
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	wjsx_assign_js_obj(isolate, v8_global, "context", ctx_object);
	wjsx_assign_js_func(isolate, v8_global, "require", ::require);
	return v8_global;
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
V8_JS_METHOD(console_print) {
	bool first = true;
	v8::Isolate* isolate = args.GetIsolate();
	for (int i = 0, l = args.Length(); i < l; i++) {
		if (not first)
			std::cout << " ";
		else
			first = false;
		const char* cstr = ::to_char_str(isolate, args[i]);
		std::cout << cstr;
	}
	std::cout << std::flush;
}
V8_JS_METHOD(console_read_line) {
	_NEW_STR(input);
	std::istream& in = std::getline(std::cin, *input);
	if (::stream_is_available(in) == FALSE) {
		throw_js_error(args.GetIsolate(), "Input stream not available...");
		return;
	}
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	args.GetReturnValue().Set(v8_str(isolate, input->c_str()));
	_free_obj(input);
}

//V8_JS_METHOD(console_read_line) {
//	//Before Callback
//	std::cout << "Enter chracter:" << std::flush;
//	std::string s;
//	while (not std::getline(std::cin, s).eof()) {
//		//After Callback
//		std::cout << "the chracter was: " << s << std::endl;
//		//Before Callback
//		std::cout << "Enter chracter:" << std::flush;
//	}
//}
v8::Local<v8::ObjectTemplate> wrapper::get_console_context(
	v8::Isolate* isolate, 
	const std::map<std::string, std::string> ctx
) {
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (const auto itr: ctx) {
		if (itr.first == "is_interactive") {
			bool is_interactive = itr.second == "1";
			ctx_object->Set(v8_str(isolate, "is_interactive"), v8::Boolean::New(isolate, is_interactive), v8::PropertyAttribute::ReadOnly);
			continue;
		}
		ctx_object->Set(v8_str(isolate, itr.first.c_str()), v8_str(isolate, itr.second.c_str()), v8::PropertyAttribute::ReadOnly);
	}
	v8_global->Set(v8_str(isolate, "env"), ctx_object, v8::PropertyAttribute::ReadOnly);
	wjsx_assign_js_func(isolate, v8_global, "__clear", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		
	});
	wjsx_assign_js_func(isolate, v8_global, "stderr",[](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			fprintf_s(_stdout, "%s", "undefined");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "Formeted string required!!!");
			return;
		}
		native_string utf_msg_str(isolate, args[0]);
		fprintf_s(_stderr, "%s", utf_msg_str.c_str());
		utf_msg_str.clear();
		return;
	});
	wjsx_assign_js_func(isolate, v8_global, "stdout", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 0) {
			throw_js_error(isolate, "Argument required!!!");
			return;
		}
		if (args[0]->IsNullOrUndefined()) {
			fprintf_s(_stdout, "%s", sow_web_jsx::to_char_str(isolate, args[0]));
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "Formeted string required!!!");
			return;
		}
		native_string utf_msg_str(isolate, args[0]);
		fprintf_s(_stdout, "%s", utf_msg_str.c_str());
		utf_msg_str.clear();
		return;
	});
	//console_read_line
	wjsx_assign_js_func(isolate, v8_global, "read_line", ::console_read_line);
	wjsx_assign_js_func(isolate, v8_global, "print", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		int argl = args.Length();
		if (argl == 0) {
			throw_js_error(isolate, "Argument Required!!!");
			return;
		}
		if (argl == 1) {
			if (args[0]->IsNullOrUndefined()) {
				std::cout << ::to_char_str(isolate, args[0]) << std::endl;
				return;
			}
			if (args[0]->IsString()) {
				native_string utf_msg_str(isolate, args[0]);
				std::cout << utf_msg_str.c_str();// << "\n";
				utf_msg_str.clear();
				return;
			}
		}
		console_print(args);
	});
	wjsx_assign_js_func(isolate, v8_global, "require", ::require);
	return v8_global;
}
//9:32 PM 11/22/2018
void ::wrapper::response_body_flush(wjsx_env&wj_env, bool end_req) {
	if (wj_env.is_flush() == TRUE)return;
	if (end_req == true || wj_env.is_available_out_stream() == FALSE ) {
		//We defined here force close request or client not connected
		__clear_cache(wj_env); //fflush(stdout);
		return;
	}
	if (n_help::is_gzip_encoding(wj_env.get_http_header()) == TRUE) {
		wj_env.set_flush();
		gzip_compress_write(wj_env);
		return;
	}
	if (write_http_status(wj_env) == FALSE)return;
	n_help::write_http_status(wj_env, false);
	//if (_set_binary_output() == FALSE)return;
	n_help::write_header(wj_env);
	n_help::write_cookies(wj_env);
	__clear_cache(wj_env);
	wj_env << "\r\n";
	wj_env.flush();
	return;
}
void ::wrapper::clear_cache(wjsx_env& wj_env) {
	__clear_cache(wj_env);
}
void ::wrapper::add_header(wjsx_env* wj_env, const char* key, const char* value) {
	n_help::add_header(wj_env->get_http_header(), key, value);
}
int ::wrapper::is_http_status_ok(wjsx_env* wj_env) {
	return n_help::write_http_status(*wj_env, true) < 0 ? FALSE : TRUE;
}
int ::wrapper::is_gzip_encoding(wjsx_env* wj_env) {
	return n_help::is_gzip_encoding(wj_env->get_http_header());
}
int ::wrapper::flush_http_status(wjsx_env* wj_env) {
	return write_http_status(*wj_env);
}
void ::wrapper::flush_header(wjsx_env* wj_env) {
	n_help::write_header(*wj_env);
}
void ::wrapper::flush_cookies(wjsx_env* wj_env) {
	n_help::write_cookies(*wj_env);
}
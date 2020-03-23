/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:14 PM 2/12/2020
#	include "zlib_w.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include <web_jsx/wjsx_env.h>
#	include "zgzip.h"
using namespace sow_web_jsx;
void gzip_compressor_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	v8::Handle<v8::Object> gzip_object = v8::Object::New(isolate);
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	if (::unwrap_wjsx_env(isolate)->is_cli() == FALSE) {
		//[gzip_deflate]
		v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args.IsConstructCall()) {
				throw_js_error(isolate, "Cannot call constructor as function!!!");
				return;
			}
			gzip::gzip_deflate* deflate = new gzip::gzip_deflate(Z_BEST_SPEED);
			if (deflate->has_error() == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				delete deflate;
				return;
			}
			v8::Local<v8::Object> obj = args.This();
			obj->SetInternalField(0, v8::External::New(isolate, deflate));
			v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
			pobj.SetWeak<gzip::gzip_deflate*>(&deflate, [](const v8::WeakCallbackInfo<gzip::gzip_deflate*>& data) {
				delete[] data.GetParameter();
			}, v8::WeakCallbackType::kParameter);
		});
		tpl->SetClassName(v8_str(isolate, "compress"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);
		v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
		//[Out Stream std::cout]
		set_prototype(isolate, prototype, "flush_header", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_available_out_stream() == FALSE)return;
			if (wj_env->is_flush() == TRUE) {
				throw_js_error(isolate, "Headers already been flushed...");
				return;
			}
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			if (deflate->has_error() == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				delete deflate;
				return;
			}
			if (::wrapper::flush_http_status(wj_env) == FALSE)return;
			if (::wrapper::is_gzip_encoding(wj_env) == FALSE) {
				::wrapper::add_header(wj_env, "Content-Encoding", "gzip");
			}
			wj_env->set_flush();
			::wrapper::flush_header(wj_env);
			::wrapper::flush_cookies(wj_env);
			_WCOUT << "\r\n";
			::wrapper::clear_cache(*wj_env);
			_WFLUSH();
			deflate->write_header(_WCOUT);
		});
		set_prototype(isolate, prototype, "write", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_available_out_stream() == FALSE)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			int do_flush = Z_NO_FLUSH;
			//Z_FINISH : Z_NO_FLUSH
			if (args.Length() > 1) {
				if (args[1]->IsNumber()) {
					v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
					do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
					if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
						throw_js_error(isolate, "deflate::Invalid stream end request.");
						return;
					}
				}
			}
			native_string str(isolate, args[0]);
			//const char* data = str.c_str();
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			size_t ret = 0; size_t len = str.size();
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(str.c_str(), len);
				ret = deflate->write(_WCOUT, source_stream, do_flush, FALSE);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = deflate->write(_WCOUT, str.c_str(), do_flush);
			}
			str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, prototype, "write_from_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_available_out_stream() == FALSE)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			int do_flush = Z_NO_FLUSH;
			//Z_FINISH : Z_NO_FLUSH
			if (args.Length() > 1) {
				if (args[1]->IsNumber()) {
					v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
					do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
					if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
						throw_js_error(isolate, "deflate::Invalid stream end request.");
						return;
					}
				}
			}
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			size_t ret = deflate->write_file(_WCOUT, abs_path->c_str(), do_flush);
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, prototype, "flush", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_available_out_stream() == FALSE)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			if (deflate == NULL) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			int ret = deflate->flush(std::cout);
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (ret < 0) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			deflate->f_close_file();
			deflate->write_footer(_WCOUT);
			delete deflate;
			_WFLUSH();
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			sow_web_jsx::wrapper::clear_cache(*wj_env);
		});
		//[/Out Stream std::cout]
		//[File Write]
		set_prototype(isolate, prototype, "fs_open", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			size_t ret = deflate->f_open_file(abs_path->c_str());
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, prototype, "fs_write_gzip_header", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			deflate->f_write_header();
		});
		set_prototype(isolate, prototype, "fs_write_from_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			int do_flush = Z_NO_FLUSH;
			//Z_FINISH : Z_NO_FLUSH
			if (args.Length() > 1) {
				if (args[1]->IsNumber()) {
					v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
					do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
					if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
						throw_js_error(isolate, "deflate::Invalid stream end request.");
						return;
					}
				}
			}
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			size_t ret = deflate->f_write_file(abs_path->c_str(), do_flush);
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, prototype, "fs_write", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			int do_flush = Z_NO_FLUSH;
			//Z_FINISH : Z_NO_FLUSH
			if (args.Length() > 1) {
				if (args[1]->IsNumber()) {
					v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
					do_flush = args[1]->Int32Value(ctx).FromMaybe(0);
					if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
						throw_js_error(isolate, "deflate::Invalid stream end request.");
						return;
					}
				}
			}

			native_string str(isolate, args[0]);
			const char* data = str.c_str();
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			size_t ret = 0; size_t len = strlen(data);
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(data, len);
				ret = deflate->f_write(source_stream, do_flush);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = deflate->f_write(data, do_flush);
			}
			str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, prototype, "fs_flush", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			int ret = deflate->f_flush();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (ret < 0) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}
		});
		set_prototype(isolate, prototype, "fs_write_gzip_footer", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			deflate->f_write_footer();
		});
		set_prototype(isolate, prototype, "fs_close_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			deflate->f_close_file();
		});
		//[/File Write]
		set_prototype(isolate, prototype, "release", [](js_method_args) {
			::gzip::gzip_deflate* deflate = ::unwrap<gzip::gzip_deflate>(args);
			if (deflate != NULL) {
				delete deflate; deflate = NULL;
				args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			}
		});
		//[/gzip_deflate]
		//[gzip_inflate]
		v8::Local<v8::FunctionTemplate> inflate_tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args.IsConstructCall()) {
				throw_js_error(isolate, "Cannot call constructor as function!!!");
				return;
			}
			if (args.Length() <= 0) {
				throw_js_error(isolate, "window bits required!!!");
				return;
			}
			if (!args[0]->IsNumber()) {
				throw_js_error(isolate, "window bits should be number!!!");
				return;
			}
			v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
			int window_bits = args[0]->Int32Value(ctx).FromMaybe(0);
			gzip::gzip_inflate* inflate = new gzip::gzip_inflate(window_bits);
			//if (!sow_web_jsx::wrapper::set_binary_mode_in()) {/*[Nothing to do when failed...]*/ }
			if (inflate->has_error() == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				delete inflate;
				return;
			}
			v8::Local<v8::Object> obj = args.This();
			obj->SetInternalField(0, v8::External::New(isolate, inflate));
			v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
			pobj.SetWeak<gzip::gzip_inflate*>(&inflate, [](const v8::WeakCallbackInfo<gzip::gzip_inflate*>& data) {
				delete[] data.GetParameter();
			}, v8::WeakCallbackType::kParameter);
		});
		inflate_tpl->SetClassName(v8_str(isolate, "decompress"));
		inflate_tpl->InstanceTemplate()->SetInternalFieldCount(1);
		v8::Local<v8::ObjectTemplate> inflate_prototype = inflate_tpl->PrototypeTemplate();
		//[Out Stream std::cout]
		set_prototype(isolate, inflate_prototype, "flush_header", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_flush() == TRUE) {
				throw_js_error(isolate, "Headers already been flushed...");
				return;
			}
			::gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			if (inflate->has_error() == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				delete inflate;
				return;
			}
			if (::wrapper::flush_http_status(wj_env) == FALSE)return;
			wj_env->set_flush();
			if (::wrapper::is_gzip_encoding(wj_env) == FALSE) {
				::wrapper::add_header(wj_env, "Content-Encoding", "gzip");
			}
			::wrapper::flush_header(wj_env);
			::wrapper::flush_cookies(wj_env);
			_WCOUT << "\r\n";
			::wrapper::clear_cache(*wj_env);
			_WFLUSH();
		});
		set_prototype(isolate, inflate_prototype, "write", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			native_string str(isolate, args[0]);
			const char* data = str.c_str();
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			size_t ret = 0; size_t len = strlen(data);
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(data, len);
				ret = inflate->write(_WCOUT, source_stream, FALSE);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = inflate->write(_WCOUT, data);
			}
			str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, inflate_prototype, "write_from_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			size_t ret = inflate->write_file(_WCOUT, abs_path->c_str());
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, inflate_prototype, "flush", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			if (wj_env == NULL)return;
			if (wj_env->is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			if (inflate == NULL) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			int ret = inflate->flush(_WCOUT);
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (ret < 0) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			inflate->f_close_file();
			delete inflate;
			_WFLUSH();
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			::wrapper::clear_cache(*wj_env);
		});
		//[/Out Stream std::cout]
		//[File Write]
		set_prototype(isolate, inflate_prototype, "fs_open", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			size_t ret = inflate->f_open_file(abs_path->c_str());
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, inflate_prototype, "fs_write_from_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			size_t ret = inflate->f_write_file(abs_path->c_str());
			_free_obj(abs_path); utf_abs_path_str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, inflate_prototype, "fs_write", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			native_string str(isolate, args[0]);
			const char* data = str.c_str();
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			size_t ret = 0; size_t len = strlen(data);
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(data, len);
				ret = inflate->f_write(source_stream);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = inflate->f_write(data);
			}
			str.clear();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (is_error_code(ret) == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}
			args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
		});
		set_prototype(isolate, inflate_prototype, "fs_flush", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			int ret = inflate->f_flush();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (ret < 0) {
				throw_js_error(isolate, inflate->get_last_error());
				return;
			}

		});
		set_prototype(isolate, inflate_prototype, "fs_close_file", [](js_method_args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			inflate->f_close_file();
		});
		//[/File Write]
		set_prototype(isolate, inflate_prototype, "release", [](js_method_args) {
			gzip::gzip_inflate* inflate = ::unwrap<gzip::gzip_inflate>(args);
			if (inflate != NULL) {
				delete inflate; inflate = NULL;
				args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			}
		});
		//[/gzip_inflate]
		gzip_object->Set(context, v8_str(isolate, "compress"), tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
		gzip_object->Set(context, v8_str(isolate, "decompress"), inflate_tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
	}
	wjsx_set_method(isolate, gzip_object, "inflate", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (args.Length() <= 1) {
			throw_js_error(isolate, "Input file and output file path required....");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "Input file path required....");
			return;
		}
		if (!args[1]->IsString()) {
			throw_js_error(isolate, "Output file path required....");
			return;
		}
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		const char* root_dir = wj_env->get_root_dir();
		native_string utf_input_path_str(isolate, args[0]);
		std::string* input_path = new std::string(root_dir);
		::get_server_map_path(utf_input_path_str.c_str(), *input_path);

		native_string utf_output_path_str(isolate, args[1]);
		std::string* output_path = new std::string(root_dir);
		::get_server_map_path(utf_output_path_str.c_str(), *output_path);
		std::string error;
		int ret = gzip::inflate_file(input_path->c_str(), output_path->c_str(), error);
		_free_obj(input_path); _free_obj(output_path);
		utf_input_path_str.clear(); utf_output_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, error.c_str());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	});
	wjsx_set_method(isolate, gzip_object, "deflate", [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (args.Length() <= 1) {
			throw_js_error(args.GetIsolate(), "Input file and output file path required....");
			return;
		}
		if (!args[0]->IsString()) {
			throw_js_error(args.GetIsolate(), "Input file path required....");
			return;
		}
		if (!args[1]->IsString()) {
			throw_js_error(args.GetIsolate(), "Output file path required....");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		int comp_level = Z_BEST_SPEED;
		//Z_FINISH : Z_NO_FLUSH
		if (args.Length() > 2) {
			if (args[2]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				comp_level = args[2]->Int32Value(ctx).FromMaybe(0);
				if (!(comp_level == Z_NO_COMPRESSION ||
					comp_level == Z_BEST_SPEED ||
					comp_level == Z_BEST_COMPRESSION ||
					comp_level == Z_DEFAULT_COMPRESSION)) {
					throw_js_error(isolate, "deflate::Invalid compression level defined.");
					return;
				}
			}
		}
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		const char* root_dir = wj_env->get_root_dir();
		native_string utf_input_path_str(isolate, args[0]);
		std::string* input_path = new std::string(root_dir);
		::get_server_map_path(utf_input_path_str.c_str(), *input_path);

		native_string utf_output_path_str(isolate, args[1]);
		std::string* output_path = new std::string(root_dir);
		::get_server_map_path(utf_output_path_str.c_str(), *output_path);
		std::string error;
		int ret = gzip::deflate_file(input_path->c_str(), output_path->c_str(), comp_level, error);
		_free_obj(input_path); _free_obj(output_path);
		utf_input_path_str.clear(); utf_output_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, error.c_str());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	});
	gzip_object->Set(context, v8_str(isolate, "Z_NO_FLUSH"), v8::Integer::New(isolate, Z_NO_FLUSH));
	gzip_object->Set(context, v8_str(isolate, "Z_FINISH"), v8::Integer::New(isolate, Z_FINISH));
	gzip_object->Set(context, v8_str(isolate, "Z_NO_COMPRESSION"), v8::Integer::New(isolate, Z_NO_COMPRESSION));
	gzip_object->Set(context, v8_str(isolate, "Z_BEST_SPEED"), v8::Integer::New(isolate, Z_BEST_SPEED));
	gzip_object->Set(context, v8_str(isolate, "Z_BEST_COMPRESSION"), v8::Integer::New(isolate, Z_BEST_COMPRESSION));
	gzip_object->Set(context, v8_str(isolate, "Z_DEFAULT_COMPRESSION"), v8::Integer::New(isolate, Z_DEFAULT_COMPRESSION));
	gzip_object->Set(context, v8_str(isolate, "MAX_WBITS"), v8::Integer::New(isolate, MAX_WBITS));
	gzip_object->Set(context, v8_str(isolate, "ZLIB_VERSION"), v8_str(isolate, ZLIB_VERSION));
	wjsx_set_object(isolate, target, "zlib", gzip_object);
}

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
#	include "zgzip.h"
using namespace sow_web_jsx;
void gzip_compressor_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	v8::Handle<v8::Object> gzip_object = v8::Object::New(isolate);
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	if (sow_web_jsx::wrapper::is_cli() == FALSE) {
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
		prototype->Set(isolate, "flush_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush()==TRUE) {
				throw_js_error(isolate, "Headers already been flushed...");
				return;
			}
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			if (deflate->has_error() == TRUE) {
				throw_js_error(isolate, deflate->get_last_error());
				delete deflate;
				return;
			}
			if (sow_web_jsx::wrapper::flush_http_status() == FALSE)return;
			if (sow_web_jsx::wrapper::set_binary_output() == FALSE)return;
			if (sow_web_jsx::wrapper::is_gzip_encoding() == FALSE) {
				sow_web_jsx::wrapper::add_header("Content-Encoding", "gzip");
			}
			sow_web_jsx::wrapper::set_flush_status(TRUE);
			sow_web_jsx::wrapper::flush_header();
			sow_web_jsx::wrapper::flush_cookies();
			std::cout << "\r\n";
			sow_web_jsx::wrapper::clear_cache(TRUE, FALSE);
			fflush(stdout);
			deflate->write_header(std::cout);
		}));
		prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
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
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			size_t ret = 0; size_t len = str.size();
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(str.c_str(), len);
				ret = deflate->write(std::cout, source_stream, do_flush, FALSE);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = deflate->write(std::cout, str.c_str(), do_flush);
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
		}));
		prototype->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
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
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			size_t ret = deflate->write_file(std::cout, abs_path->c_str(), do_flush);
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
		}));
		prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
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
			deflate->write_footer(std::cout);
			delete deflate;
			fflush(stdout);
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			sow_web_jsx::wrapper::clear_cache(TRUE, TRUE);
		}));
		//[/Out Stream std::cout]
		//[File Write]
		prototype->Set(isolate, "fs_open", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
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
		}));
		prototype->Set(isolate, "fs_write_gzip_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			deflate->f_write_header();
		}));
		prototype->Set(isolate, "fs_write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
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
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
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
		}));
		prototype->Set(isolate, "fs_write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
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
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
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
		}));
		prototype->Set(isolate, "fs_flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			int ret = deflate->f_flush();
			if (ret == FALSE) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			if (ret < 0) {
				throw_js_error(isolate, deflate->get_last_error());
				return;
			}

		}));
		prototype->Set(isolate, "fs_write_gzip_footer", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			deflate->f_write_footer();
		}));
		prototype->Set(isolate, "fs_close_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			deflate->f_close_file();
		}));
		//[/File Write]
		prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			gzip::gzip_deflate* deflate = sow_web_jsx::unwrap<gzip::gzip_deflate>(args);
			if (deflate != NULL) {
				delete deflate; deflate = NULL;
				args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			}
		}));
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
			if (!sow_web_jsx::wrapper::set_binary_mode_in()) {/*[Nothing to do when failed...]*/ }
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
		inflate_prototype->Set(isolate, "flush_header", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == TRUE) {
				throw_js_error(isolate, "Headers already been flushed...");
				return;
			}
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			if (inflate->has_error() == TRUE) {
				throw_js_error(isolate, inflate->get_last_error());
				delete inflate;
				return;
			}
			if (sow_web_jsx::wrapper::flush_http_status() == FALSE)return;
			if (sow_web_jsx::wrapper::set_binary_output() == FALSE)return;
			sow_web_jsx::wrapper::set_flush_status(TRUE);
			if (sow_web_jsx::wrapper::is_gzip_encoding() == FALSE) {
				sow_web_jsx::wrapper::add_header("Content-Encoding", "gzip");
			}
			sow_web_jsx::wrapper::flush_header();
			sow_web_jsx::wrapper::flush_cookies();
			std::cout << "\r\n";
			sow_web_jsx::wrapper::clear_cache(TRUE, FALSE);
			sow_web_jsx::wrapper::set_flush_status(TRUE);
			fflush(stdout);
		}));
		inflate_prototype->Set(isolate, "write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			native_string str(isolate, args[0]);
			const char* data = str.c_str();
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			size_t ret = 0; size_t len = strlen(data);
			if (len > CHUNK) {
				std::stringstream source_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
				source_stream.write(data, len);
				ret = inflate->write(std::cout, source_stream, FALSE);
				source_stream.clear(); std::stringstream().swap(source_stream);
			}
			else {
				ret = inflate->write(std::cout, data);
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
		}));
		inflate_prototype->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			size_t ret = inflate->write_file(std::cout, abs_path->c_str());
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
		}));
		inflate_prototype->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (sow_web_jsx::wrapper::is_flush() == FALSE) {
				throw_js_error(isolate, "Headers did not flushed yet...");
				return;
			}
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			if (inflate == NULL) {
				throw_js_error(isolate, "Stream already flashed...");
				return;
			}
			int ret = inflate->flush(std::cout);
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
			fflush(stdout);
			args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			sow_web_jsx::wrapper::clear_cache(TRUE, TRUE);
		}));
		//[/Out Stream std::cout]
		//[File Write]
		inflate_prototype->Set(isolate, "fs_open", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
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
		}));
		inflate_prototype->Set(isolate, "fs_write_from_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "File Path Required required....");
				return;
			}
			native_string utf_abs_path_str(isolate, args[0]);
			std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
			sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
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
		}));
		inflate_prototype->Set(isolate, "fs_write", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			if (!args[0]->IsString()) {
				throw_js_error(isolate, "data required....");
				return;
			}
			native_string str(isolate, args[0]);
			const char* data = str.c_str();
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
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
		}));
		inflate_prototype->Set(isolate, "fs_flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
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

		}));
		inflate_prototype->Set(isolate, "fs_close_file", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			v8::Isolate* isolate = args.GetIsolate();
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			inflate->f_close_file();
		}));
		//[/File Write]
		inflate_prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
			gzip::gzip_inflate* inflate = sow_web_jsx::unwrap<gzip::gzip_inflate>(args);
			if (inflate != NULL) {
				delete inflate; inflate = NULL;
				args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
			}
		}));
		//[/gzip_inflate]
		gzip_object->Set(context, v8_str(isolate, "compress"), tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
		gzip_object->Set(context, v8_str(isolate, "decompress"), inflate_tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
	}
	gzip_object->Set(context, v8_str(isolate, "inflate"), v8::Function::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
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
		native_string utf_input_path_str(isolate, args[0]);
		std::string* input_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf_input_path_str.c_str(), *input_path);

		native_string utf_output_path_str(isolate, args[1]);
		std::string* output_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf_output_path_str.c_str(), *output_path);
		std::string error;
		int ret = gzip::inflate_file(input_path->c_str(), output_path->c_str(), error);
		_free_obj(input_path); _free_obj(output_path);
		utf_input_path_str.clear(); utf_output_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, error.c_str());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
	gzip_object->Set(context, v8_str(isolate, "deflate"), v8::Function::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
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
		native_string utf_input_path_str(isolate, args[0]);
		std::string* input_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf_input_path_str.c_str(), *input_path);

		native_string utf_output_path_str(isolate, args[1]);
		std::string* output_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf_output_path_str.c_str(), *output_path);
		std::string error;
		int ret = gzip::deflate_file(input_path->c_str(), output_path->c_str(), comp_level, error);
		_free_obj(input_path); _free_obj(output_path);
		utf_input_path_str.clear(); utf_output_path_str.clear();
		if (ret == FALSE) {
			throw_js_error(isolate, error.c_str());
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, (double)ret));
	}));
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

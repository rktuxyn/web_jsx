/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:52 PM 4/8/2020
#	include "curl_download_wrapper.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include "download_request.h"
using namespace sow_web_jsx;
V8_JS_METHOD(http_download_request) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		throw_js_type_error(isolate, "Object required!!!");
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
	v8::Local<v8::Value> v8_url_str = config->Get(ctx, v8_str(isolate, "url")).ToLocalChecked();
	if (v8_url_str->IsNullOrUndefined() || !v8_url_str->IsString()) {
		throw_js_type_error(isolate, "Url should be string!!!");
		return;
	}
	v8::Local<v8::Value> v8_out_file = config->Get(ctx, v8_str(isolate, "output")).ToLocalChecked();
	if (v8_out_file->IsNullOrUndefined() || !v8_out_file->IsString()) {
		throw_js_type_error(isolate, "Output file path should be string!!!");
		return;
	}
	native_string utf_url_str(isolate, v8_url_str);
	http_client::http_download* http = new http_client::http_download(utf_url_str.c_str());
	v8::Local<v8::Value>v8_header_str = config->Get(ctx, v8_str(isolate, "header")).ToLocalChecked();
	if (!v8_header_str->IsNullOrUndefined() && v8_header_str->IsArray()) {
		v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_header_str);
		for (uint32_t i = 0, l = harr->Length(); i < l; i++) {
			v8::Local<v8::Value>v8_val = harr->Get(ctx, i).ToLocalChecked();
			if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
			v8::String::Utf8Value key(isolate, v8_val);
			http->set_header(*key);
		}
		v8_header_str.Clear();
	}
	v8::Local<v8::Value>v8_cookie_str = config->Get(ctx, v8_str(isolate, "cookie")).ToLocalChecked();
	if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsString()) {
		v8::String::Utf8Value utf_cook_str(isolate, v8_cookie_str);
		http->set_cookie(*utf_cook_str);
	}
	else if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsArray()) {
		v8::Local<v8::Array> carr = v8::Local<v8::Array>::Cast(v8_cookie_str);
		std::string* cookies = new std::string();
		for (uint32_t i = 0, l = carr->Length(); i < l; i++) {
			v8::Local<v8::Value>v8_val = carr->Get(ctx, i).ToLocalChecked();
			if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
			v8::String::Utf8Value key(isolate, v8_val);
			if (i == 0) {
				cookies->append(*key); continue;
			}
			cookies->append("; ");
			cookies->append(*key);
		}
		http->set_cookie(cookies->c_str());
		cookies->clear(); delete cookies;
		v8_cookie_str.Clear();
	}
	wjsx_env* env = ::unwrap_wjsx_env(isolate);
	int show_progress = FALSE;
	if (env->is_cli() == FALSE) {
		http->read_debug_information(false);
	}
	else {
		v8::Local<v8::Value> v8_isDebug = config->Get(ctx, v8_str(isolate, "is_debug")).ToLocalChecked();
		if (v8_isDebug->IsBoolean()) {
			http->read_debug_information(::to_boolean(isolate, v8_isDebug));
		}
		else {
			http->read_debug_information(true);
		}
		v8::Local<v8::Value> v8_show_progress = config->Get(ctx, v8_str(isolate, "show_progress")).ToLocalChecked();
		if (v8_show_progress->IsBoolean()) {
			show_progress = ::to_boolean(isolate, v8_show_progress) == true ? TRUE : FALSE;
		}
	}
	v8::Local<v8::Value> v8_v_ssl = config->Get(ctx, v8_str(isolate, "is_verify_ssl")).ToLocalChecked();
	if (v8_v_ssl->IsBoolean()) {
		http->verify_ssl(sow_web_jsx::to_boolean(isolate, v8_v_ssl));
	}
	else {
		http->verify_ssl(true);
	}
	v8::Local<v8::Value> v8_v_ssl_host = config->Get(ctx, v8_str(isolate, "is_verify_ssl_host")).ToLocalChecked();
	if (v8_v_ssl_host->IsBoolean()) {
		http->verify_ssl_host(sow_web_jsx::to_boolean(isolate, v8_v_ssl_host));
	}
	else {
		http->verify_ssl_host(true);
	}
	env->get_root_dir();
	native_string utf_output_str(isolate, v8_out_file);
	_NEW_STRA(output_path, env->get_root_dir());
	::get_server_map_path(utf_output_str.c_str(), *output_path);
	int rec = http->download(output_path->c_str(), show_progress);
	_free_obj(output_path);
	config.Clear();
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (rec < 0) {
		v8_result->Set(
			ctx,
			v8_str(isolate, "ret_val"),
			v8::Number::New(isolate, rec)
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "ret_msg"),
			v8_str(isolate, http->get_last_error())
		);
	}
	else {
		v8_result->Set(
			ctx,
			v8_str(isolate, "ret_val"),
			v8::Number::New(isolate, rec)
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "ret_msg"),
			v8_str(isolate, "success")
		);
	}
	args.GetReturnValue().Set(v8_result); delete http;
	v8_result.Clear(); utf_url_str.clear();
	return;
}
void http_download_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	wjsx_set_method(isolate, target, "create_http_download_request", ::http_download_request);
}

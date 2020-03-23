/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//11:39 PM 2/12/2020
#	include "curl_http_wrapper.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include "http_request.h"
using namespace sow_web_jsx;
V8_JS_METHOD(http_request) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Object required!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
	v8::Local<v8::Value> v8_url_str = config->Get(ctx, v8_str(isolate, "url")).ToLocalChecked();
	if (v8_url_str->IsNullOrUndefined() || !v8_url_str->IsString()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Url should be string!!!")));
		return;
	}
	v8::Local<v8::Value> v8_method_str = config->Get(ctx, v8_str(isolate, "method")).ToLocalChecked();
	if (v8_method_str->IsNullOrUndefined() || !v8_method_str->IsString()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Method should be string!!!")));
		return;
	}
	v8::String::Utf8Value utf_method_str(isolate, v8_method_str);
	std::string method(*utf_method_str);
	v8::Local<v8::Value>v8_payload_val = config->Get(ctx, v8_str(isolate, "body")).ToLocalChecked();
	if (method == "POST") {
		if (v8_payload_val->IsNullOrUndefined() || !v8_payload_val->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Payload required for POST Request!!!")));
			return;
		}
	}
	v8::String::Utf8Value utf_url_str(isolate, v8_url_str);
	v8::Local<v8::Value> v8_follow_location = config->Get(ctx, v8_str(isolate, "follow_location")).ToLocalChecked();
	bool follow_location = true;
	if (v8_follow_location->IsBoolean()) {
		follow_location = sow_web_jsx::to_boolean(isolate, v8_follow_location);
	}
	http_client::http_request* http = new http_client::http_request(*utf_url_str, follow_location);
	v8_url_str.Clear();
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
	if (::unwrap_wjsx_env(isolate)->is_cli() == FALSE) {
		http->read_debug_information(false);
	}
	else {
		v8::Local<v8::Value> v8_isDebug = config->Get(ctx, v8_str(isolate, "is_debug")).ToLocalChecked();
		if (v8_isDebug->IsBoolean()) {
			http->read_debug_information(sow_web_jsx::to_boolean(isolate, v8_isDebug));
		}
		else {
			http->read_debug_information(true);
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
	std::string* resp_header = new std::string();
	std::string* resp_body = new std::string();
	int rec = 0;
	if (method == "POST") {
		native_string payload_str(isolate, v8_payload_val);
		rec = http->post(payload_str.c_str(), *resp_header, *resp_body);
		v8_payload_val.Clear(); payload_str.clear();
	}
	else {
		rec = http->get(*resp_header, *resp_body);
	}
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
		v8_result->Set(
			ctx,
			v8_str(isolate, "response_header"),
			v8_str(isolate, resp_header->c_str())
		);
		v8_result->Set(
			ctx,
			v8_str(isolate, "response_body"),
			v8_str(isolate, resp_body->c_str())
		);
	}
	args.GetReturnValue().Set(v8_result);
	delete http; _free_obj(resp_header); _free_obj(resp_body);
	v8_result.Clear();
	return;
}
void http_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	wjsx_set_method(isolate, target, "create_http_request", http_request);
}

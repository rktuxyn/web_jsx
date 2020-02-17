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
#	include "smtp_client.h"
#	include "curl_smtp_wrapper.h"
using namespace sow_web_jsx;

void smtp_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Object required!!!")));
		return;
	}
	smtp_client::smtp_request* smtp = new smtp_client::smtp_request();
	if (smtp->has_error()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, smtp->get_last_error())));
		delete smtp;
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
	v8::Local<v8::Value> v8_val;
	if (sow_web_jsx::wrapper::is_cli() == FALSE) {
		smtp->read_debug_information(false);
	}
	else {
		v8_val = config->Get(ctx, v8_str(isolate, "is_debug")).ToLocalChecked();
		if (v8_val->IsBoolean()) {
			smtp->read_debug_information(sow_web_jsx::to_boolean(isolate, v8_val));
		}
		else {
			smtp->read_debug_information(true);
		}
	}
	v8::Local<v8::Value> smtp_user = config->Get(ctx, v8_str(isolate, "user")).ToLocalChecked();
	if (smtp_user->IsNullOrUndefined() || !smtp_user->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP User required...")));
		delete smtp;
		return;
	}
	v8::Local<v8::Value> smtp_pwd = config->Get(ctx, v8_str(isolate, "password")).ToLocalChecked();
	if (smtp_pwd->IsNullOrUndefined() || !smtp_pwd->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP password required...")));
		delete smtp;
		return;
	}
	smtp->credentials(
		to_char_str(isolate, smtp_user),
		to_char_str(isolate, smtp_pwd)
	);
	v8_val = config->Get(ctx, v8_str(isolate, "host")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP Host required...")));
		delete smtp;
		return;
	}
	smtp->host(to_char_str(isolate, v8_val));
	v8_val = config->Get(ctx, v8_str(isolate, "tls")).ToLocalChecked();
	if (v8_val->IsBoolean() && v8_val->IsTrue()) {
		smtp->enable_tls_connection();
	}
	v8_val = config->Get(ctx, v8_str(isolate, "is_verify_ssl")).ToLocalChecked();
	if (v8_val->IsBoolean()) {
		smtp->verify_ssl(sow_web_jsx::to_boolean(isolate, v8_val));
	}
	else {
		smtp->verify_ssl(true);
	}
	v8_val = config->Get(ctx, v8_str(isolate, "is_verify_ssl_host")).ToLocalChecked();
	if (v8_val->IsBoolean()) {
		smtp->verify_ssl_host(sow_web_jsx::to_boolean(isolate, v8_val));
	}
	else {
		smtp->verify_ssl_host(true);
	}
	v8_val = config->Get(ctx, v8_str(isolate, "is_http_auth")).ToLocalChecked();
	if (v8_val->IsBoolean()) {
		smtp->http_auth(sow_web_jsx::to_boolean(isolate, v8_val));
	}
	else {
		smtp->http_auth(true);
	}
	v8_val = config->Get(ctx, v8_str(isolate, "cert_path")).ToLocalChecked();
	if (v8_val->IsString()) {
		smtp->set_server_cert(to_char_str(isolate, v8_val));
	}
	smtp->set_date_header();
	v8_val = config->Get(ctx, v8_str(isolate, "to")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP To required...")));
		delete smtp;
		return;
	}
	smtp->mail_to(to_char_str(isolate, v8_val));
	v8_val = config->Get(ctx, v8_str(isolate, "from")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP From required...")));
		delete smtp;
		return;
	}
	smtp->mail_from(to_char_str(isolate, v8_val));
	v8_val = config->Get(ctx, v8_str(isolate, "cc")).ToLocalChecked();
	if (!v8_val->IsNullOrUndefined() && v8_val->IsString()) {
		smtp->mail_cc(to_char_str(isolate, v8_val));
	}
	v8_val = config->Get(ctx, v8_str(isolate, "bcc")).ToLocalChecked();
	if (!v8_val->IsNullOrUndefined() && v8_val->IsString()) {
		smtp->mail_bcc(to_char_str(isolate, v8_val));
	}
	v8_val = config->Get(ctx, v8_str(isolate, "mail_domain")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP Domain required...")));
		delete smtp;
		return;
	}
	else {
		smtp->set_message_id(to_char_str(isolate, v8_val));
	}
	v8_val = config->Get(ctx, v8_str(isolate, "subject")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		smtp->mail_subject("");
	}
	else {
		smtp->mail_subject(to_char_str(isolate, v8_val));
	}
	smtp->prepare();
	if (smtp->has_error()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, smtp->get_last_error())));
		delete smtp;
		return;
	}
	v8_val = config->Get(ctx, v8_str(isolate, "attachments")).ToLocalChecked();
	if (!v8_val->IsNullOrUndefined() && v8_val->IsArray()) {
		v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_val);
		harr->Set(0, v8_str(isolate, ""));
		for (uint32_t i = 0, l = harr->Length(); i < l; i++) {
			v8::Local<v8::Value>v_val = harr->Get(ctx, i).ToLocalChecked();
			if (v_val->IsNullOrUndefined() || !v_val->IsObject())continue;
			v8::Local<v8::Object> def = v8::Handle<v8::Object>::Cast(v_val);
			smtp->add_attachment(
				to_char_str(isolate, def->Get(ctx, v8_str(isolate, "name")).ToLocalChecked()),
				to_char_str(isolate, def->Get(ctx, v8_str(isolate, "mim_type")).ToLocalChecked()),
				to_char_str(isolate, def->Get(ctx, v8_str(isolate, "path")).ToLocalChecked()),
				to_char_str(isolate, def->Get(ctx, v8_str(isolate, "encoder")).ToLocalChecked())
			);
			def.Clear(); v_val.Clear();
		}
		harr.Clear(); v8_val.Clear();
	}
	if (smtp->has_error()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, smtp->get_last_error())));
		delete smtp;
		return;
	}
	bool isHtml = false;
	v8_val = config->Get(ctx, v8_str(isolate, "is_html")).ToLocalChecked();
	if (!v8_val->IsNullOrUndefined() && v8_val->IsBoolean()) {
		isHtml = sow_web_jsx::to_boolean(isolate, v8_val);
	}
	v8_val = config->Get(ctx, v8_str(isolate, "body")).ToLocalChecked();
	if (v8_val->IsNullOrUndefined() || !v8_val->IsString()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, "SMTP Body required...")));
		delete smtp;
		return;
	}
	native_string body(isolate, v8_val);
	smtp->send_mail(
		/*body*/body.c_str(),
		/*is_html*/isHtml
	);
	if (smtp->has_error()) {
		isolate->ThrowException(v8::Exception::Error(
			v8_str(isolate, smtp->get_last_error())));
	}
	else {
		args.GetReturnValue().Set(v8_str(isolate, "Success"));
	}
	body.clear();
	v8_val.Clear(); config.Clear();
	delete smtp;
}
void smtp_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	wjsx_set_method(isolate, target, "create_smtp_request", smtp_request);
}

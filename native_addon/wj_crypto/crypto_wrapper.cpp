/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//11:09 AM 2/13/2020
#	include "crypto_wrapper.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include "crypto.h"
using namespace sow_web_jsx;
V8_JS_METHOD(generate_key_iv) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string key, iv, error;
	if (crypto::generate_key_iv(key, iv, error) == FALSE) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, error.c_str())));
		error.clear();
		return;
	}
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8_result->Set(
		ctx,
		v8_str(isolate, "key"),
		v8_str(isolate, key.c_str())
	);
	v8_result->Set(
		ctx,
		v8_str(isolate, "iv"),
		v8_str(isolate, iv.c_str())
	);
	args.GetReturnValue().Set(v8_result);
	key.clear(); iv.clear();
}
V8_JS_METHOD(encrypt_decrypt_file) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args.Length() < 5) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "KEY, IV, Source, Dest file path and mode required!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Base64 KEY required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Base64 IV required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Source file path required!!!")));
		return;
	}
	if (!args[3]->IsString() || args[3]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Dest file path required!!!")));
		return;
	}
	if (!args[4]->IsNumber() || args[4]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Mode required (0||1)!!!")));
		return;
	}
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Number> num = args[4]->ToNumber(ctx).ToLocalChecked();
	int should_encrypt = (int)num->ToInteger(ctx).ToLocalChecked()->Value();
	if (!(should_encrypt == TRUE || should_encrypt == FALSE)) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Mode should be (0||1)!!!")));
		return;
	}
	native_string utf_source_path(isolate, args[2]);
	native_string utf_dest_path(isolate, args[3]);
	wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
	const char* root_dir = wj_env->get_root_dir();
	std::string* source_path = new std::string(root_dir);
	sow_web_jsx::get_server_map_path(utf_source_path.c_str(), *source_path);
	if (__file_exists(source_path->c_str()) == false) {
		_free_obj(source_path);
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Source file not found....")));
		return;
	}
	std::string* dest_path = new std::string();
	dest_path->append(root_dir);
	sow_web_jsx::get_server_map_path(utf_dest_path.c_str(), *dest_path);

	native_string utf_key(isolate, args[0]);
	native_string utf_iv(isolate, args[1]);
	std::string error_str;
	int res = crypto::evp_encrypt_decrypt_file(
		/*int should_encrypt*/should_encrypt,
		/*const char* key*/utf_key.c_str(),
		/*const char* iv*/utf_iv.c_str(),
		/*const char* in_file_path*/source_path->c_str(),
		/*const char* out_file_path*/dest_path->c_str(),
		/*std::string & err*/error_str
	);
	utf_key.clear(); utf_iv.clear(); utf_source_path.clear(); utf_dest_path.clear();
	_free_obj(source_path); _free_obj(dest_path);
	if (res == FALSE) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, error_str.c_str())));
	}
}
V8_JS_METHOD(encrypt_str) {
	v8::Isolate* isolate = args.GetIsolate();

	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Base64 KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Base64 IV required!!!")));
		return;
	}
	native_string utf_plain_text(isolate, args[0]);
	native_string utf_key(isolate, args[1]);
	native_string utf_iv(isolate, args[2]);
	std::stringstream dest(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	int rec = crypto::encrypt(utf_plain_text.c_str(), utf_key.c_str(), utf_iv.c_str(), dest);
	utf_key.clear(); utf_iv.clear();
	if (rec == FALSE) {
		isolate->ThrowException(v8::Exception::Error(
			sow_web_jsx::concat_msg(isolate, "Unable to encrypt plain text.==>", dest.str().c_str())));
	}
	else {
		args.GetReturnValue().Set(v8_str(isolate, dest.str().c_str()));
	}
	utf_plain_text.clear();
	dest.clear(); std::stringstream().swap(dest);
}
V8_JS_METHOD(decrypt_str) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "IV required!!!")));
		return;
	}
	native_string utf_encrypted_text(isolate, args[0]);
	native_string utf_key(isolate, args[1]);
	native_string utf_iv(isolate, args[2]);
	std::stringstream dest(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	int rec = crypto::decrypt(utf_encrypted_text.c_str(), utf_key.c_str(), utf_iv.c_str(), dest);
	utf_key.clear(); utf_iv.clear();
	if (rec == FALSE) {
		isolate->ThrowException(v8::Exception::Error(
			sow_web_jsx::concat_msg(isolate, "Unable to decrypt encrypted text.==>", dest.str().c_str())));
	}
	else {
		args.GetReturnValue().Set(v8_str(isolate, dest.str().c_str()));
	}
	utf_encrypted_text.clear();
	dest.clear(); std::stringstream().swap(dest);
}
void cypto_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	v8::Local<v8::Object>crypto_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, crypto_object, "generate_key_iv", generate_key_iv);
	wjsx_set_method(isolate, crypto_object, "encrypt_decrypt_file", encrypt_decrypt_file);
	wjsx_set_method(isolate, crypto_object, "encrypt", encrypt_str);
	wjsx_set_method(isolate, crypto_object, "decrypt", decrypt_str);
	wjsx_set_object(isolate, target, "crypto", crypto_object);
}

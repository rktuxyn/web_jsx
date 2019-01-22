/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:24 PM 1/22/2019
#include "v8_util.h"
const char * sow_web_jsx::_toCharStr(const v8::String::Utf8Value & value) {
	if (value.length() <= 0)return "";
	return *value ? *value : "<string conversion failed>";
};
const char* sow_web_jsx::to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
	v8::String::Utf8Value str(isolate, x);
	return _toCharStr(str);
};
v8::Local<v8::String> sow_web_jsx::v8_str(v8::Isolate* isolate, const char* x) {
	return v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked();
}
void sow_web_jsx::__set__exception(v8::Isolate * isolate, v8::TryCatch * try_catch, template_result & tr) {
	tr.is_error = true;
	v8::String::Utf8Value exception(isolate, try_catch->Exception());
	tr.err_msg = _toCharStr(exception);
};


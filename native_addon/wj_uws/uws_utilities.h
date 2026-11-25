/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
//7:26 PM 12/5/2019
//https://github.com/uNetworking/uWebSockets.js
#if !defined(_web_socket_utilities_h)
#define _web_socket_utilities_h
#include <uwebsockets/App.h>
#include <v8.h>
using namespace v8;
struct PerContextData {
	Isolate* isolate;
	UniquePersistent<Object> reqTemplate;
	UniquePersistent<Object> resTemplate[2];
	UniquePersistent<Object> wsTemplate[2];
	/* We hold all apps until free */
	std::vector<std::unique_ptr<uWS::App>> apps;
	std::vector<std::unique_ptr<uWS::SSLApp>> sslApps;
};

template <class APP>
static constexpr int getAppTypeIndex() {
	/* Returns 1 for SSLApp and 0 for App */
	return std::is_same<APP, uWS::SSLApp>::value;
}

class NativeString {
	char* data;
	size_t length;
	String::Utf8Value* utf8Value = nullptr;
	bool invalid = false;
public:
	NativeString(Isolate* isolate, const Local<Value>& value) {
		if (value->IsUndefined()) {
			data = nullptr;
			length = 0;
		}
		else if (value->IsString()) {
			char utf8ValueMemory[sizeof(String::Utf8Value)];
			utf8Value = new (utf8ValueMemory) String::Utf8Value(isolate, value);
			data = (**utf8Value);
			length = utf8Value->length();
		}
		else if (value->IsTypedArray()) {
			Local<ArrayBufferView> arrayBufferView = Local<ArrayBufferView>::Cast(value);
			ArrayBuffer::Contents contents = arrayBufferView->Buffer()->GetContents();
			length = arrayBufferView->ByteLength();
			data = (char*)contents.Data() + arrayBufferView->ByteOffset();
		}
		else if (value->IsArrayBuffer()) {
			Local<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::Cast(value);
			ArrayBuffer::Contents contents = arrayBuffer->GetContents();
			length = contents.ByteLength();
			data = (char*)contents.Data();
		}
		else {
			invalid = true;
		}
	}

	bool isInvalid(const FunctionCallbackInfo<Value>& args) {
		if (invalid) {
			args.GetReturnValue().Set(args.GetIsolate()->ThrowException(String::NewFromUtf8(args.GetIsolate(), "Text and data can only be passed by String, ArrayBuffer or TypedArray.", NewStringType::kNormal).ToLocalChecked()));
		}
		return invalid;
	}

	std::string_view getString() {
		return { data, length };
	}

	~NativeString() {
		if (utf8Value) {
			utf8Value->~Utf8Value();
		}
	}
};
#endif
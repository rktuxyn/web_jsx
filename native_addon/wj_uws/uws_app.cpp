/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:26 PM 12/5/2019
//https://github.com/uNetworking/uWebSockets.js
#	include "uws_app.h"
#	include <uwebsockets/App.h>
#	include <iostream>
#	include <vector>
#	include <type_traits>
#	include <v8.h>
using namespace v8;
/* Compatibility for V8 7.0 and earlier */
#	include <v8-version.h>
bool BooleanValue(Isolate* isolate, Local<Value> value) {
#if V8_MAJOR_VERSION < 7 || (V8_MAJOR_VERSION == 7 && V8_MINOR_VERSION == 0)
	/* Old */
	return value->BooleanValue(isolate->GetCurrentContext()).ToChecked();
#else
	/* Node.js 12, 13 */
	return value->BooleanValue(isolate);
#endif
}
#	include "uws_utilities.h"
#	include "uws_web_socket_wrapper.h"
#	include "uws_http_response_wrapper.h"
#	include "uws_http_request_wrapper.h"
#	include "uws_app_wrapper.h"
namespace sow_web_jsx {
	void uWS_free(const FunctionCallbackInfo<Value>& args) {
		/* We get the External holding perContextData */
		PerContextData* perContextData = (PerContextData*)Local<External>::Cast(args.Data())->Value();

		/* Todo: this will always be true */
		if (perContextData) {
			/* Freeing apps here, it could be done earlier but not sooner */
			perContextData->apps.clear();
			perContextData->sslApps.clear();
			/* Freeing the loop here means we give time for our timers to close, etc */
			uWS::Loop::get()->free();


			// we need to mark this
			delete perContextData;

			// we can override the exports->free function to null after!

			//args.Data()

			//Local<External>::Cast(args.Data())->
		}
	}
	/* todo: Put this function and all inits of it in its own header */
	void uWS_us_listen_socket_close(const FunctionCallbackInfo<Value>& args) {
		// this should take int ssl first
		us_listen_socket_close(0, (struct us_listen_socket_t*) External::Cast(*args[0])->Value());
	}
	void uws_export(const v8::FunctionCallbackInfo<v8::Value>& args) {
		//Isolate* isolate, Local<Object> exports
		v8::Isolate* isolate = args.GetIsolate();
		v8::Handle<v8::Object> exports = v8::Object::New(isolate);
		/* We want this so that we can redefine process.nextTick to using the V8 native microtask queue */
		// todo: setting this might be crashing nodejs?
		isolate->SetMicrotasksPolicy(MicrotasksPolicy::kAuto);

		/* Init the template objects, SSL and non-SSL, store it in per context data */
		PerContextData* perContextData = new PerContextData;
		perContextData->isolate = isolate;
		perContextData->reqTemplate.Reset(isolate, HttpRequestWrapper::init(isolate));
		perContextData->resTemplate[0].Reset(isolate, HttpResponseWrapper::init<0>(isolate));
		perContextData->resTemplate[1].Reset(isolate, HttpResponseWrapper::init<1>(isolate));
		perContextData->wsTemplate[0].Reset(isolate, WebSocketWrapper::init<0>(isolate));
		perContextData->wsTemplate[1].Reset(isolate, WebSocketWrapper::init<1>(isolate));

		/* Refer to per context data via External */
		Local<External> externalPerContextData = External::New(isolate, perContextData);

		/* uWS namespace */
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "App", NewStringType::kNormal).ToLocalChecked(), FunctionTemplate::New(isolate, uWS_App<uWS::App>, externalPerContextData)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked();
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "SSLApp", NewStringType::kNormal).ToLocalChecked(), FunctionTemplate::New(isolate, uWS_App<uWS::SSLApp>, externalPerContextData)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked();
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "free", NewStringType::kNormal).ToLocalChecked(), FunctionTemplate::New(isolate, uWS_free, externalPerContextData)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked();

		/* Expose some µSockets functions directly under uWS namespace */
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "us_listen_socket_close", NewStringType::kNormal).ToLocalChecked(), FunctionTemplate::New(isolate, uWS_us_listen_socket_close)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()).ToChecked();

		/* Compression enum */
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "DISABLED", NewStringType::kNormal).ToLocalChecked(), Integer::NewFromUnsigned(isolate, uWS::DISABLED)).ToChecked();
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "SHARED_COMPRESSOR", NewStringType::kNormal).ToLocalChecked(), Integer::NewFromUnsigned(isolate, uWS::SHARED_COMPRESSOR)).ToChecked();
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "DEDICATED_COMPRESSOR", NewStringType::kNormal).ToLocalChecked(), Integer::NewFromUnsigned(isolate, uWS::DEDICATED_COMPRESSOR)).ToChecked();

		/* Listen options */
		exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "LIBUS_LISTEN_EXCLUSIVE_PORT", NewStringType::kNormal).ToLocalChecked(), Integer::NewFromUnsigned(isolate, LIBUS_LISTEN_EXCLUSIVE_PORT)).ToChecked();
		args.GetReturnValue().Set(exports);
		uWS::Loop::get()->run();
	}
}
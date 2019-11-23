/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "v8_engine.h"

void sow_web_jsx::js_compiler::v8_engine::create_engine(const char* exec_path) {
	v8::V8::InitializeICUDefaultLocation(exec_path);
	v8::V8::InitializeExternalStartupData(exec_path);
	//v8::Platform* platforms = v8::platform::CreateDefaultPlatform();
	//std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	_platform = v8::platform::CreateDefaultPlatform();
	//_platform = platform.get();
	//_platform = v8::platform::NewDefaultPlatform().get();
	v8::V8::InitializePlatform(_platform);
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	v8::V8::Initialize();
	_isolate = v8::Isolate::New(create_params);
}

sow_web_jsx::js_compiler::v8_engine::v8_engine(const char * exec_path) {
	_disposed = false;
	create_engine(exec_path);
	//has_context = false;
}

v8::Isolate * sow_web_jsx::js_compiler::v8_engine::get_current_isolate() {
	if (_disposed) {
		throw new std::runtime_error("Engine already disposed!!!");
	}
	v8::Isolate*iso = v8::Isolate::GetCurrent();
	if (iso != NULL)return iso;
	return _isolate;
}

/*v8::Local<v8::Context> sow_web_jsx::js_compiler::v8_engine::get_context() {
	return v8::Local<v8::Context>::New(_isolate, _context);
}

void sow_web_jsx::js_compiler::v8_engine::set_context(v8::Local<v8::Context> ctx) {
	if (has_context)return;
	has_context = true;
	_context.Reset(_isolate, ctx);
}*/

void sow_web_jsx::js_compiler::v8_engine::dispose() {
	if (_disposed)return;
	_disposed = true;
	//v8::Local<v8::Context> ctx = v8::Local<v8::Context>::New(_isolate, _context);
	//ctx->DetachGlobal();
	//ctx.Clear();
	//_isolate->LowMemoryNotification();
	_isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	//delete _platform;
	_isolate = NULL;
}

sow_web_jsx::js_compiler::v8_engine::~v8_engine() {
	if (_disposed)return;
	dispose();
}

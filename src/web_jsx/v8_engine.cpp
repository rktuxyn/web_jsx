/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "v8_engine.h"
void sow_web_jsx::js_compiler::v8_engine::create_engine(const char * exec_path) {
	v8::V8::InitializeICUDefaultLocation(exec_path);
	v8::V8::InitializeExternalStartupData(exec_path);
	_platform = v8::platform::CreateDefaultPlatform();
	v8::V8::InitializePlatform(_platform);
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	v8::V8::Initialize();
	_isolate = v8::Isolate::New(create_params);
};

sow_web_jsx::js_compiler::v8_engine::v8_engine(const char * exec_path) {
	_disposed = false;
	create_engine(exec_path);
};

v8::Isolate * sow_web_jsx::js_compiler::v8_engine::get_current_isolate() {
	return _isolate;
};

void sow_web_jsx::js_compiler::v8_engine::dispose() {
	if (_disposed)return;
	_disposed = true;
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	delete _platform;
	_isolate = NULL;
};

sow_web_jsx::js_compiler::v8_engine::~v8_engine() {
	if (_disposed)return;
	_disposed = true;
	dispose();
};

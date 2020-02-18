/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "v8_engine.h"
#	include <libplatform/libplatform.h>
#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE
void set_v8_flag() {
	const char* arg[3] = {
		"--sim_stack_size=20480",
		"--expose_async_hooks",
		//"--trace_turbo_jt",
		"--expose_gc"
	};
	int* argc = new int(3);
	v8::V8::SetFlagsFromCommandLine(argc, (char**)arg, true);
	delete argc;
	//v8::V8::SetFlagsFromString(v8Flags, (int)strlen(v8Flags));
}
void sow_web_jsx::js_compiler::v8_engine::create_engine(const char* exec_path) {
	/*std::string app_dir(exec_path);
	app_dir.append("\\");*/
	//v8::V8::InitializeICUDefaultLocation(exec_path);
	//v8::V8::InitializeExternalStartupData(exec_path);
	//_platform = v8::platform::NewDefaultPlatform();
	//_platform = std::move(v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled));
	/*v8::StartupData data;
	data.data = "";
	data.raw_size = 10;
	v8::V8::SetNativesDataBlob(&data);
	v8::V8::SetSnapshotDataBlob(&data);*/
	_platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	//std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	//std::make_unique<v8::Platform> pxn = v8::platform::NewDefaultPlatform();
	//_platform = platform.get();
	//v8::V8::InitializePlatform(platform.get());
	//v8::Platform* platforms = v8::platform::CreateDefaultPlatform();
	//std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	//_platform = v8::platform::CreateDefaultPlatform();
	//_platform = platform.get();
	//_platform = v8::platform::NewDefaultPlatform().get();
	v8::V8::InitializePlatform(_platform.get());
	v8::V8::Initialize();
	set_v8_flag();
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator =
		v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	//v8::V8::Initialize();
	_isolate = v8::Isolate::New(create_params);
}


sow_web_jsx::js_compiler::v8_engine::v8_engine(const char * exec_path) {
	_disposed = FALSE;
	create_engine(exec_path);
	//has_context = false;
}

v8::Isolate * sow_web_jsx::js_compiler::v8_engine::get_current_isolate() {
	if (_disposed == TRUE) {
		throw new std::runtime_error("Engine already disposed!!!");
	}
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	if (isolate != NULL)return isolate;
	return _isolate;
}

v8::Isolate* sow_web_jsx::js_compiler::v8_engine::get_global_isolate(){
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

void sow_web_jsx::js_compiler::v8_engine::wait_for_pending_task() {
	if (_disposed == TRUE)return;
	v8::Platform* platform = _platform.get();
	while (v8::platform::PumpMessageLoop(platform, _isolate))
		continue;
}
void sow_web_jsx::js_compiler::v8_engine::dispose() {
	if (_disposed == TRUE)return;
	//wait_for_pending_task();
	_disposed = TRUE;
	//v8::Local<v8::Context> ctx = v8::Local<v8::Context>::New(_isolate, _context);
	//ctx->DetachGlobal();
	//ctx.Clear();
	//_isolate->LowMemoryNotification();
	_isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	_platform.reset();
	//delete _platform;
	_isolate = NULL;
}

sow_web_jsx::js_compiler::v8_engine::~v8_engine() {
	dispose();
}

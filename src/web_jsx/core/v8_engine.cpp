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

#if !defined(n_isolate_pool)
#	define n_isolate_pool new isolate_pool
#endif//!n_isolate_pool

#if !defined(_dispose_isolate)
#define _dispose_isolate(isolate)\
while(isolate){\
isolate->ContextDisposedNotification();\
isolate->IdleNotificationDeadline(_platform->MonotonicallyIncreasingTime() + 1.0);\
isolate->LowMemoryNotification();\
isolate->Dispose();\
isolate=NULL;\
}
#endif//!_dispose_isolate

#if !defined(_free_pool)
#	define _free_pool(pool)\
pool->busy=0; pool->error_code=0;\
pool->error_msg=NULL;\

#endif//!_free_module

#if !defined(_free_pools)
#	define _free_pools(pools)\
while(pools){\
isolate_pool* pool = pools;\
pools = pools->next;\
_free_pool(pool)\
_dispose_isolate(pool->isolate);\
}
#endif//!_free_module

using namespace sow_web_jsx::js_compiler;
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
//class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
//public:
//	virtual void* Allocate(size_t length) {
//		void* data = AllocateUninitialized(length);
//		return data == NULL ? data : memset(data, 0, length);
//	}
//	virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
//	virtual void Free(void* data, size_t) { free(data); }
//};
void v8_engine::create_engine(const char* exec_path) {
	set_v8_flag();
	v8::V8::InitializeICUDefaultLocation(exec_path);
	/*v8::StartupData data;
	data.data = "";
	data.raw_size = 10;
	v8::V8::SetNativesDataBlob(&data);
	v8::V8::SetSnapshotDataBlob(&data);*/
	_platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	v8::V8::InitializePlatform(_platform.get());
	v8::V8::Initialize();
	v8::V8::InitializeExternalStartupData(exec_path);
}

isolate_pool* v8_engine::create_isolate() {
	if (_array_buffer_allocator == NULL) {
		_array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	}
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = _array_buffer_allocator;
	v8::Isolate* isolate = v8::Isolate::New(create_params);
	isolate_pool* pool = n_isolate_pool;
	pool->isolate = isolate;
	pool->next = _pools;
	pool->busy = TRUE;
	pool->error_code = FALSE;
	_pools = pool;
	return pool;
}


v8_engine::v8_engine(const char * exec_path) {
	_disposed = FALSE; _pools = NULL;
	create_engine(exec_path);
}

//v8::Isolate * v8_engine::get_current_isolate() {
//	if (_disposed == TRUE)return NULL;
//	v8::Isolate* isolate = v8::Isolate::GetCurrent();
//	if (isolate != NULL)return isolate;
//	return _isolate;
//}
v8::Isolate* v8_engine::get_current_isolate() {
	if (_disposed == TRUE)return NULL;
	if (_pools != NULL)return _pools->isolate;
	isolate_pool* i_pool = NULL;
	if (_pools != NULL) {
		for (i_pool = _pools; i_pool; i_pool = _pools->next) {
			if (i_pool->busy < 0 || !i_pool->busy) break;
		}
		if (i_pool && i_pool->busy) {
			i_pool = NULL;
		}
	}
	if (i_pool != NULL && i_pool->busy == 0 ) {
		return i_pool->isolate;
	}
	return create_isolate()->isolate;
}
v8::Isolate* v8_engine::get_global_isolate(){
	if (_pools == NULL)return NULL;
	return  _pools->isolate;
}
void v8_engine::wait_for_pending_task() {
	if (_disposed == TRUE)return;
	if (_pools == NULL)return;
	v8::Platform* platform = _platform.get();
	while (v8::platform::PumpMessageLoop(platform, _pools->isolate))
		continue;
}
void v8_engine::dispose() {
	if (_disposed == TRUE)return;
	_disposed = TRUE;
	_free_pools(_pools);
	if (_array_buffer_allocator != NULL) {
		delete _array_buffer_allocator;
		_array_buffer_allocator = NULL;
	}
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	_platform.reset();
	//_isolate = NULL;
}

v8_engine::~v8_engine() {
	dispose();
}

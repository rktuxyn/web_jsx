/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "v8_engine.h"
//#if (defined(_WIN32)||defined(_WIN64))
//#	include "web_jsx_global.h"
//#endif//!_WIN32
#	include "wjsx_env.h"
#	include <memory>
#	include <libplatform/libplatform.h>

#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE

using namespace sow_web_jsx;
std::shared_ptr<v8::Platform> _platform;
std::shared_ptr<v8::ArrayBuffer::Allocator> _array_buffer_allocator;
int _disposed = FALSE, _initialize = FALSE;

//#if (defined(_WIN32)||defined(_WIN64))
//int translateSEH(EXCEPTION_POINTERS * pExcept) {
//	// Translate SEH exception to a C++ exception.  NOTE: compiling with /EHa is required!!
//	char* msg = new char[256];
//#pragma warning(push)
//#pragma warning(disable:4477)
//	sprintf_s(msg, 256, "Unhandled exception 0x%08x at 0x%08x",
//		pExcept->ExceptionRecord->ExceptionCode,
//		pExcept->ExceptionRecord->ExceptionAddress);
//#pragma warning(pop)
//	throw std::exception(msg);
//}
//#endif//!_WIN32

void set_v8_flag() {
	const char* arg[2] = {
		/*"--sim_stack_size=20480",
		//"--stack_size=20480",
		"--cleanup_code_caches_at_gc",*/
		"--expose_async_hooks",
		"--expose_gc"
	};
	int* argc = new int(2);
	v8::V8::SetFlagsFromCommandLine(argc, (char**)arg, true);
	delete argc;
//#if (defined(_WIN32)||defined(_WIN64))
//	v8::V8::SetUnhandledExceptionCallback(translateSEH);
//#endif//!_WIN32
	//v8::V8::SetFlagsFromString(v8Flags, (int)strlen(v8Flags));
}
size_t _memory_limit = 0;
size_t _last_heap_size = 0;
#define _1_MB 1024 * 1024
void GCEpilogueCallback(v8::Isolate* isolate, v8::GCType type, v8::GCCallbackFlags /* flags */) {
	v8::HeapStatistics heap_statistics;
	isolate->GetHeapStatistics(&heap_statistics);

	if (type != v8::GCType::kGCTypeIncrementalMarking
		&& heap_statistics.used_heap_size() > _memory_limit* _1_MB) {
		isolate->TerminateExecution();
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "OOM error in GC")));
		if (heap_statistics.used_heap_size() > _last_heap_size* _1_MB / 0.9
			&& _last_heap_size < _memory_limit * _1_MB / 0.9) {
			isolate->LowMemoryNotification();
		}
		_last_heap_size = heap_statistics.used_heap_size();
	}
}
void v8_engine::create_engine(const char* exec_path){
	ASSERT(_disposed == FALSE);
	ASSERT(_initialize == FALSE);
	set_v8_flag();
	v8::V8::InitializeICUDefaultLocation(exec_path);
	_platform = v8::platform::NewDefaultPlatform(0, v8::platform::IdleTaskSupport::kEnabled);
	v8::V8::InitializePlatform(_platform.get());
	v8::V8::Initialize();
	v8::V8::InitializeExternalStartupData(exec_path);
	_array_buffer_allocator = std::shared_ptr<v8::ArrayBuffer::Allocator>(v8::ArrayBuffer::Allocator::NewDefaultAllocator());
	_initialize = TRUE;
	
}

v8::Isolate* v8_engine::create_isolate(){
	ASSERT(_disposed == FALSE);
	ASSERT(_initialize == TRUE);
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = _array_buffer_allocator.get();
	return v8::Isolate::New(create_params);
}

v8::Isolate* sow_web_jsx::v8_engine::create_isolate(std::mutex&mutex){
	std::lock_guard<std::mutex> guard(mutex);
	return v8_engine::create_isolate();
}

void v8_engine::wait_for_pending_task(v8::Isolate* isolate){
	ASSERT(_disposed == FALSE);
	ASSERT(_initialize == TRUE);
	v8::Platform* platform = _platform.get();
	while (v8::platform::PumpMessageLoop(platform, isolate))
		continue;
}

void v8_engine::dispose_isolate(v8::Isolate* isolate){
	ASSERT(_disposed == FALSE);
	ASSERT(_initialize == TRUE);
	_dispose_isolate(isolate);
}

void sow_web_jsx::v8_engine::dispose_isolate(v8::Isolate* isolate, std::mutex& mutex){
	std::lock_guard<std::mutex> guard(mutex);
	v8_engine::dispose_isolate(isolate);
}

void sow_web_jsx::v8_engine::wait_for_pending_task(v8::Isolate* isolate, std::mutex& mutex){
	std::lock_guard<std::mutex> guard(mutex);
	v8_engine::wait_for_pending_task(isolate);
}

void v8_engine::dispose_engine(){
	ASSERT(_disposed == FALSE);
	ASSERT(_initialize == TRUE);
	_disposed = TRUE;
	_array_buffer_allocator.reset();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	_platform.reset();
}
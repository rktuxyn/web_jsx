/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _v8_engine_h)
#	define _v8_engine_h
#	include <v8.h>
#if defined(WEB_JSX_MULTI_THREAD)
#	include <mutex>
#endif//!WEB_JSX_MULTI_THREAD
#if !defined(_MAX_ISOLATE)
#	define _MAX_ISOLATE		50
#	define _MIN_ISOLATE		10
#endif//!_MAX_ISOLATE

#if !defined(_dispose_isolate)
#define _dispose_isolate(isolate)		\
while(isolate){							\
isolate->ContextDisposedNotification();	\
isolate->Dispose();						\
isolate = NULL;							\
}
#endif//!_dispose_isolate

namespace sow_web_jsx {
	namespace v8_engine {
		//11:04 PM 2/23/2020
		void create_engine(const char* exec_path);
		v8::Isolate* create_isolate();
#if defined(WEB_JSX_MULTI_THREAD)
		v8::Isolate* create_isolate(std::mutex&mutex);
		void dispose_isolate(v8::Isolate* isolate, std::mutex& mutex);
		void wait_for_pending_task(v8::Isolate* isolate, std::mutex& mutex);
#endif//!WEB_JSX_MULTI_THREAD
		void wait_for_pending_task(v8::Isolate* isolate);
		void dispose_isolate(v8::Isolate* isolate);
		void dispose_engine();
	}
}
#endif//!_v8_engine_h
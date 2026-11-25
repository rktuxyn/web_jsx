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

namespace fsys_web_jsx {
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
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
#define _v8_engine_h
#if !defined( _v8_util_h)
#include "v8_util.h"
#endif //!_v8_util_h
#if !defined(V8_LIBPLATFORM_LIBPLATFORM_H_)
#include <libplatform/libplatform.h>
#endif//!V8_LIBPLATFORM_LIBPLATFORM_H_
#if !defined(_native_wrapper_h)
#include "native_wrapper.h"
#endif//!_native_wrapper_h
#if !defined(UV_H)
#include <uv.h>
#endif//!UV_H
namespace sow_web_jsx {
	namespace js_compiler {
		class v8_engine {
		private:
			v8::Isolate* _isolate;
			v8::Platform* _platform;
			bool _disposed;
			void create_engine(const char* exec_path);
			//v8::Persistent<v8::Context>_context;
		public:
			//bool has_context;
			v8_engine(const char* exec_path);
			v8::Isolate* get_current_isolate();
			//v8::Local<v8::Context> get_context();
			//void set_context(v8::Local<v8::Context> ctx);
			void dispose();
			~v8_engine();
		};
	}
}
#endif//
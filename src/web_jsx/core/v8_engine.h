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
#include "v8_util.h"
namespace sow_web_jsx {
	namespace js_compiler {
		class v8_engine {
		private:
			v8::Isolate* _isolate;
			std::shared_ptr<v8::Platform> _platform;
			bool _disposed;
			void create_engine(const char* exec_path);
		public:
			explicit v8_engine(const char* exec_path);
			v8::Isolate* get_current_isolate();
			v8::Isolate* get_global_isolate();
			void wait_for_pending_task();
			void dispose();
			~v8_engine();
		};
	}
}
#endif//!_v8_engine_h
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//5:19 PM 3/30/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_jsx_file_h)
#	define _jsx_file_h
#	include <v8.h>
namespace sow_web_jsx {
	void jsx_file_bind(v8::Isolate* isolate, v8::Local<v8::Object> ctx);
}
#endif//_jsx_file_h
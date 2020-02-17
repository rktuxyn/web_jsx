/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:42 PM 12/13/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_npgsql_wrapper_h)
#	define _npgsql_wrapper_h
#	include <web_jsx/v8_util.h>
namespace sow_web_jsx {
	void npgsql_export(v8::Isolate* isolate, v8::Handle<v8::Object> target);
}
#endif//!_npgsql_wrapper_h
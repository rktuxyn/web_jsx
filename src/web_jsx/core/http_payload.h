/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:00 PM 12/24/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _http_payload_h)
#	define _http_payload_h
#	include "v8_util.h"
#	include <libplatform/libplatform.h>
namespace sow_web_jsx {
	void read_http_posted_file(const v8::FunctionCallbackInfo<v8::Value>& args);
	void read_payload(const v8::FunctionCallbackInfo<v8::Value>& args);
	void write_file_from_payload(const v8::FunctionCallbackInfo<v8::Value>& args);
}
#endif//!_http_payload_h

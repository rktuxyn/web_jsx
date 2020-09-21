/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:25 PM 3/9/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_wj_core_wrapper_h)
#	define _wj_core_wrapper_h
#include <v8.h>
void export_wj_core(v8::Isolate* isolate, v8::Handle<v8::Object> target);
#endif//!_wj_core_wrapper_h

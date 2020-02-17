/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _spawn_uv_h)
#	define _spawn_uv_h
#	include <v8.h>
void export_uv(v8::Isolate* isolate, v8::Handle<v8::Object> target);
#endif//!_spawn_uv_h
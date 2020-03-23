/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:42 AM 1/11/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_bitmap_h)
#	define _bitmap_h
#	include <v8.h>
void bitmap_export(v8::Isolate* isolate, v8::Handle<v8::Object> target);
#endif//!_bitmap_h
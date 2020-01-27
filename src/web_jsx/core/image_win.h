/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:57 PM 1/16/2020
#if (defined(_WIN32)||defined(_WIN64))
#if !defined(_image_win_h)
#	define _image_win_h
#	include <v8.h>
void image_export(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx);
#endif//!_image_win_h
#endif//!_WIN32||_WIN64
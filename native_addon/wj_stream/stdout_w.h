/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:13 PM 2/12/2020
#if !defined(_stdout_w_h)
#	define _stdout_w_h
#	include <v8.h>
void stdout_export(v8::Isolate* isolate, v8::Handle<v8::Object> target);
#endif//!_image_win_h
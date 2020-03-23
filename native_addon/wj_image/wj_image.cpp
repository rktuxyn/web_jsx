/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//4:05 PM 2/3/2020
#	include <web_jsx/web_jsx.h>
#if (defined(_WIN32)||defined(_WIN64))
#	include "image_win.h"
#else
#	include "image_unix.h"
#endif//!_WIN32||_WIN64
#	include "bitmap.h"

void web_jsx_native_module(v8::Handle<v8::Object> target) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	image_export(isolate, target);
	bitmap_export(isolate, target);
	return;
}
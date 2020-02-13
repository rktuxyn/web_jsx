/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:14 PM 2/12/2020
#	include <web_jsx/web_jsx.h>
#	include "zlib_w.h"
#	include "stdout_w.h"
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gzip_compressor_export(isolate, target);
	stdout_export(isolate, target);
	return;
}
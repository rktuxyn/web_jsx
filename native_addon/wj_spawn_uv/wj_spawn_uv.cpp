/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:58 PM 2/10/2020
#	include <web_jsx/web_jsx.h>
#	include "spawn_uv.h"
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	export_uv(v8::Isolate::GetCurrent(), target);
	return;
}
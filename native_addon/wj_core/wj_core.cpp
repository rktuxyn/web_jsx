/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:40 AM 3/9/2020
#	include <web_jsx/web_jsx.h>
#	include "wj_core_wrapper.h"
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	export_wj_core(v8::Isolate::GetCurrent(), target);
	return;
}
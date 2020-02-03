/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "uws_app.h"
#	include <web_jsx/web_jsx.h>
//11:20 PM 2/1/2020
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	wjsx_set_method(isolate, target, "uws_export", sow_web_jsx::uws_export);
	return;
}
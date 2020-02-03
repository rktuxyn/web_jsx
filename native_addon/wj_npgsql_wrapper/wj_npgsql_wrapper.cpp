/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "npgsql_wrapper.h"
#	include <web_jsx/web_jsx.h>
//11:20 PM 2/1/2020
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	sow_web_jsx::npgsql_export(v8::Isolate::GetCurrent(), target);
	return;
}
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:59 AM 2/13/2020
#	include <web_jsx/web_jsx.h>
#	include "crypto_wrapper.h"
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	cypto_export(v8::Isolate::GetCurrent(), target);
	return;
}

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//8:59 PM 2/12/2020
#	include <web_jsx/web_jsx.h>
#	include "curl_http_wrapper.h"
#	include "curl_smtp_wrapper.h"
#	include "curl_download_wrapper.h"
void web_jsx_native_module(v8::Handle<v8::Object> target) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	::http_export(isolate, target);
	::smtp_export(isolate, target);
	::http_download_export(isolate, target);
	sow_web_jsx::register_resource(::on_resource_free);
	return;
}
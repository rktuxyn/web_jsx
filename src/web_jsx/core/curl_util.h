/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:08 PM 12/25/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_curl_util_h)
#	define _curl_util_h
#	include <curl/curl.h>
namespace sow_web_jsx {
	CURLcode ssl_ctx_callback(CURL* curl, void* ssl_ctx, void* userptr);
	int debug_log(CURL* handle, curl_infotype type,
		char* data, size_t size,
		void* userp);
}
#endif
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//11:25 AM 4/8/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_download_request_h)
#	define _http_request_h
#	include <string>
#	include <curl/curl.h>
namespace http_client {
	class http_download {
	private:
		CURL* _curl;
		struct curl_slist* _header_chunk;
		int _disposed;
		char* _internal_error;
		const char* _full_url;
		int _is_debug;
		void set_error(const char* error);
		void prepare();
	public:
		explicit http_download(const char* full_url);
		~http_download();
		const char* get_last_error();
		void set_header(const char* header);
		void set_cookie(const char* cookie);
		void read_debug_information(bool isDebug);
		void verify_ssl(bool verify);
		void verify_ssl_host(bool verify);
		int download(const char* out_file, int show_progress);
	};
};
#endif//!_download_request_h

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//5:37 AM 12/2/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_http_request_h)
#	define _http_request_h
#	include <string>
#	include <curl/curl.h>
namespace http_client {
	class http_request {
	private:
		CURL* _curl;
		struct curl_slist* _header_chunk;
		bool _follow_location;
		bool _disposed;
		char* _internal_error;
		const char* _full_url;
		void set_error(const char* error);
		void prepare();
		int send_request(std::string& response_header, std::string& response_body);
	public:
		explicit http_request(const char* full_url, bool follow_location);
		~http_request();
		const char* get_last_error();
		void set_header(const char* header);
		void set_cookie(const char* cookie);
		void read_debug_information(bool isDebug);
		void verify_ssl(bool verify);
		void verify_ssl_host(bool verify);
		int get(std::string& response_header, std::string& response_body);
		int post(const char* body, std::string& response_header, std::string& response_body);
	};
};

#endif//!_http_request_h
//5:37 AM 12/2/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_http_request_h)
#	define _http_request_h
#	include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#	include <stdlib.h>// exit, atoi, malloc, free
#	include <string>// !_XSTRING_// memcpy, memset
#	include <iostream>
#	include <curl/curl.h>
#	include "glb_r.h"
#	include "curl_util.h"
namespace http_client {
	class http_request {
	private:
		CURL* _curl;
		struct curl_slist* _header_chunk;
		bool _follow_location;
		bool _disposed;
		char* _internal_error;
		const char* _full_url;
		virtual void set_error(const char* error);
		virtual void prepare();
		virtual int send_request(std::string& response_header, std::string& response_body);
	public:
		http_request(const char* full_url, bool follow_location);
		virtual ~http_request();
		virtual const char* get_last_error();
		virtual void set_header(const char* header);
		virtual void set_cookie(const char* cookie);
		virtual void read_debug_information(bool isDebug);
		virtual void verify_ssl(bool verify);
		virtual void verify_ssl_host(bool verify);
		virtual int get(std::string& response_header, std::string& response_body);
		virtual int post(const char* body, std::string& response_header, std::string& response_body);
	};
};

#endif//!_http_request_h
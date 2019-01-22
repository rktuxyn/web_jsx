//5:37 AM 12/2/2018
#pragma once
#pragma warning(disable : 4996)
#if !defined(_http_request_h)
#define _http_request_h
#if !defined(_INC_STDIO)
#include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#endif//!_INC_STDIO
#if !defined(_INC_STDLIB)
#include <stdlib.h>// exit, atoi, malloc, free
#endif // !_INC_STDLIB
#if !defined(_XSTRING_)
#include <string>// !_XSTRING_// memcpy, memset
#endif //!_XSTRING_
#if !defined(_IOSTREAM_)
#include <iostream>
#endif//!_IOSTREAM_
#if !defined(__CURL_CURL_H)
#include <curl/curl.h>
#endif//!__CURL_CURL_H
namespace http_client {
	class http_request {
	private:
		CURL *_curl;
		struct curl_slist *_header_chunk;
		bool _follow_location;
		bool _disposed;
		char* _internal_error;
		const char*_full_url;
		virtual void set_error(const char* error);
		virtual void prepare();
		virtual int send_request(std::string & response_header, std::string &response_body);
	public:
		http_request(const char*full_url, bool follow_location);
		virtual ~http_request();
		virtual const char* get_last_error();
		virtual void set_header(const char*header);
		virtual void set_cookie(const char*cookie);
		virtual int get(std::string & response_header, std::string &response_body);
		virtual int post(const char*body, std::string & response_header, std::string &response_body);
	};
};

#endif//!_http_request_h
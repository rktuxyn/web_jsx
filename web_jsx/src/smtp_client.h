//7:08 PM 12/25/2018
#pragma once
#pragma warning(disable : 4996)
#if !defined(_smtp_client_h)
#define _smtp_client_h
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
#if !defined(_CTIME_)
#include <ctime>
#endif//!_CTIME_
#if !defined(_CSTRING_)
#include <cstring>
#endif//!_CSTRING_
#if !defined(jsx_export)
#define jsx_export __declspec(dllexport)
#endif//!jsx_export
//https://curl.haxx.se/libcurl/c/smtp-mail.html
//https://codereview.stackexchange.com/questions/139784/sending-email-using-libcurl
namespace smtp_client {
	/*std::string date_time_now() {
		const int RFC5322_TIME_LEN = 32;
		time_t t;
		struct tm *tm;

		std::string ret;
		ret.resize(RFC5322_TIME_LEN);

		time(&t);
		tm = localtime(&t);

		strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", tm);

		return ret;
	};*/
	class smtp_request {
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
		smtp_request(const char*full_url, bool follow_location);
		virtual ~smtp_request();
		const char* get_last_error();
		virtual void set_header(const char*header);
		virtual void set_cookie(const char*cookie);
		virtual int get(std::string & response_header, std::string &response_body);
		virtual int post(const char*body, std::string & response_header, std::string &response_body);
	};
};

#endif//!_smtp_client_h

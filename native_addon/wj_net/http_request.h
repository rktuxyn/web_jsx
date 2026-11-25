/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
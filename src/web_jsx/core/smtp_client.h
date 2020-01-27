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
#if !defined(_smtp_client_h)
#	define _smtp_client_h
#	include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#	include <stdlib.h>// exit, atoi, malloc, free
#	include <string>// !_XSTRING_// memcpy, memset
#	include <iostream>
#	include <curl/curl.h>
#	include <ctime>
#	include <cstring>
#	include "glb_r.h"
#	include <sstream>
#	include <regex>
#	include <vector>
#	include "curl_util.h"
//https://curl.haxx.se/libcurl/c/smtp-mail.html
//https://codereview.stackexchange.com/questions/139784/sending-email-using-libcurl
//https://curl.haxx.se/libcurl/c/smtp-mime.html
namespace smtp_client {
	typedef struct {
		size_t lines_read; /* count up */
		std::vector<char*> data;
	}smtp_ctx;
	typedef struct {
		std::string name; /* count up */
		std::string path;
		std::string mime_type;
		std::string encoder;
	}mail_attachment;
	class smtp_request {
	private:
		//smtp_ctx _upload_ctx;
		CURL *_curl;
		bool _disposed;
		bool _has_error;
		bool _is_debug;
		char* _internal_error;
		struct curl_slist* _recipients;
		struct curl_slist* _cheaders;
		std::vector<std::string>* _headers;
		std::vector<mail_attachment*>* _attachments;
		virtual bool init();
		virtual void log(const char* format, const char* str);
		virtual void set_error(const char* error);
	public:
		smtp_request();
		virtual bool has_error();
		virtual void host(const std::string host);
		virtual void credentials(const std::string user, const std::string password);
		virtual void http_auth(bool is_http_auth);
		virtual void add_attachment(
			const std::string name, 
			const std::string mime_type,
			const std::string path,
			const std::string encoder
		);
		virtual void set_date_header();
		virtual void set_message_id(const std::string mail_domain);
		virtual void mail_from(const std::string from);
		virtual void mail_to(const std::string to);
		virtual void mail_cc(const std::string cc);
		virtual void mail_bcc(const std::string bcc);
		virtual void mail_subject(const std::string subject);
		virtual void read_debug_information(bool isDebug);
		virtual void verify_ssl(bool verify);
		virtual void verify_ssl_host(bool verify);
		virtual void set_server_cert(const std::string path);
		virtual void enable_tls_connection();
		virtual void prepare();
		virtual int send_mail(const std::string body, bool isHtml);
		//virtual int test_mail(const std::string body, bool isHtml);
		virtual const char* get_last_error();
		virtual ~smtp_request();
	};
};

#endif//!_smtp_client_h

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
#	include <curl/curl.h>
#	include <string>
#	include <vector>
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
		bool init();
		void log(const char* format, const char* str);
		void set_error(const char* error);
	public:
		explicit smtp_request();
		//creates a deep copy of the source smtp_request.
		explicit smtp_request(const smtp_request& that);
		bool has_error()const;
		void host(const char* host);
		void credentials(const char* user, const char* password);
		void http_auth(bool is_http_auth);
		void add_attachment(
			const char* name,
			const char* mime_type,
			const char* path,
			const char* encoder
		);
		void set_date_header();
		void set_message_id(const char* mail_domain);
		void mail_from(const char* from);
		void mail_to(const char* to);
		void mail_cc(const char* cc);
		void mail_bcc(const char* bcc);
		void mail_subject(const char* subject);
		void read_debug_information(bool isDebug);
		void verify_ssl(bool verify);
		void verify_ssl_host(bool verify);
		void set_server_cert(const char* path);
		void enable_tls_connection();
		void prepare();
		int send_mail(const char* body, bool isHtml);
		//int test_mail(const std::string body, bool isHtml);
		const char* get_last_error()const;
		virtual ~smtp_request();
	};
};

#endif//!_smtp_client_h

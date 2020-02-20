/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "smtp_client.h"
#	include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#	include <stdlib.h>// exit, atoi, malloc, free
#	include <string>// !_XSTRING_// memcpy, memset
#	include <iostream>
#	include <web_jsx/web_jsx.h>
#	include <sstream>
#	include <regex>
#	include <ctime>
#	include "curl_util.h"
namespace smtp_client {
#pragma region Help
	bool _sr = false;
	void onr_resource_free() {
		///We will no longer be needing curl funcionality
		curl_global_cleanup();
	}
	void date_time_now(std::string& ret) {
		time_t t;
		struct tm* tm;
		ret.resize(32);//RFC5322_TIME_LEN
		time(&t);
		tm = localtime(&t);
		strftime(&ret[0], 32, "%a, %d %b %Y %H:%M:%S %z", tm);
		return;
	}
	void generate_message_id(std::string& ret) {
		//const int MESSAGE_ID_LEN = 37;
		time_t t;
		struct tm tm;
		//std::string ret;
		ret.resize(15);
		time(&t);
		gmtime_s(&tm, &t);
		strftime(const_cast<char*>(ret.c_str()), 37, "%Y%m%d%H%M%S.", &tm);
		ret.reserve(37);
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		while (ret.size() < 37) {
			ret += alphanum[rand() % (sizeof(alphanum) - 1)];
		}
		return;
	}
#pragma endregion
#pragma region CURL_SMTP
	smtp_request::smtp_request() {
		_is_debug = false;
		_disposed = false; _has_error = this->init();
		if (_has_error)return;
		_internal_error = NULL;
		_recipients = NULL;
#if defined(FAST_CGI_APP)
		if (_sr == false) {
			_sr = true;
			sow_web_jsx::register_resource(onr_resource_free);
		}
#endif//FAST_CGI_APP
		_cheaders = NULL;
		_headers = new std::vector<std::string>();
		_attachments = new std::vector<mail_attachment*>();
	}
	//creates a deep copy of the source smtp_request.
	smtp_request::smtp_request(const smtp_request& that){
		throw std::runtime_error("SMTP instance copy doesn't supported...");
	}
	bool smtp_request::has_error()const {
		return _has_error;
	}
	bool smtp_request::init() {
		if (_has_error == true)return true;
		if (_curl) return false;
		_curl = curl_easy_init();
		if (!_curl) {
			this->set_error("Unable to initialized CURL");
			return true;
		}
		return false;
	}
	void smtp_request::log(const char* format, const char* str){
		if (!_is_debug)return;
		fprintf(stderr, format, str);
		fputc('\n', stderr);//New Line
	}
	void smtp_request::host(const char* host) {
		if (this->init())return;
		/*
			Pass in a pointer to the URL to work with. The parameter should be
			a char * to a zero terminated string which must be URL-encoded in the
			following format:
			scheme://host:port/path
			For a greater explanation of the format please see RFC 3986.
			This is the URL for your mailserver
		*/
		curl_easy_setopt(_curl, CURLOPT_URL, host);
		/*
			Pass a long. If the value is 1, curl will send the initial response to
			the server in the first authentication packet in order to reduce the
			number of ping pong requests. Only applicable to the following supporting
			SASL authentication mechanisms:
			* Login * Plain * GSSAPI * NTLM * OAuth 2.0
			Note: Whilst IMAP supports this option there is no need to explicitly set it,
			as libcurl can determine the feature itself when the server supports the
			SASL-IR CAPABILITY.
		*/
		curl_easy_setopt(_curl, CURLOPT_SASL_IR, 1L);
		log("Host: %s", host);
	}
	void smtp_request::credentials(const char* user, const char* password) {
		if (this->init())return;
		/*
			Pass a char * as parameter, which should be pointing to the zero terminated
			user name to use for the transfer.
			Set username and password
		*/
		curl_easy_setopt(_curl, CURLOPT_USERNAME, user);
		/*
			Pass a char * as parameter, which should be pointing to the zero terminated
			password to use for the transfer.
		*/
		curl_easy_setopt(_curl, CURLOPT_PASSWORD, password);
		log("credentials->User: %s", user);
		log("credentials->Pwd: %s", password);
	}
	void smtp_request::http_auth(bool is_http_auth) {
		if (this->init())return;
		if (is_http_auth) {
			curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_NONE);
		}
		else {
			curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		}
		log("http_auth: %s", is_http_auth == true ? "TRUE" : "FALSE");
	}
	void smtp_request::set_date_header() {
		if (this->init())return;
		std::string msgDate("");
		date_time_now(msgDate);
		//Date: Tue, 22 Aug 2017 14:08:43 +0100
		//_headers = curl_slist_append(_headers, ("Date: " + msgDate).c_str());
		msgDate = ("Date: " + msgDate).c_str();
		_headers->push_back(msgDate.c_str());
		//log("Date: %s", msgDate.c_str());
		msgDate.clear();
	}
	void smtp_request::set_message_id(const char* mail_domain) {
		if (this->init())return;
		std::string str("");
		generate_message_id(str);
		str.append("@");
		str = ("Message-ID: ") + str;
		//_headers->push_back(str.c_str());
		//str.clear();
		str.append(mail_domain);
		_headers->push_back(str.c_str());
		//log("%s", _message_id.c_str());
	}
	void smtp_request::add_attachment(
		const char* name,
		const char* mime_type,
		const char* path,
		const char* encoder
	) {
		if (strlen(name) == 0
			|| strlen(mime_type) == 0
			|| strlen(path) == 0
			) {
			this->set_error("Attachment name or mim_type or path should not left blank...");
			return;
		}
		if (this->init())return;
		mail_attachment* ma = new mail_attachment();//(mail_attachment*)malloc(sizeof mail_attachment);
		ma->name = name;
		ma->path = path;
		ma->mime_type = mime_type;
		ma->encoder = encoder;
		_attachments->push_back(ma);
	}
	void smtp_request::mail_from(const char* from) {
		if (this->init())return;
		/* Note that this option isn't strictly required, omitting it will result
		* in libcurl sending the MAIL FROM command with empty sender data. All
		* autoresponses should have an empty reverse-path, and should be directed
		* to the address in the reverse-path which triggered them. Otherwise,
		* they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
		* details.
		*/
		std::string str(from);
		curl_easy_setopt(_curl, CURLOPT_MAIL_FROM, str.c_str());
		_headers->push_back(("From: " + str).c_str());
		str.clear();
	}
	void smtp_request::mail_to(const char* to) {
		if (this->init())return;
		std::string str(to);
		_recipients = curl_slist_append(_recipients, str.c_str());
		_headers->push_back(("To: " + str).c_str());
		str.clear();
	}
	void smtp_request::mail_cc(const char* cc) {
		if (this->init())return;
		std::regex regx("(?:\\r\\n|\\n|\\r)");
		std::istringstream reader(std::string(cc).c_str());
		std::string str;
		while (std::getline(reader, str, ',')) {
			str = std::regex_replace(str, regx, "");
			if (str.empty() || str.size() == std::string::npos)continue;
			_recipients = curl_slist_append(_recipients, str.c_str());
			_headers->push_back(("Cc: " + str).c_str());
			str.clear();
		}
		std::istringstream().swap(reader);
	}
	void smtp_request::mail_bcc(const char* bcc) {
		if (this->init())return;
		std::regex regx("(?:\\r\\n|\\n|\\r)");
		std::istringstream reader(std::string(bcc).c_str());
		std::string str;
		while (std::getline(reader, str, ',')) {
			str = std::regex_replace(str, regx, "");
			if (str.empty() || str.size() == std::string::npos)continue;
			_recipients = curl_slist_append(_recipients, str.c_str());
			_headers->push_back(("Bcc: " + str).c_str());
			str.clear();
		}
		std::istringstream().swap(reader);
	}
	void smtp_request::mail_subject(const char* subject){
		if (this->init())return;
		std::string str(subject);
		_headers->push_back(("Subject: " + str).c_str());
		str.clear();
	}
	void smtp_request::read_debug_information(bool isDebug){
		/* ask libcurl to show us the verbose output */
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, isDebug == true ? 1L : 0L);
		if (isDebug == true) {
			/* if CURLOPT_VERBOSE is enabled then CURLOPT_DEBUGFUNCTION will be work*/
			curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, sow_web_jsx::debug_log);
		}
		_is_debug = isDebug;
	}
	void smtp_request::verify_ssl(bool verify){
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, verify == true ? 1L : 0L);
		curl_easy_setopt(_curl, CURLOPT_SSL_CTX_FUNCTION, *sow_web_jsx::ssl_ctx_callback);
		log("verify_ssl: %s", verify == true ? "TRUE" : "FALSE");
	}
	void smtp_request::verify_ssl_host(bool verify){
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, verify == true ? 1L : 0L);
		log("verify_ssl_host: %s", verify == true ? "TRUE" : "FALSE");
	}
	void smtp_request::set_server_cert(const char* path){
		/* If your server doesn't have a valid certificate, then you can disable
		* part of the Transport Layer Security protection by setting the
		* CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
		*   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		*   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		* That is, in general, a bad idea. It is still better than sending your
		* authentication details in plain text though.  Instead, you should get
		* the issuer certificate (or the host certificate if the certificate is
		* self-signed) and add it to the set of certificates that are known to
		* libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
		* for more information. */
		curl_easy_setopt(_curl, CURLOPT_CAINFO, path);
		log("CERT: %s", path);
	}
	void smtp_request::enable_tls_connection() {
		curl_easy_setopt(_curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		curl_easy_setopt(_curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
		log("TLS: %s", "TRUE");
	}
	void smtp_request::prepare() {
		if (this->init())return;
		curl_easy_setopt(_curl, CURLOPT_MAIL_RCPT, _recipients);
		/* Build and set the message header list. */
		_cheaders = NULL;
		if (_headers->size() > 0) {
			for (std::vector<std::string>::iterator t = _headers->begin(); t != _headers->end(); ++t) {
				std::string str = *t;
				_cheaders = curl_slist_append(_cheaders, str.c_str());
				log("%s", str.c_str());
			}
			//_cheaders = curl_slist_append(_cheaders, NULL);
			curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _cheaders);
			return;
		}
		this->set_error("No mail header found....");
	}
	int smtp_request::send_mail(const char* body, bool isHtml) {
		if (this->init())return -1;
		_has_error = false;
		curl_mime* alt = curl_mime_init(_curl);
		curl_mimepart* part;
		curl_mime* mime;
		if (_is_debug)
			log("Body: %s", body);
		part = curl_mime_addpart(alt);
		curl_mime_data(part, body, CURL_ZERO_TERMINATED);
		if (isHtml == true) {
			curl_mime_type(part, "text/html");
		}
		/*
			curl_mime_type sets a mime part's content type.
			part is the part's handle to assign the content type to.
			mimetype points to the nul-terminated file mime type string;
			it may be set to NULL to remove a previously attached mime type.
			The mime type string is copied into the part, thus the associated
			storage may safely be released or reused after call. Setting a part's
			type twice is valid: only the value set by the last call is retained.
			In the absence of a mime type and if needed by the protocol specifications,
			a default mime type is determined by the context:
			- If set as a custom header, use this value.
			- application/form-data for an HTTP form post.
			- If a remote file name is set, the mime type is taken from the file name extension, or application/octet-stream by default.
			- For a multipart part, multipart/mixed.
			- text/plain in other cases.
			create a mime handle
		*/
		mime = curl_mime_init(_curl);
		/*
			https://curl.haxx.se/libcurl/c/curl_mime_addpart.html
			curl_mime_addpart creates and appends a new empty part to the given
			mime structure and returns a handle to it. The returned part handle
			can subsequently be populated using functions from the mime API.
			mime is the handle of the mime structure in which the new part
			must be appended.
		*/
		/* Create the inline part. */
		part = curl_mime_addpart(mime);
		curl_mime_subparts(part, alt);
		curl_mime_type(part, "multipart/alternative");
		struct curl_slist* slist = curl_slist_append(NULL, "Content-Disposition: inline");
		curl_mime_headers(part, slist, 1);
		/* Trying to Add attachment. */
		if (_attachments->size() > 0) {
			for (std::vector<mail_attachment*>::iterator t = _attachments->begin(); t != _attachments->end(); ++t) {
				mail_attachment* ma = *t;
				/* Build the mime message. */
				part = curl_mime_addpart(mime);
				/* get data from this file */
				curl_mime_filedata(part, ma->path.c_str());
				/* content-type for this part */
				curl_mime_type(part, ma->mime_type.c_str());
				/* set name */
				curl_mime_name(part, ma->name.c_str());
				/*
					https://curl.haxx.se/libcurl/c/curl_mime_encoder.html
					Supported encoding schemes are:
					"binary": the data is left unchanged, the header is added.
					"8bit": header added, no data change.
					"7bit": the data is unchanged, but is each byte is checked to
					be a 7-bit value; if not, a read error occurs.
					"base64": Data is converted to base64 encoding, then split
					in CRLF-terminated lines of at most 76 characters.
					"quoted-printable": data is encoded in quoted printable
					lines of at most 76 characters. Since the resulting size of
					the final data cannot be determined prior to reading the
					original data, it is left as unknown, causing chunked
					transfer in HTTP. For the same reason, this encoder may
					not be used with IMAP. This encoder targets text data that
					is mostly ASCII and should not be used with other types of data.
				*/
				/* encode file data in base64 for transfer */
				if (ma->encoder.empty()) {
					curl_mime_encoder(part, "base64");
				}
				else {
					curl_mime_encoder(part, ma->encoder.c_str());
				}
				/* Log*/
				log("Add attachment %s", ma->path.c_str());
				//delete ma;
			}
		}
		curl_easy_setopt(_curl, CURLOPT_MIMEPOST, mime);
		/* Send the message */
		CURLcode res = curl_easy_perform(_curl);
		int rec = 0;
		if (res != CURLE_OK) {
			this->set_error(curl_easy_strerror(res));
			rec = -1;
		}
		/* Free lists. */
		curl_slist_free_all(_recipients);
		curl_slist_free_all(_cheaders);
		/* curl won't send the QUIT command until you call cleanup, so you should
		 * be able to re-use this connection for additional messages (setting
		 * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
		 * curl_easy_perform() again. It may not be a good idea to keep the
		 * connection open for a very long time though (more than a few minutes
		 * may result in the server timing out the connection), and you do want to
		 * clean up in the end.
		 */
		curl_easy_cleanup(_curl);
		/* Free multipart message. */
		curl_mime_free(mime);
		_headers->clear(); delete _headers; _headers = NULL;
		_attachments->clear(); delete _attachments; _attachments = NULL;
		_curl = NULL; _recipients = NULL;
		_cheaders = NULL; mime = NULL;
		return rec;
	}
	const char* smtp_request::get_last_error()const {
		if (_internal_error == NULL)return "No error found in current context.";
		return const_cast<const char*>(_internal_error);
	}
	void smtp_request::set_error(const char* error) {
		_free_char(_internal_error);
		size_t len = strlen(error);
		_internal_error = new char[len + 1];
		strcpy_s(_internal_error, len, error);
		_has_error = true;
	}
	smtp_request::~smtp_request() {
		_free_char(_internal_error);
		_free_obj(_headers); _free_obj(_attachments);
		if (_recipients != NULL) {
			curl_slist_free_all(_recipients);
			_recipients = NULL;
		}
		if (_cheaders != NULL) {
			curl_slist_free_all(_cheaders);
			_cheaders = NULL;
		}
		if (_curl != NULL) {
			curl_easy_cleanup(_curl);
			_curl = NULL;
		}
	}
#pragma endregion
}
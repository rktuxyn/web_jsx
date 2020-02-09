//5:40 AM 12/2/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//Read more https://curl.haxx.se/libcurl/c/https.html
#	include "http_request.h"
using namespace http_client;
#if defined(FAST_CGI_APP)
bool _sr = false;
void onr_resource_free() {
	///We will no longer be needing curl funcionality
	curl_global_cleanup();
}
#endif//!FAST_CGI_APP
http_request::http_request(const char * full_url, bool follow_location = false) {
	_disposed = false;
	_curl = curl_easy_init();
	_header_chunk = NULL;
	_full_url = full_url;
	_follow_location = follow_location;
	_internal_error = NULL;
#if defined(FAST_CGI_APP)
	if (_sr == false) {
		_sr = true;
		sow_web_jsx::register_resource(onr_resource_free);
	}
#endif//FAST_CGI_APP
}
void http_request::set_error(const char * error) {
	if (_internal_error != NULL) {
		delete[] _internal_error;
		_internal_error = NULL;
	}
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	/*_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);*/
}
size_t write_callback(char *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
void http_request::prepare() {
	//CURLcode
	if (_header_chunk)
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_chunk);
	/*if (_cookie_chunk)
		curl_easy_setopt(_curl, CURLOPT_COOKIE, _cookie_chunk);*/
	if (_follow_location == true) {
		curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
	}
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(_curl, CURLOPT_URL, _full_url);
	/* complete within 20 seconds */
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 120L);
	curl_easy_setopt(_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
	curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, "gzip");
}
void http_request::verify_ssl(bool verify) {
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
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, verify);

	curl_easy_setopt(_curl, CURLOPT_SSL_CTX_FUNCTION, *sow_web_jsx::ssl_ctx_callback);
}
void http_request::verify_ssl_host(bool verify) {
	/*
	* If the site you're connecting to uses a different host name that what
	* they have mentioned in their server certificate's commonName (or
	* subjectAltName) fields, libcurl will refuse to connect. You can skip
	* this check, but this will make the connection less secure.
	*/
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, verify);
}
void http_request::read_debug_information(bool isDebug) {
	/* ask libcurl to show us the verbose output */
	curl_easy_setopt(_curl, CURLOPT_VERBOSE, isDebug);
	if (isDebug == true) {
		/* if CURLOPT_VERBOSE is enabled then CURLOPT_DEBUGFUNCTION will be work*/
		curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, sow_web_jsx::debug_log);
	}
}
int http_request::send_request(std::string & response_header, std::string &response_body) {
	this->prepare();
	curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &response_header);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response_body);
	CURLcode res = curl_easy_perform(_curl);
	int rec = 0;
	if (res != CURLE_OK) {
		this->set_error(curl_easy_strerror(res));
		//fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		rec = -1;
	}
	curl_easy_cleanup(_curl);
#if !defined(FAST_CGI_APP)
	curl_global_cleanup();
#endif//!FAST_CGI_APP
	return rec;
}
http_request::~http_request() {
	if (_header_chunk != NULL) {
		delete _header_chunk; _header_chunk = NULL;
	}
	if (_internal_error != NULL) {
		delete[] _internal_error;
		_internal_error = NULL;
	}
}
const char * http_request::get_last_error() {
	return const_cast<const char*>(_internal_error);
}
void http_request::set_header(const char * header) {
	_header_chunk = curl_slist_append(_header_chunk, header);
}
void http_request::set_cookie(const char * cookie) {
	curl_easy_setopt(_curl, CURLOPT_COOKIE, cookie);
}
int http_request::get(std::string & response_header, std::string &response_body) {
	return this->send_request(response_header, response_body);
}
int http_request::post(const char * body, std::string & response_header, std::string &response_body) {
	//curl_easy_setopt(_curl, CURLOPT_POST, 1);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body);
	return this->send_request(response_header, response_body);
}

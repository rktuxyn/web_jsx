//5:40 AM 12/2/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "http_request.h"
using namespace http_client;
void http_request::set_error(const char * error) {
	delete _internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
};
size_t write_callback(char *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
};

void http_request::prepare() {
	//CURLcode
	if (_header_chunk)
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_chunk);
	/*if (_cookie_chunk)
		curl_easy_setopt(_curl, CURLOPT_COOKIE, _cookie_chunk);*/
	if (_follow_location == true)
		curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(_curl, CURLOPT_URL, _full_url);
	/* complete within 20 seconds */
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 120L);
	curl_easy_setopt(_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
	curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, "gzip");
	//curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, "");
	//curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
	
};
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
	return rec;
};
http_request::http_request(const char * full_url, bool follow_location = false) {
	_disposed = false;
	_curl = curl_easy_init();
	_header_chunk = NULL;
	_full_url = full_url;
	_follow_location = follow_location;
	_internal_error = new char[1] {'s'};
};
http_request::~http_request() {
	delete _header_chunk;
	delete _internal_error;
};
const char * http_request::get_last_error() {
	return const_cast<const char*>(this->_internal_error);
};
void http_request::set_header(const char * header) {
	_header_chunk = curl_slist_append(_header_chunk, header);
};
void http_request::set_cookie(const char * cookie) {
	curl_easy_setopt(_curl, CURLOPT_COOKIE, cookie);
};
int http_request::get(std::string & response_header, std::string &response_body) {
	return this->send_request(response_header, response_body);
};
int http_request::post(const char * body, std::string & response_header, std::string &response_body) {
	//curl_easy_setopt(_curl, CURLOPT_POST, 1);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body);
	return this->send_request(response_header, response_body);
};

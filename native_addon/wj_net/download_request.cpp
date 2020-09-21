/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:03 PM 4/8/2020
#	include "download_request.h"
#	include <stdio.h>
#	include <stdlib.h>
#	include <iostream>
#	include <fstream>
#	include <web_jsx/web_jsx.h>
#	include "curl_util.h"
#	include <windows.h>
#	include <math.h>
using namespace http_client;

size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	if (stream == NULL)return 0;
	size_t ret = fwrite(ptr, size, nmemb, stream);
	fflush(stream);
	return ret;
}
#if (defined(_WIN32)||defined(_WIN64))
int nb_bar;
double last_progress, progress_bar_adv;
HANDLE _hConsole = NULL;
int progress_bar(void* bar, double t, double d){
	if (last_progress != round(d / t * 100)){
		nb_bar = 25;
		progress_bar_adv = round(d / t * nb_bar);
		std::cout << "\r ";
		SetConsoleTextAttribute(_hConsole, 160);
		std::cout << " Progress : [ ";
		if (round(d / t * 100) < 10){
			std::cout << "0" << round(d / t * 100) << " %]";
		}
		else{
			std::cout << round(d / t * 100) << " %] ";
		}
		SetConsoleTextAttribute(_hConsole, 15);
		std::cout << " [";
		SetConsoleTextAttribute(_hConsole, 10);
		for (int i = 0; i <= progress_bar_adv; i++){
			std::cout << "#";
		}
		SetConsoleTextAttribute(_hConsole, 15);
		for (int i = 0; i < nb_bar - progress_bar_adv; i++){
			std::cout << ".";
		}
		std::cout << "]";
		last_progress = round(d / t * 100);
	}
	return 0;
}
int set_console_attribute(int attribute) {
	if (_hConsole == NULL) {
		_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	return SetConsoleTextAttribute(_hConsole, (WORD)attribute);
}
#else
int progress_bar(void* bar, double t, double d) {
#error NOT IMPLIMENTED
}
int set_console_attribute(int attribute) {
#error NOT IMPLIMENTED
}
#endif//!_WIN32||_WIN64
http_download::http_download(const char* full_url){
	_disposed = FALSE; _is_debug = FALSE;
	_curl = curl_easy_init();
	_header_chunk = NULL;
	_internal_error = NULL;
	_full_url = full_url;
}
http_download::~http_download(){
	if (_header_chunk != NULL) {
		curl_slist_free_all(_header_chunk); _header_chunk = NULL;
	}
	_free_char(_internal_error);
}
void http_download::set_error(const char* error) {
	_free_char(_internal_error);
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
}
void http_download::prepare() {
	if (_disposed == TRUE)return;
	_disposed = TRUE;
	if (_header_chunk)
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_chunk);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, ::write_callback);
	curl_easy_setopt(_curl, CURLOPT_URL, _full_url);
	curl_easy_setopt(_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
	curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, "gzip");
}

void http_download::verify_ssl(bool verify) {
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
void http_download::verify_ssl_host(bool verify) {
	/*
	* If the site you're connecting to uses a different host name that what
	* they have mentioned in their server certificate's commonName (or
	* subjectAltName) fields, libcurl will refuse to connect. You can skip
	* this check, but this will make the connection less secure.
	*/
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, verify);
}
int http_download::download(const char* out_file, int show_progress) {
	this->prepare();
	FILE* fstream;
	errno_t err = fopen_s(&fstream, out_file, "wb");
	if (err != 0 || fstream == NULL) {
		_NEW_STRA(error_str, "Unable to open file in #");
		error_str->append(out_file);
		this->set_error(error_str->c_str());
		_free_obj(error_str);
		return -1;
	}
	//curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &response_header);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fstream);
	if (show_progress == TRUE) {
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0L);
		//progress_bar : the fonction for the progress bar
		curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, ::progress_bar);
		::set_console_attribute(11);
		std::cout << " Start download..." << std::endl << std::endl;
	}
	else {
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 1L);
	}
	CURLcode res = curl_easy_perform(_curl);
	fclose(fstream);
	int rec = 0;
	if (res == CURLE_OK) {
		if (show_progress == TRUE) {
			set_console_attribute(10);
			std::cout << std::endl << std::endl << " The file was download with succes" << std::endl;
		}
	}
	else {
		rec = -1;
		this->set_error(curl_easy_strerror(res));
		if (show_progress == TRUE) {
			set_console_attribute(4);
			std::cout << std::endl << std::endl << " Error" << std::endl;
		}
	}
	curl_easy_cleanup(_curl);
	if (_is_debug == TRUE) {
		set_console_attribute(11);
	}
	return rec;
}
void http_download::read_debug_information(bool isDebug) {
	/* ask libcurl to show us the verbose output */
	curl_easy_setopt(_curl, CURLOPT_VERBOSE, isDebug);
	if (isDebug == true) {
		/* if CURLOPT_VERBOSE is enabled then CURLOPT_DEBUGFUNCTION will be work*/
		curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, sow_web_jsx::debug_log);
		_is_debug = TRUE;
	}
}

const char* http_download::get_last_error() {
	return const_cast<const char*>(_internal_error);
}
void http_download::set_header(const char* header) {
	_header_chunk = curl_slist_append(_header_chunk, header);
}
void http_download::set_cookie(const char* cookie) {
	curl_easy_setopt(_curl, CURLOPT_COOKIE, cookie);
}
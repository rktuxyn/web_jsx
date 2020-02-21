/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:06 PM 2/21/2020
#include "wjsx_env.h"

#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj

#if !defined(WJMT)
void wjsx_env::create_env() {
	body = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	_headers = new std::map<std::string, std::string>();
	_cookies = new std::vector<std::string>();
	_http_status = new std::vector<std::string>();
	_is_flush = FALSE; _is_disposed = FALSE;
}

int wjsx_env::re_create_env(){
	if (_is_disposed == FALSE)this->free();
	this->create_env();
	return TRUE;
}

wjsx_env::wjsx_env(){
	create_env();
}
#endif//!WJMT

int wjsx_env::is_flush(){
	return _is_flush;
}

void wjsx_env::set_flush(){
	_is_flush = TRUE;
}

std::map<std::string, std::string>& wjsx_env::get_header(){
	return *_headers;
}

std::vector<std::string>& wjsx_env::get_cookies(){
	return *_cookies;
}

std::vector<std::string>& wjsx_env::get_http_status(){
	return *_http_status;
}

void wjsx_env::free(){
	if (_is_disposed == TRUE)return;
	_is_disposed = TRUE;
	_free_obj(_headers);
	_free_obj(_cookies);
	_free_obj(_http_status);
}

wjsx_env::~wjsx_env(){
	this->free();
}

/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:00 PM 2/21/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined( _wjsx_env_h)
#	define _wjsx_env_h
#	include <map>
#	include <vector>
#	include <string>
#	include <sstream>

#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE

#if defined(WJMT)
	#if !defined(_WCOUT)
	#	define _WCOUT wj_env->cout
	#endif//!_WCOUT
	#if !defined(_WCERR)
	#	define _WCERR wj_env->cerr
	#endif//!_WCERR
	#if !defined(_WCIN)
	#	define _WCIN wj_env->cin
	#endif//!_WCIN
#else
	#if !defined(_WCOUT)
	#	define _WCOUT std::cout
	#endif//!_WCOUT
	#if !defined(_WCERR)
	#	define _WCERR std::cerr
	#endif//!_WCERR
	#if !defined(_WCIN)
	#	define _WCIN std::cin
	#endif//!_WCIN
#endif//!WJMT

class wjsx_env {
private:
	int _is_flush;
	int _is_disposed;
	std::map<std::string, std::string>* _headers;
	std::vector<std::string>* _cookies;
	std::vector<std::string>* _http_status;
#if defined(WJMT)
	template<class _istream, class _ostream>
	void create_env(_istream& in, _ostream& out, _ostream& err) {
		body = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		_headers = new std::map<std::string, std::string>();
		_cookies = new std::vector<std::string>();
		_http_status = new std::vector<std::string>();
		_is_flush = FALSE; _is_disposed = FALSE;
		cin = in; cout = out; cerr = err;
	}
#else
	void create_env();
	int re_create_env();
#endif//!WJMT
public:
#if defined(WJMT)
	template<class _istream, class _ostream>
	explicit wjsx_env(_istream& in, _ostream& out, _ostream& err) {
		this->create_env(in, out, err);
	}
	template<class _istream, class _ostream>
	int re_create_env(_istream& in, _ostream& out, _ostream& err) {
		if (_is_disposed == FALSE)this->free();
		this->create_env(in, out, err);
		return TRUE;
	}
	std::ostream cout;
	std::istream cin;
	std::ostream cerr;
#else
	explicit wjsx_env();
#endif//!WJMT
	wjsx_env(const wjsx_env&) = delete;
	wjsx_env& operator=(const wjsx_env&) = delete;
	std::stringstream body;
	int is_flush();
	void set_flush();
	std::map<std::string, std::string>& get_header();
	std::vector<std::string>& get_cookies();
	std::vector<std::string>& get_http_status();
	void free();
	~wjsx_env();
};
#endif//!_wjsx_env_h
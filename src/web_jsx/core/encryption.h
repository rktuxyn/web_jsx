/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_encryption_h)
#pragma warning(disable : 4996)
#define _encryption_h
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif//_web_jsx_global_h
#if defined(__client_build)
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
#if !defined(_UNORDERED_MAP_)
#include <unordered_map>
#endif//!_UNORDERED_MAP_
#if !defined(_ALGORITHM_)
#include <algorithm>
#endif//!_ALGORITHM_
//https://codereview.stackexchange.com/questions/198226/simple-encryption-for-strings?newreg=ae31ff1ca51a4a5bbdcb2aed1bb6c705
//template class jsx_export std::unordered_map<char, char>;
class Cipher {
private:
	std::unordered_map<char, char> map;
public:
	explicit Cipher(int n);
	std::string encrypt(std::string) const;
	~Cipher();
private:
	static std::unordered_map<char, char> make_map(int n);
};
#endif//!_encryption_h
#endif//!__client_build
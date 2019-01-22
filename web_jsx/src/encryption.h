#pragma once
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
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
#	define _encryption_h
#	include <string>
#	include <unordered_map>
//https://codereview.stackexchange.com/questions/198226/simple-encryption-for-strings?newreg=ae31ff1ca51a4a5bbdcb2aed1bb6c705
class Cipher {
private:
	std::unordered_map<char, char>* map;
public:
	explicit Cipher(int n);
	void encrypt_decrypt(std::string& str_out) const;
	void clear();
	~Cipher();
private:
	static std::unordered_map<char, char>* make_map(int n);
};
#endif//!WEB_JSX_CLIENT_BUILD
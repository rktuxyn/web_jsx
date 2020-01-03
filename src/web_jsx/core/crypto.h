//3:32 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_crypto_h)
#define _crypto_h
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
#if !defined(_SSTREAM_)
#include <sstream> // std::stringstream
#endif//_SSTREAM_
//https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
namespace crypto {
	void string_to_hex(const std::string& input, std::string&output);
	void hex_to_string(const std::string& input, std::string&output);
	int generate_key_iv(
		std::string& key_str, 
		std::string& iv_str, 
		std::string& error_msg
	);
	int evp_encrypt_decrypt(int should_encrypt,
		std::stringstream& source,
		std::stringstream& dest,
		unsigned char* ckey,
		unsigned char* ivec
	);
	int encrypt(const char*plain_text, const char*key, const char *iv, std::stringstream&dest);
	int decrypt(const char*encrypt_text, const char*key, const char *iv, std::stringstream& dest);
};
#endif//!CRYPTO_H
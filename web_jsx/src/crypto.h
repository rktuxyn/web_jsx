//3:32 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma once
#if !defined(_crypto_h)
#define _crypto_h
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
#if !defined(HEADER_CONF_H)
#include <openssl/conf.h>
#endif//!HEADER_CONF_H
#if !defined(HEADER_ENVELOPE_H)
#include <openssl/evp.h>
#endif//!HEADER_ENVELOPE_H
#if !defined(HEADER_ERR_H)
#include <openssl/err.h>
#endif//!HEADER_ERR_H
#if !defined(HEADER_CONF_H)
#include <openssl/applink.c>
#endif//!HEADER_CONF_H
#include <algorithm>
#include <stdexcept>
#if !defined(jsx_export)
#define jsx_export __declspec(dllexport)
#endif//!HEADER_CONF_H
//https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
namespace crypto {
	int evp_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
		unsigned char *iv, unsigned char *ciphertext);
	int evp_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
		unsigned char *iv, unsigned char *plaintext);
	void string_to_hex(const std::string& input, std::string&output);
	void hex_to_string(const std::string& input, std::string&output);
	int encrypt(const char*plain_text, const char*key, const char *iv, std::string&encrypt_text);
	int decrypt(const char*encrypt_text, const char*key, const char *iv, std::string&plain_text);
};
#endif//!CRYPTO_H
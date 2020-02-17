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
#	define _crypto_h
#	include <string>
#	include <sstream> // std::stringstream
#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj
//https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
namespace crypto {
	int generate_key_iv(
		std::string& key_str, 
		std::string& iv_str, 
		std::string& error_msg
	);
	int encrypt(
		const char*plain_text, 
		const char*key, const char *iv,
		std::stringstream&dest
	);
	int decrypt(
		const char*encrypt_text, 
		const char*key, 
		const char *iv, 
		std::stringstream& dest
	);
	int evp_encrypt_decrypt_file(
		int should_encrypt,
		const char* key,
		const char* iv,
		const char* in_file_path,
		const char* out_file_path,
		std::string& err
	);
};
#endif//!_crypto_h
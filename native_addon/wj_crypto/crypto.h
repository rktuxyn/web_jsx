//3:32 AM 11/30/2018
/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
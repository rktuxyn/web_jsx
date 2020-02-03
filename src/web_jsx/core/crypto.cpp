//3:54 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning (disable : 4996)
#	include "crypto.h"
#	include <openssl/conf.h>
#	include <openssl/evp.h>
#	include <openssl/err.h>
//#	include <openssl/applink.c>
#	include <algorithm>
#	include <stdexcept>
#	include <openssl/rand.h>
#if !defined(_base64_h)
#	include "base64.h"
#endif//!_base64_h
#if !defined(TRUE)
#	define TRUE 1
#endif//!TRUE
#if !defined(FALSE)
#	define FALSE 0
#endif//!FALSE
#	define AES_256_KEY_SIZE 32
#	define AES_BLOCK_SIZE 16
#	define BUFSIZE 1024
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
struct evp_cipher_ctx_st {
	const EVP_CIPHER* cipher;
	ENGINE* engine; /* functional reference if 'cipher' is ENGINE-provided */
	int encrypt;        /* encrypt or decrypt */
	int buf_len;        /* number we have left */
	unsigned char  oiv[EVP_MAX_IV_LENGTH];  /* original iv */
	unsigned char  iv[EVP_MAX_IV_LENGTH];   /* working iv */
	unsigned char buf[EVP_MAX_BLOCK_LENGTH];/* saved partial block */
	int num;                /* used by cfb/ofb/ctr mode */
	void* app_data;     /* application stuff */
	int key_len;        /* May change for variable length cipher */
	unsigned long flags;    /* Various flags */
	void* cipher_data; /* per EVP data */
	int final_used;
	int block_mask;
	unsigned char final[EVP_MAX_BLOCK_LENGTH];/* possible final block */
} /* EVP_CIPHER_CTX */;
//https://stackoverflow.com/questions/3381614/c-convert-string-to-hexadecimal-and-vice-versa
void crypto::string_to_hex(const std::string & input, std::string & output) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return;
}
void crypto::hex_to_string(const std::string & input, std::string & output) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	if (len & 1) throw std::invalid_argument("odd length");
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2) {
		char a = input[i];
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) throw std::invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) throw std::invalid_argument("not a hex digit");
		output.push_back((char)((p - lut) << 4) | (char)(q - lut));
	}
	return;
}
void set_error(std::stringstream& dest, const char* error) {
	dest.clear(); std::stringstream().swap(dest);
	dest << error;
	dest << ERR_error_string(ERR_get_error(), NULL);
	return;
}
int crypto::evp_encrypt_decrypt(
	int should_encrypt,
	std::stringstream& source,
	std::stringstream& dest,
	unsigned char* ckey,
	unsigned char* ivec
) {
	EVP_CIPHER_CTX* ctx;
	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		set_error(dest, "ERROR: EVP_CIPHER_CTX_new failed. OpenSSL error:");
		return FALSE;
	}
	const EVP_CIPHER* cipher_type = EVP_aes_256_cbc();
	/* Don't set key or IV right away; we want to check lengths */
	int rec = EVP_CipherInit_ex(ctx, cipher_type, NULL, NULL, NULL, should_encrypt);
	if (rec < 0) { 
		set_error(dest, "ERROR: EVP_CipherInit_ex failed. OpenSSL error:");
		EVP_CIPHER_CTX_cleanup(ctx);
		return FALSE; 
	}
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == AES_256_KEY_SIZE);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == AES_BLOCK_SIZE);
	/* Now we can set key and IV */
	rec = EVP_CipherInit_ex(ctx, NULL, NULL, ckey, ivec, should_encrypt);
	if (rec < 0) {
		set_error(dest, "ERROR: EVP_CipherInit_ex failed. OpenSSL error:");
		EVP_CIPHER_CTX_cleanup(ctx); 
		return FALSE; 
	}
	int blocksize = EVP_CIPHER_CTX_block_size(ctx);
	source.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = source.tellg();
	std::streamoff utotalSize = totalSize;
	source.seekg(0, std::ios::beg);//Back to begain of stream
	int write_len = 0; std::streamsize n; int read_len = 0;
	int out_len; int cipher_len; int result_len = 0;
	while (true) {
		char* read_buf;
		if (utotalSize >= BUFSIZE) {
			read_len = BUFSIZE;
		}
		else {
			read_len = (int)utotalSize;
		}
		read_len += 1;
		read_buf = new char[read_len];
		read_buf[read_len - 1] = '\0';
		// Read in data in blocks until EOF. Update the ciphering with each read.
		n = source.rdbuf()->sgetn(read_buf, read_len);
		cipher_len = read_len + blocksize;
		cipher_len += 1;
		unsigned char* cipher_buf = new unsigned char[cipher_len];
		cipher_buf[cipher_len - 1] = '\0';
		rec = EVP_CipherUpdate(ctx, cipher_buf, &out_len, reinterpret_cast<unsigned char*>(read_buf), (int)n);
		if (rec < 0) {
			delete[]cipher_buf; delete[]read_buf;
			set_error(dest, "ERROR: EVP_CipherUpdate failed. OpenSSL error:");
			EVP_CIPHER_CTX_cleanup(ctx);
			return FALSE;
		}
		dest.write(reinterpret_cast<const char*>(cipher_buf), out_len);
		delete[]cipher_buf; delete[]read_buf;
		result_len += out_len;
		utotalSize -= n;
		if (utotalSize <= 0)break;//EOF
	}
	cipher_len = BUFSIZE + blocksize;
	// Now cipher the final block and write it out.
	unsigned char* cipher_buf = new unsigned char[cipher_len];
	rec = EVP_CipherFinal(ctx, cipher_buf, &out_len);
	if (rec < 0) {
		delete[] cipher_buf;
		set_error(dest, "ERROR: EVP_CipherFinal failed. OpenSSL error:");
		EVP_CIPHER_CTX_cleanup(ctx);
		return FALSE;
	}
	result_len += out_len;
	dest.write(reinterpret_cast<const char*>(cipher_buf), out_len);
	// Free memory
	delete[] cipher_buf;
	EVP_CIPHER_CTX_cleanup(ctx);
	return result_len;
}
int crypto::generate_key_iv(
	std::string& key_str,
	std::string& iv_str,
	std::string& error_msg
) {
	/* Key to use for encrpytion and decryption */
	unsigned char* key = new unsigned char[AES_256_KEY_SIZE];
	/* Initialization Vector */
	unsigned char* iv = new unsigned char[AES_BLOCK_SIZE];
	/* Generate cryptographically strong pseudo-random bytes for key and IV */
	if (!RAND_bytes(key, AES_256_KEY_SIZE) || !RAND_bytes(iv, AES_BLOCK_SIZE)) {
		/* OpenSSL reports a failure, act accordingly */
		delete[] key; delete[] iv;
		error_msg = "ERROR: RAND_bytes error: ";
		error_msg.append(strerror(errno));
		return FALSE;
	}
	std::string _key, _iv;
	_key.reserve(AES_256_KEY_SIZE);
	_iv.reserve(AES_BLOCK_SIZE);
	_key.append(reinterpret_cast<const char*>(key), AES_256_KEY_SIZE);
	_iv.append(reinterpret_cast<const char*>(iv), AES_BLOCK_SIZE);
	key_str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(_key.c_str()), (int)_key.size());
	iv_str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(_iv.c_str()), (int)_iv.size());
	delete[] key; delete[] iv; _iv.clear(); _key.clear();
	return TRUE;
}
//https://www.openssl.org/docs/man1.0.2/crypto/EVP_EncryptUpdate.html
int crypto::encrypt(
	const char* plain_text,
	const char * key,
	const char * iv,
	std::stringstream& dest
) {
	if (key == NULL || (key != NULL && key[0] == '\0')) {
		dest << "Base64 Key required...";
		return FALSE;
	}
	if (iv == NULL || (iv != NULL && iv[0] == '\0')) {
		dest << "Base64 Iv required...";
		return FALSE;
	}
	std::stringstream source(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	source.write(plain_text, strlen(plain_text));
	char* key_text = strdup(sow_web_jsx::base64::to_decode_str(key).c_str());
	char* iv_text = strdup(sow_web_jsx::base64::to_decode_str(iv).c_str());
	dest.clear();
	int res = evp_encrypt_decrypt(
		TRUE, source, dest,
		reinterpret_cast<unsigned char*>(key_text),
		reinterpret_cast<unsigned char*>(iv_text)
	);
	delete[]key_text; delete[]iv_text;
	source.clear(); std::stringstream().swap(source);
	if (res == FALSE)return FALSE;
	std::string encrypted_str(dest.str().c_str(), res); dest.clear(); std::stringstream().swap(dest);
	std::string base64_str = sow_web_jsx::base64::to_encode_str(reinterpret_cast<const unsigned char*>(encrypted_str.c_str()), (int)encrypted_str.size());
	encrypted_str.clear();
	res = (int)base64_str.size();
	dest.write(base64_str.c_str(), res); base64_str.clear();
	return res;
}
int crypto::decrypt(
	const char* encrypt_text, 
	const char* key, const char* iv, 
	std::stringstream& dest
) {
	if (key == NULL || (key != NULL && key[0] == '\0')) {
		dest << "Base64 Key required...";
		return FALSE;
	}
	if (iv == NULL || (iv != NULL && iv[0] == '\0')) {
		dest << "Base64 Iv required...";
		return FALSE;
	}
	std::stringstream source(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	std::string str = sow_web_jsx::base64::to_decode_str(encrypt_text);
	source.write(str.c_str(), str.size()); str.clear();
	char* key_text = strdup(sow_web_jsx::base64::to_decode_str(key).c_str());
	char* iv_text = strdup(sow_web_jsx::base64::to_decode_str(iv).c_str());
	int res = evp_encrypt_decrypt(
		FALSE, source, dest,
		reinterpret_cast<unsigned char*>(key_text),
		reinterpret_cast<unsigned char*>(iv_text)
	);
	delete[]key_text; delete[]iv_text;
	source.clear(); std::stringstream().swap(source);
	return res;
}
//END 2:18 PM 1/3/2020
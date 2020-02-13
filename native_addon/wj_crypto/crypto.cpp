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
#	include <algorithm>
#	include <stdexcept>
#	include <openssl/rand.h>
#	include <web_jsx/base64.h>
#	include <fstream>
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
//struct evp_cipher_ctx_st {
//	const EVP_CIPHER* cipher;
//	ENGINE* engine; /* functional reference if 'cipher' is ENGINE-provided */
//	int encrypt;        /* encrypt or decrypt */
//	int buf_len;        /* number we have left */
//	unsigned char  oiv[EVP_MAX_IV_LENGTH];  /* original iv */
//	unsigned char  iv[EVP_MAX_IV_LENGTH];   /* working iv */
//	unsigned char buf[EVP_MAX_BLOCK_LENGTH];/* saved partial block */
//	int num;                /* used by cfb/ofb/ctr mode */
//	void* app_data;     /* application stuff */
//	int key_len;        /* May change for variable length cipher */
//	unsigned long flags;    /* Various flags */
//	void* cipher_data; /* per EVP data */
//	int final_used;
//	int block_mask;
//	unsigned char final[EVP_MAX_BLOCK_LENGTH];/* possible final block */
//} /* EVP_CIPHER_CTX */;
void set_error(std::stringstream& dest, const char* error) {
	dest.clear(); std::stringstream().swap(dest);
	dest << error;
	dest << ERR_error_string(ERR_get_error(), NULL);
	return;
}
void set_error(std::string& error_str, const char* cerror) {
	error_str.clear();
	error_str.append(cerror);
	error_str.append(ERR_error_string(ERR_get_error(), NULL));
	return;
}
/*
std::stringstream& source,
std::stringstream& dest,
*/
template<class _source_strm, class _dest_strm>
int evp_encrypt_decrypt(
	int should_encrypt,
	_source_strm& source,
	_dest_strm& dest,
	unsigned char* ckey,
	unsigned char* ivec,
	std::string&error
) {
	EVP_CIPHER_CTX* ctx;
	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		set_error(error, "ERROR: EVP_CIPHER_CTX_new failed. OpenSSL error:");
		return FALSE;
	}
	const EVP_CIPHER* cipher_type = EVP_aes_256_cbc();
	/* Don't set key or IV right away; we want to check lengths */
	int rec = EVP_CipherInit_ex(ctx, cipher_type, NULL, NULL, NULL, should_encrypt);
	if (rec < 0) { 
		set_error(error, "ERROR: EVP_CipherInit_ex failed. OpenSSL error:");
		EVP_CIPHER_CTX_cleanup(ctx);
		return FALSE; 
	}
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == AES_256_KEY_SIZE);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == AES_BLOCK_SIZE);
	/* Now we can set key and IV */
	rec = EVP_CipherInit_ex(ctx, NULL, NULL, ckey, ivec, should_encrypt);
	if (rec < 0) {
		set_error(error, "ERROR: EVP_CipherInit_ex failed. OpenSSL error:");
		EVP_CIPHER_CTX_cleanup(ctx); 
		return FALSE; 
	}
	int blocksize = EVP_CIPHER_CTX_block_size(ctx);
	source.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = source.tellg();
	std::streamoff utotalSize = totalSize;
	source.seekg(0, std::ios::beg);//Back to begain of stream
	//int write_len = 0; //std::streamsize n;
	int out_len; int cipher_len; int result_len = 0;
	do {
		unsigned read_len = 0;
		char* read_buf;
		if (utotalSize >= BUFSIZE) {
			read_len = BUFSIZE;
		}
		else {
			read_len = (int)utotalSize;
		}
		read_len += sizeof(char);
		read_buf = new char[read_len];
		read_buf[read_len - 1] = '\0';
		// Read in data in blocks until EOF. Update the ciphering with each read.
		//n = source.rdbuf()->sgetn(read_buf, read_len);
		source.read(read_buf, read_len);
		cipher_len = read_len + blocksize;
		cipher_len += 1;
		unsigned char* cipher_buf = new unsigned char[cipher_len];
		cipher_buf[cipher_len - 1] = '\0';
		rec = EVP_CipherUpdate(ctx, cipher_buf, &out_len, reinterpret_cast<unsigned char*>(read_buf), (int)read_len);
		if (rec < 0) {
			delete[]cipher_buf; delete[]read_buf;
			set_error(error, "ERROR: EVP_CipherUpdate failed. OpenSSL error:");
			EVP_CIPHER_CTX_cleanup(ctx);
			return FALSE;
		}
		dest.write(reinterpret_cast<const char*>(cipher_buf), out_len);
		delete[]cipher_buf; delete[]read_buf;
		result_len += out_len;
		utotalSize -= read_len;
		//utotalSize -= n;
		//if (utotalSize <= 0)break;//EOF
	} while (!source.eof());
	cipher_len = BUFSIZE + blocksize;
	// Now cipher the final block and write it out.
	unsigned char* cipher_buf = new unsigned char[cipher_len];
	rec = EVP_CipherFinal(ctx, cipher_buf, &out_len);
	if (rec < 0) {
		delete[] cipher_buf;
		set_error(error, "ERROR: EVP_CipherFinal failed. OpenSSL error:");
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
	sow_web_jsx::base64::to_encode_str(_key, key_str);
	sow_web_jsx::base64::to_encode_str(_iv, iv_str);
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
	if (key == NULL || strlen(key) == 0) {
		dest << "Base64 Key required...";
		return FALSE;
	}
	if (iv == NULL || strlen(key) == 0) {
		dest << "Base64 Iv required...";
		return FALSE;
	}
	std::stringstream source(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	source.write(plain_text, strlen(plain_text));
	std::string* _key = new std::string();
	sow_web_jsx::base64::to_decode_str(key, *_key);
	std::string* _iv = new std::string();
	sow_web_jsx::base64::to_decode_str(iv, *_iv);
	dest.clear(); std::string error_str;
	int res = evp_encrypt_decrypt(
		TRUE, source, dest,
		reinterpret_cast<unsigned char*>(_key->data()),
		reinterpret_cast<unsigned char*>(_iv->data()),
		error_str
	);
	_free_obj(_key); _free_obj(_iv);
	source.clear(); std::stringstream().swap(source);
	if (res == FALSE) {
		dest << error_str;
		error_str.clear();
		return FALSE;
	}
	std::string* encrypted_str = new std::string(dest.str().c_str(), res);
	dest.clear(); std::stringstream().swap(dest);
	std::string* base64_str = new std::string();
	sow_web_jsx::base64::to_encode_str(*encrypted_str, *base64_str);
	_free_obj(encrypted_str);
	res = (int)base64_str->size();
	dest.write(base64_str->c_str(), res);
	_free_obj(base64_str);
	return res;
}
int crypto::decrypt(
	const char* encrypt_text, 
	const char* key, const char* iv, 
	std::stringstream& dest
) {
	if (key == NULL || strlen(key) == 0) {
		dest << "Base64 Key required...";
		return FALSE;
	}
	if (iv == NULL || strlen(key) == 0) {
		dest << "Base64 Iv required...";
		return FALSE;
	}
	std::stringstream source(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	std::string* str = new std::string();
	sow_web_jsx::base64::to_decode_str(encrypt_text, *str);
	source.write(str->c_str(), str->size()); _free_obj(str);
	std::string* _key = new std::string();
	sow_web_jsx::base64::to_decode_str(key, *_key);
	std::string* _iv = new std::string();
	sow_web_jsx::base64::to_decode_str(iv, *_iv);
	std::string error_str;
	int res = evp_encrypt_decrypt(
		FALSE, source, dest,
		reinterpret_cast<unsigned char*>(_key->data()),
		reinterpret_cast<unsigned char*>(_iv->data()),
		error_str
	);
	_free_obj(_key); _free_obj(_iv);
	source.clear(); std::stringstream().swap(source);
	if (res == FALSE) {
		dest << error_str;
		error_str.clear();
	}
	return res;
}

int crypto::evp_encrypt_decrypt_file(
	int should_encrypt,
	const char* key,
	const char* iv,
	const char* in_file_path,
	const char* out_file_path,
	std::string&err
) {
	if (key == NULL || strlen(key) == 0) {
		err = "Base64 Key required...";
		return FALSE;
	}
	if (iv == NULL || strlen(key) == 0) {
		err = "Base64 Iv required...";
		return FALSE;
	}
	std::ifstream* source_strem = new std::ifstream(in_file_path, std::ifstream::binary);
	if (!source_strem->is_open()) {
		err = "Unable to open source file #";
		err.append(in_file_path);
		delete source_strem;
		return FALSE;
	}
	std::ofstream* dest_strem = new std::ofstream(out_file_path, std::ofstream::out | std::ofstream::binary);
	if (!dest_strem->is_open()) {
		err = "Unable to create dest file #";
		err.append(out_file_path);
		source_strem->close();
		delete source_strem; delete dest_strem;
		return FALSE;
	}
	std::string* _key = new std::string();
	sow_web_jsx::base64::to_decode_str(key, *_key);
	std::string* _iv = new std::string();
	sow_web_jsx::base64::to_decode_str(iv, *_iv);
	int res = evp_encrypt_decrypt(
		should_encrypt, *source_strem, *dest_strem,
		reinterpret_cast<unsigned char*>(_key->data()),
		reinterpret_cast<unsigned char*>(_iv->data()),
		err
	);
	_free_obj(_key); _free_obj(_iv);
	source_strem->close(); dest_strem->flush(); dest_strem->close();
	delete source_strem; delete dest_strem;
	
	return res;
}
//END 2:18 PM 1/3/2020
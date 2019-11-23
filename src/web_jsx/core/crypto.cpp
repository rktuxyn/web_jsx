//3:54 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "crypto.h"
struct evp_cipher_ctx_st {
	const EVP_CIPHER *cipher;
	ENGINE *engine; /* functional reference if 'cipher' is ENGINE-provided */
	int encrypt;        /* encrypt or decrypt */
	int buf_len;        /* number we have left */
	unsigned char  oiv[EVP_MAX_IV_LENGTH];  /* original iv */
	unsigned char  iv[EVP_MAX_IV_LENGTH];   /* working iv */
	unsigned char buf[EVP_MAX_BLOCK_LENGTH];/* saved partial block */
	int num;                /* used by cfb/ofb/ctr mode */
	void *app_data;     /* application stuff */
	int key_len;        /* May change for variable length cipher */
	unsigned long flags;    /* Various flags */
	void *cipher_data; /* per EVP data */
	int final_used;
	int block_mask;
	unsigned char final[EVP_MAX_BLOCK_LENGTH];/* possible final block */
} /* EVP_CIPHER_CTX */;

void handleErrors(void) {
	ERR_print_errors_fp(stderr);
	//abort();
};
int crypto::evp_encrypt(unsigned char * plaintext, int plaintext_len, unsigned char * key, unsigned char * iv, unsigned char * ciphertext) {
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-2;
	}
	EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-3;
	}
	ciphertext_len = len;
	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-4;
	}
	ciphertext_len += len;
	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);

	return ciphertext_len;
}
int crypto::evp_encrypt(
	unsigned char* plaintext, int plaintext_len,
	unsigned char* key, unsigned char* iv,
	sow_web_jsx::secure_blob* encrypt_text
) {
	EVP_CIPHER_CTX* ctx;

	//int len;

	//int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-2;
	}
	EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	 // First, encrypt the plain_text data
	unsigned int current_size = 0;
	int encrypt_size = 0;
	sow_web_jsx::secure_blob cipher_text(plaintext_len + EVP_MAX_BLOCK_LENGTH);
	if (1 != EVP_EncryptUpdate(ctx, &cipher_text[current_size], &encrypt_size, plaintext, plaintext_len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-3;
	}
	current_size += encrypt_size;
	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	encrypt_size = 0;
	unsigned char* final_buf = NULL;
	if (static_cast<unsigned int>(current_size) < cipher_text.size())
		final_buf = &cipher_text[current_size];
	if (1 != EVP_EncryptFinal_ex(ctx, final_buf, &encrypt_size)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-4;
	}
	current_size += encrypt_size;
	cipher_text.resize(current_size);
	encrypt_text->swap(cipher_text);
	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);
	return current_size;
}

int crypto::evp_decrypt(
	unsigned char* ciphertext, int ciphertext_len,
	unsigned char* key, unsigned char* iv, 
	sow_web_jsx::secure_blob* plaintext
) {
	EVP_CIPHER_CTX* ctx;
	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}
	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-2;
	}

	EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	//unsigned int current_size = 0;
	//int dcrypt_size = 0;
	sow_web_jsx::secure_blob local_plain_text(ciphertext_len);
	if (local_plain_text.size() >
		static_cast<unsigned int>(std::numeric_limits<int>::max())) {
		// EVP_DecryptUpdate takes a signed int
		return -3;
	}
	int final_size = 0;
	int decrypt_size = (int)local_plain_text.size();

	if (1 != EVP_DecryptUpdate(ctx, local_plain_text.data(), &decrypt_size, ciphertext, ciphertext_len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-3;
	}
	final_size += decrypt_size;
	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	unsigned char* final_buf = NULL;
	if (static_cast<unsigned int>(decrypt_size) < local_plain_text.size())
		final_buf = &local_plain_text[decrypt_size];

	if (1 != EVP_DecryptFinal_ex(ctx, final_buf, &decrypt_size)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-4;
	}
	final_size += decrypt_size;
	local_plain_text.resize(final_size);
	plaintext->swap(local_plain_text);
	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);
	return final_size;
}
int evp_encrypt_x(std::stringstream& plaintext, unsigned char * key, unsigned char * iv, std::string&ciphertext) {
	EVP_CIPHER_CTX *ctx;
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}
	
	/*if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		return-1;
	}*/
	if (!EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, 1)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return -2;
	};

	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	if (!EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, 1)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return -3;
	};
	ctx->flags = EVP_PADDING_PKCS7;
	//EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	plaintext.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = plaintext.tellg();
	std::streamoff utotalSize = totalSize;
	plaintext.seekg(0, std::ios::beg);//Back to begain of stream
	int write_len = 0;
	//bool is_first = true;
	std::streamsize n;
	unsigned char outbuf[EBUFF + EVP_MAX_BLOCK_LENGTH];
	unsigned char inbuf[EBUFF];
	int outlen;
	do {
		n = plaintext.rdbuf()->sgetn((char*)inbuf, EBUFF);
		if (n <= 0)break;
		if (!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, (int)n)) {
			/* Error */
			EVP_CIPHER_CTX_cleanup(ctx);
			return -4;
		}
		if (outlen > 0)
			ciphertext.append((char*)outbuf, outlen);
		write_len += outlen;
		totalSize -= n;
		if (totalSize <= 0)break;
	} while (true);
	if (!EVP_CipherFinal_ex(ctx, outbuf, &outlen)) {
		std::string().swap(ciphertext);
		ciphertext = const_cast<const char*>(ERR_error_string(ERR_get_error(), NULL));
		ciphertext.append(";Buff-len==>");
		ciphertext.append(std::to_string(ctx->buf_len));
		EVP_CIPHER_CTX_cleanup(ctx);
		return-5;
	}
	if (outlen > 0)
		ciphertext.append((char*)outbuf, outlen);
	write_len += outlen;
	EVP_CIPHER_CTX_cleanup(ctx);
	return write_len;
};
int evp_decrypt_x(std::stringstream& ciphertext, unsigned char * key, unsigned char * iv, std::string&plaintext) {
	EVP_CIPHER_CTX *ctx;
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}
	/*if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		return-1;
	}*/
	if (!EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, 0)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return -2;
	};

	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
	
	if (!EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, 0)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return -3;
	};
	ctx->flags = EVP_PADDING_PKCS7;//EVP_CIPH_NO_PADDING
	//EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	ciphertext.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = ciphertext.tellg();
	std::streamoff utotalSize = totalSize;
	ciphertext.seekg(0, std::ios::beg);//Back to begain of stream
	int write_len = 0;
	//bool is_first = true;
	std::streamsize n;
	unsigned char outbuf[EBUFF + EVP_MAX_BLOCK_LENGTH];
	unsigned char inbuf[EBUFF];
	int outlen;
	do {
		n = ciphertext.rdbuf()->sgetn((char*)inbuf, EBUFF);
		if (n <= 0)break;
		if (!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, (int)n)) {
			/* Error */
			EVP_CIPHER_CTX_cleanup(ctx);
			return -4;
		}
		if (outlen > 0)
			plaintext.append((char*)outbuf, outlen);
		write_len += outlen;
		totalSize -= n;
		if (totalSize <= 0)break;
	} while (true);
	if (!EVP_CipherFinal_ex(ctx, outbuf, &outlen)) {
		std::string().swap(plaintext);
		plaintext = const_cast<const char*>(ERR_error_string(ERR_get_error(), NULL));
		plaintext.append(";Buff-len==>");
		plaintext.append(std::to_string(ctx->buf_len));
		plaintext.append(";Final-len==>");
		plaintext.append(std::to_string(ctx->final_used));
		EVP_CIPHER_CTX_cleanup(ctx);
		return-5;
	}
	if (outlen > 0)
		plaintext.append((char*)outbuf, outlen);
	write_len += outlen;
	EVP_CIPHER_CTX_cleanup(ctx);
	return write_len;
};
int crypto::evp_decrypt(unsigned char * ciphertext, int ciphertext_len, unsigned char * key, unsigned char * iv, unsigned char * plaintext) {
	EVP_CIPHER_CTX *ctx;
	int len;

	int plaintext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		return-1;
	}
	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-2;
	}

	EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);
	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, (int*)&len, ciphertext, ciphertext_len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-3;
	}
	plaintext_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, (int*)&len)) {
		EVP_CIPHER_CTX_cleanup(ctx);
		return-4;
	}
	plaintext_len += len;
	/* Clean up */
	EVP_CIPHER_CTX_cleanup(ctx);
	return plaintext_len;
};
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
};
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

//https://www.openssl.org/docs/man1.0.2/crypto/EVP_EncryptUpdate.html
int crypto::encrypt(const char* plain_text, const char * key, const char * iv, std::string & encrypt_text) {
	/* Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, dependant on the
	 * algorithm and mode
	 */
	int plain_text_len = (int)strlen ((char *)plain_text);
	char * p_text = (char*)malloc(plain_text_len + 1);
	strcpy(p_text, plain_text);
	char * key_text = (char*)malloc(strlen(key) + 1);
	strcpy(key_text, key);
	char * iv_text = (char*)malloc(strlen(iv) + 1);
	strcpy(iv_text, iv);
	//unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
	// Encrypt the plaintext 
	//unsigned char* ciphertext = (unsigned char*)malloc(plain_text_len + EVP_MAX_BLOCK_LENGTH);
	sow_web_jsx::secure_blob* cipher_text = new sow_web_jsx::secure_blob(plain_text_len + EVP_MAX_BLOCK_LENGTH);
	
	//unsigned char* ciphertext = (unsigned char*)malloc((((plain_text_len + EVP_MAX_BLOCK_LENGTH) / EVP_MAX_BLOCK_LENGTH) * EVP_MAX_BLOCK_LENGTH) + 1);
	//if (ciphertext == NULL) return -10;
	int ciphertext_len = crypto::evp_encrypt (
		reinterpret_cast<unsigned char*>(p_text),
		plain_text_len, reinterpret_cast<unsigned char*>(key_text),
		reinterpret_cast<unsigned char*>(iv_text), cipher_text
	);
	free(p_text); free(key_text); free(iv_text);
	if (ciphertext_len < 0) {
		free(cipher_text);
		encrypt_text = std::to_string(ciphertext_len);
		//encrypt_text.append((const char *)ciphertext, strlen((const char *)ciphertext));
		return ciphertext_len;
	}
	std::string* encrypted_txt = new std::string(cipher_text->char_data());
	string_to_hex(*encrypted_txt, encrypt_text);
	free(encrypted_txt);
	free(cipher_text);
	return ciphertext_len;
	//return decrypt(encrypt_text.c_str(), key, iv, encrypt_text);
};
int crypto::decrypt(const char* encrypt_text, const char* key, const char* iv, std::string& plain_text) {
	/* Buffer for the decrypted text */
	std::string* _encrypt_text = new std::string();
	hex_to_string(encrypt_text, *_encrypt_text);
	const char* enc_txt = _encrypt_text->c_str();
	int encrypt_text_len = (int)strlen((char*)enc_txt);
	char* enc_text = (char*)malloc(encrypt_text_len + 1);
	strcpy(enc_text, enc_txt);
	char* key_text = (char*)malloc(strlen(key) + 1);
	strcpy(key_text, key);
	char* iv_text = (char*)malloc(strlen(iv) + 1);
	strcpy(iv_text, iv);
	sow_web_jsx::secure_blob* decrypted_text = new sow_web_jsx::secure_blob(encrypt_text_len + EVP_MAX_BLOCK_LENGTH);
	new sow_web_jsx::secure_blob(std::string(""));
	int decryptedtext_len = crypto::evp_decrypt(
		reinterpret_cast<unsigned char*>(enc_text),
		encrypt_text_len, reinterpret_cast<unsigned char*>(key_text),
		reinterpret_cast<unsigned char*>(iv_text), decrypted_text
	);
	free(_encrypt_text);
	free(enc_text); free(key_text); free(iv_text);
	if (decryptedtext_len < 0) {
		free(decrypted_text);
		plain_text = std::to_string(decryptedtext_len);
		//plain_text.append((const char *)decryptedtext, strlen((const char *)decryptedtext));
		return decryptedtext_len;
	}
	plain_text = std::string(decrypted_text->char_data());
	free(decrypted_text);
	return decryptedtext_len;
}
//int crypto::decrypt(const char * encrypt_text, const char * key, const char * iv, std::string & plain_text) {
//	/* Buffer for the decrypted text */
//	std::string* _encrypt_text = new std::string();
//	hex_to_string(encrypt_text, *_encrypt_text);
//	/*std::stringstream ss(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
//	ss << _encrypt_text->data();
//	int res = evp_decrypt_x(ss, (unsigned char *)key, (unsigned char *)iv, plain_text);
//	delete _encrypt_text;
//	std::stringstream().swap(ss);
//	return res;*/
//	const char* enc_txt = _encrypt_text->c_str();
//	int encrypt_text_len = (int)strlen ((char *)enc_txt);
//	char * enc_text = (char*)malloc(encrypt_text_len + 1);
//	strcpy(enc_text, enc_txt);
//	char * key_text = (char*)malloc(strlen(key) + 1);
//	strcpy(key_text, key);
//	char * iv_text = (char*)malloc(strlen(iv) + 1);
//	strcpy(iv_text, iv);
//
//	unsigned char*decryptedtext = (unsigned char*)malloc(encrypt_text_len);
//	if (decryptedtext == NULL) return -10;
//	//unsigned char decryptedtext[1024 + EVP_MAX_BLOCK_LENGTH];
//	int decryptedtext_len = crypto::evp_decrypt(
//		reinterpret_cast<unsigned char*>(enc_text),
//		encrypt_text_len, reinterpret_cast<unsigned char*>(key_text),
//		reinterpret_cast<unsigned char*>(iv_text), decryptedtext
//	);
//	free(_encrypt_text);
//	free(enc_text); free(key_text); free(iv_text);
//	if (decryptedtext_len < 0) {
//		free(decryptedtext);
//		plain_text = std::to_string(decryptedtext_len);
//		//plain_text.append((const char *)decryptedtext, strlen((const char *)decryptedtext));
//		return decryptedtext_len;
//	}
//	decryptedtext[decryptedtext_len] = '\0';
//	plain_text = std::string((const char *)decryptedtext, decryptedtext_len);
//	//plain_text.append((const char *)decryptedtext, decryptedtext_len);
//	free(decryptedtext);
//	return decryptedtext_len;
//}

//int crypto::encrypt(const char* plain_text, const char* key, const char* iv, std::string& encrypt_text) {
//	/* Buffer for ciphertext. Ensure the buffer is long enough for the
//	 * ciphertext which may be longer than the plaintext, dependant on the
//	 * algorithm and mode
//	 */
//	int plain_text_len = (int)strlen((char*)plain_text);
//	char* p_text = (char*)malloc(plain_text_len + 1);
//	strcpy(p_text, plain_text);
//	char* key_text = (char*)malloc(strlen(key) + 1);
//	strcpy(key_text, key);
//	char* iv_text = (char*)malloc(strlen(iv) + 1);
//	strcpy(iv_text, iv);
//	//unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
//	// Encrypt the plaintext 
//	//unsigned char* ciphertext = (unsigned char*)malloc(plain_text_len + EVP_MAX_BLOCK_LENGTH);
//	sow_web_jsx::secure_blob* cipher_text = new sow_web_jsx::secure_blob(plain_text_len + EVP_MAX_BLOCK_LENGTH);
//
//	unsigned char* ciphertext = (unsigned char*)malloc((((plain_text_len + EVP_MAX_BLOCK_LENGTH) / EVP_MAX_BLOCK_LENGTH) * EVP_MAX_BLOCK_LENGTH) + 1);
//	if (ciphertext == NULL) return -10;
//	int ciphertext_len = crypto::evp_encrypt(
//		reinterpret_cast<unsigned char*>(p_text),
//		plain_text_len, reinterpret_cast<unsigned char*>(key_text),
//		reinterpret_cast<unsigned char*>(iv_text), cipher_text
//	);
//	free(p_text); free(key_text); free(iv_text);
//	if (ciphertext_len < 0) {
//		free(ciphertext);
//		encrypt_text = std::to_string(ciphertext_len);
//		//encrypt_text.append((const char *)ciphertext, strlen((const char *)ciphertext));
//		return ciphertext_len;
//	}
//	ciphertext[ciphertext_len] = '\0';
//	std::string* encrypted_txt = new std::string((const char*)ciphertext, ciphertext_len);
//	string_to_hex(*encrypted_txt, encrypt_text);
//	free(encrypted_txt);
//	free(ciphertext);
//	return ciphertext_len;
//	//return decrypt(encrypt_text.c_str(), key, iv, encrypt_text);
//};
//END 5:39 AM 11/30/2018
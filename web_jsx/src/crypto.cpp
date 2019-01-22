//3:54 AM 11/30/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "crypto.h"
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
		return-1;
	}

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
		return-1;
	}
	ciphertext_len = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
		return-1;
	}
	ciphertext_len += len;
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
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
		return-1;
	}

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
		return-1;
	}
	plaintext_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
		return-1;
	}
	plaintext_len += len;
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
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
};
//https://www.openssl.org/docs/man1.0.2/crypto/EVP_EncryptUpdate.html
int crypto::encrypt(const char* plain_text, const char * key, const char * iv, std::string & encrypt_text) {
	/* Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, dependant on the
	 * algorithm and mode
	 */
	
	//int plain_text_len = (int)strlen ((char *)plain_text);
	unsigned char ciphertext[1024 + EVP_MAX_BLOCK_LENGTH];
	/* Encrypt the plaintext */
	int ciphertext_len = evp_encrypt ((unsigned char *)plain_text, 
		(int)strlen ((char *)plain_text), (unsigned char *)key, 
		(unsigned char *)iv, ciphertext
	);
	if (ciphertext_len < 0)return -1;
	std::string* encrypted_txt = new std::string((const char *)ciphertext, ciphertext_len);
	string_to_hex(*encrypted_txt, encrypt_text);
	delete encrypted_txt;
	return 1;
};
int crypto::decrypt(const char * encrypt_text, const char * key, const char * iv, std::string & plain_text) {
	/* Buffer for the decrypted text */
	std::string* _encrypt_text = new std::string();
	hex_to_string(encrypt_text, *_encrypt_text);
	const char* enc_txt = _encrypt_text->c_str();
	delete _encrypt_text;
	unsigned char decryptedtext[1024 + EVP_MAX_BLOCK_LENGTH];
	int decryptedtext_len = evp_decrypt((unsigned char *)enc_txt,
		(int)strlen ((char *)enc_txt), (unsigned char *)key,
		(unsigned char *)iv, decryptedtext
	);
	if (decryptedtext_len < 0)return -1;
	plain_text.append((const char *)decryptedtext, decryptedtext_len);
	return 1;
};
//END 5:39 AM 11/30/2018
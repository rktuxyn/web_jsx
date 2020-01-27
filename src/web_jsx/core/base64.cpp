/*
   base64.cpp and base64.h
   base64 encoding and decoding with C++.
   Version: 1.01.00
   Copyright (C) 2004-2017 René Nyffenegger
   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this source code must not be misrepresented; you must not
	  claim that you wrote the original source code. If you use this source code
	  in a product, an acknowledgment in the product documentation would be
	  appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
	  misrepresented as being the original source code.
   3. This notice may not be removed or altered from any source distribution.
   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/
//2:17 PM 1/3/2020
#	include "base64.h"
#	include <iostream>
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
const char kBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
size_t decoded_length(const char* in, size_t in_length) {
	int numEq = 0;

	const char* in_end = in + in_length;
	while (*--in_end == '=') ++numEq;

	return ((6 * in_length) / 8) - numEq;
}
size_t decoded_length(const std::string& in) {
	int numEq = 0;
	size_t n = in.size();

	for (std::string::const_reverse_iterator it = in.rbegin(); *it == '='; ++it) {
		++numEq;
	}

	return ((6 * n) / 8) - numEq;
}
size_t encoded_length(size_t length) {
	return (length + 2 - ((length + 2) % 3)) / 3 * 4;
}
size_t encoded_length(const std::string& in) {
	return encoded_length(in.length());
}
void strip_padding(std::string* in) {
	while (!in->empty() && *(in->rbegin()) == '=') in->resize(in->size() - 1);
}
void a3_to_a4(unsigned char* a4, unsigned char* a3) {
	a4[0] = (a3[0] & 0xfc) >> 2;
	a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
	a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
	a4[3] = (a3[2] & 0x3f);
}

void a4_to_a3(unsigned char* a3, unsigned char* a4) {
	a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
	a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
	a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}
unsigned char b64_lookup(unsigned char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 71;
	if (c >= '0' && c <= '9') return c + 4;
	if (c == '+') return 62;
	if (c == '/') return 63;
	return 255;
}

namespace sow_web_jsx {
	std::string base64::to_encode_str(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}

		return ret;
	}

	std::string base64::to_decode_str(std::string const& encoded_string) {
		size_t in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;
		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = 0; j < i; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}
	/*
	Copyright © 2004-2017 by René Nyffenegger

	This source code is provided 'as-is', without any express or implied
	warranty. In no event will the author be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this source code must not be misrepresented; you must not
	   claim that you wrote the original source code. If you use this source code
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original source code.

	3. This notice may not be removed or altered from any source distribution.
	*/
	bool base64::to_encode_str(const std::string& in, std::string&out) {
		int i = 0, j = 0;
		size_t enc_len = 0;
		unsigned char a3[3];
		unsigned char a4[4];
		out.resize(encoded_length(in));
		size_t input_len = in.size();
		std::string::const_iterator input = in.begin();
		while (input_len--) {
			a3[i++] = *(input++);
			if (i == 3) {
				a3_to_a4(a4, a3);
				for (i = 0; i < 4; i++) {
					out[enc_len++] = kBase64Alphabet[a4[i]];
				}
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 3; j++) {
				a3[j] = '\0';
			}
			a3_to_a4(a4, a3);
			for (j = 0; j < i + 1; j++) {
				out[enc_len++] = kBase64Alphabet[a4[j]];
			}
			while ((i++ < 3)) {
				out[enc_len++] = '=';
			}
		}
		return (enc_len == out.size());
	}

	bool base64::to_encode_str(const char* input, size_t input_length, char* out, size_t out_length) {
		int i = 0, j = 0;
		char* out_begin = out;
		unsigned char a3[3];
		unsigned char a4[4];
		size_t encod_length = encoded_length(input_length);
		if (out_length < encod_length) return false;
		while (input_length--) {
			a3[i++] = *input++;
			if (i == 3) {
				a3_to_a4(a4, a3);
				for (i = 0; i < 4; i++) {
					*out++ = kBase64Alphabet[a4[i]];
				}
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 3; j++) {
				a3[j] = '\0';
			}
			a3_to_a4(a4, a3);
			for (j = 0; j < i + 1; j++) {
				*out++ = kBase64Alphabet[a4[j]];
			}
			while ((i++ < 3)) {
				*out++ = '=';
			}
		}
		return (out == (out_begin + encod_length));
	}

	bool base64::to_decode_str(const std::string& in, std::string&out) {
		int i = 0, j = 0;
		size_t dec_len = 0;
		unsigned char a3[3];
		unsigned char a4[4];

		size_t input_len = in.size();
		std::string::const_iterator input = in.begin();

		out.resize(decoded_length(in));

		while (input_len--) {
			if (*input == '=') {
				break;
			}

			a4[i++] = *(input++);
			if (i == 4) {
				for (i = 0; i < 4; i++) {
					a4[i] = b64_lookup(a4[i]);
				}

				a4_to_a3(a3, a4);

				for (i = 0; i < 3; i++) {
					out[dec_len++] = a3[i];
				}

				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++) {
				a4[j] = '\0';
			}

			for (j = 0; j < 4; j++) {
				a4[j] = b64_lookup(a4[j]);
			}

			a4_to_a3(a3, a4);

			for (j = 0; j < i - 1; j++) {
				out[dec_len++] = a3[j];
			}
		}

		return (dec_len == out.size());
	}
	
	bool base64::to_decode_str(const char* input, size_t input_length, char* out, size_t out_length) {
		int i = 0, j = 0;
		char* out_begin = out;
		unsigned char a3[3];
		unsigned char a4[4];

		size_t decod_length = decoded_length(input, input_length);

		if (out_length < decod_length) return false;

		while (input_length--) {
			if (*input == '=') {
				break;
			}

			a4[i++] = *(input++);
			if (i == 4) {
				for (i = 0; i < 4; i++) {
					a4[i] = b64_lookup(a4[i]);
				}

				a4_to_a3(a3, a4);

				for (i = 0; i < 3; i++) {
					*out++ = a3[i];
				}

				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++) {
				a4[j] = '\0';
			}

			for (j = 0; j < 4; j++) {
				a4[j] = b64_lookup(a4[j]);
			}

			a4_to_a3(a3, a4);

			for (j = 0; j < i - 1; j++) {
				*out++ = a3[j];
			}
		}

		return (out == (out_begin + decod_length));
	}
}
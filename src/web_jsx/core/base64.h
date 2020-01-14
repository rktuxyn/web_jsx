//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_base64_h)
#define _base64_h
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
namespace sow_web_jsx {
	namespace base64 {
		std::string to_encode_str(unsigned char const* bytes_to_encode, unsigned int in_len);
		std::string to_decode_str(std::string const& encoded_string);
		bool to_encode_str(const std::string& in, std::string&out);
		bool to_encode_str(const char* input, size_t input_length, char* out, size_t out_length);
		bool to_decode_str(const std::string& in, std::string&out);
		bool to_decode_str(const char* input, size_t input_length, char* out, size_t out_length);
	}
}
#endif /* _base64_h */
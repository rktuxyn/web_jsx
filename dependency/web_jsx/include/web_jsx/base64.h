//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_base64_h)
#	define _base64_h
#	include <string>

#if !defined(WJSX_API)
#	define WJSX_API(type) type
#endif//!WJSX_API

namespace sow_web_jsx {
	namespace base64 {
		WJSX_API(std::string) to_encode_str(unsigned char const* bytes_to_encode, unsigned int in_len);
		WJSX_API(std::string) to_decode_str(std::string const& encoded_string);
		WJSX_API(bool) to_encode_str(const std::string& in, std::string&out);
		WJSX_API(bool) to_encode_str(const char* input, size_t input_length, char* out, size_t out_length);
		WJSX_API(bool) to_decode_str(const std::string& in, std::string&out);
		WJSX_API(bool) to_decode_str(const char* input, size_t input_length, char* out, size_t out_length);
	}
}
#endif /* _base64_h */
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

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

namespace sow_web_jsx {
	namespace base64 {
		EXPORT_WJSX std::string to_encode_str(unsigned char const* bytes_to_encode, unsigned int in_len);
		EXPORT_WJSX std::string to_decode_str(std::string const& encoded_string);
		EXPORT_WJSX bool to_encode_str(const std::string& in, std::string&out);
		EXPORT_WJSX bool to_encode_str(const char* input, size_t input_length, char* out, size_t out_length);
		EXPORT_WJSX bool to_decode_str(const std::string& in, std::string&out);
		EXPORT_WJSX bool to_decode_str(const char* input, size_t input_length, char* out, size_t out_length);
	}
}
#endif /* _base64_h */
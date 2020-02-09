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

#if defined(__WJSX_SHARED)
#if !defined(_export_wjsx)
#	define _export_wjsx __declspec(dllexport)
#endif//!jsx_export
#else
#if !defined(_export_wjsx)
#	define _export_wjsx
#endif//!_export_wjsx
#endif//__WJSX_SHARED

namespace sow_web_jsx {
	namespace base64 {
		_export_wjsx std::string to_encode_str(unsigned char const* bytes_to_encode, unsigned int in_len);
		_export_wjsx std::string to_decode_str(std::string const& encoded_string);
		_export_wjsx bool to_encode_str(const std::string& in, std::string&out);
		_export_wjsx bool to_encode_str(const char* input, size_t input_length, char* out, size_t out_length);
		_export_wjsx bool to_decode_str(const std::string& in, std::string&out);
		_export_wjsx bool to_decode_str(const char* input, size_t input_length, char* out, size_t out_length);
	}
}
#endif /* _base64_h */
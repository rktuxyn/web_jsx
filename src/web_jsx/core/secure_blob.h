#pragma once
#if !defined(_secure_blob_h)
#define _secure_blob_h
#if !defined(_VECTOR_)
#include <vector>
#endif//!_VECTOR_
#if !defined(_XSTRING_)
#include <string>
#endif // !_XSTRING_
//2:22 PM 11/22/2019
namespace sow_web_jsx {
	//https://chromium.googlesource.com/aosp/platform/external/libchromeos/+/master/brillo/secure_blob.h
	using Blob = std::vector<uint8_t>;
	class secure_blob : public Blob {
	public:
		secure_blob() = default;
		using Blob::vector;  // Inherit standard constructors from vector.
		explicit secure_blob(const std::string& data);
		~secure_blob();
		void resize(size_type count);
		void resize(size_type count, const value_type& value);
		void clear();
		std::string to_string() const;
		char* char_data() { return reinterpret_cast<char*>(data()); }
		const char* char_data() const {
			return reinterpret_cast<const char*>(data());
		}
		static secure_blob Combine(const secure_blob& blob1, const secure_blob& blob2);
	};
}
#endif //!_secure_blob_h
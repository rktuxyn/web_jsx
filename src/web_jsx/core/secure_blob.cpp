//2:22 PM 11/22/2019
#include "secure_blob.h"
namespace sow_web_jsx {
	void* SecureMemset(void* v, int c, size_t n) {
		volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(v);
		while (n--)
			* p++ = c;
		return v;
	}
	int SecureMemcmp(const void* s1, const void* s2, size_t n) {
		const uint8_t* us1 = reinterpret_cast<const uint8_t*>(s1);
		const uint8_t* us2 = reinterpret_cast<const uint8_t*>(s2);
		int result = 0;
		if (0 == n)
			return 1;
		/* Code snippet without data-dependent branch due to
		 * Nate Lawson (nate@root.org) of Root Labs. */
		while (n--)
			result |= *us1++ ^ *us2++;
		return result != 0;
	}
	secure_blob::secure_blob(const std::string& data)
		: secure_blob(data.begin(), data.end()) {}
	secure_blob::~secure_blob() {
		clear();
	}
	void secure_blob::resize(size_type count) {
		if (count < size()) {
			//Clone above repo
			SecureMemset(data() + count, 0, capacity() - count);
		}
		Blob::resize(count);
	}
	void secure_blob::resize(size_type count, const value_type& value) {
		if (count < size()) {
			SecureMemset(data() + count, 0, capacity() - count);
		}
		Blob::resize(count, value);
	}
	void secure_blob::clear() {
		//SecureMemset(data(), 0, capacity());
		Blob::clear();
	}
	std::string secure_blob::to_string() const {
		return std::string(data(), data() + size());
	}
	secure_blob secure_blob::Combine(const secure_blob& blob1,
		const secure_blob& blob2) {
		secure_blob result;
		result.reserve(blob1.size() + blob2.size());
		result.insert(result.end(), blob1.begin(), blob1.end());
		result.insert(result.end(), blob2.begin(), blob2.end());
		return result;
	}
	
}

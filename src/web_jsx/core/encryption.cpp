/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "encryption.h"
#	include <algorithm>
#if defined(WEB_JSX_CLIENT_BUILD)
Cipher::Cipher(int n)
	: map{ make_map(n) } {
};
std::unordered_map<char, char> Cipher::make_map(int n) {
	// helper function to give a positive value for a%b
	auto mod = [](int a, int b) { a %= b; return a < 0 ? a + b : a; };

	std::unordered_map<char, char> map;
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`~!@#$%^&*(){}[]123456789";
	int const m = sizeof alphabet - 1; // subtract the final NUL
	for (int i = 0; i < m; ++i) {
		map.insert(std::make_pair(alphabet[i], alphabet[mod(i + n, m)]));
	}

	return map;
};

std::string Cipher::encrypt(std::string s) const {
	std::transform(s.begin(), s.end(), s.begin(), [=](char c) {
		auto it = map.find(c);
		return it == map.end() ? c : it->second;
	});
	return s;
}
Cipher::~Cipher() {
	std::unordered_map<char, char>().swap(map);
}
;
#endif//!WEB_JSX_CLIENT_BUILD
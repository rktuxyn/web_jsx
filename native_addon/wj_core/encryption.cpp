/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "encryption.h"
#	include <algorithm>

Cipher::Cipher(int n)
	: map{ make_map(n) } {
}
std::unordered_map<char, char>* Cipher::make_map(int n) {
	// helper function to give a positive value for a%b
	auto mod = [](int a, int b) { a %= b; return a < 0 ? a + b : a; };
	std::unordered_map<char, char>* map = new std::unordered_map<char, char>();
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`~!@#$%^&*(){}[]123456789";
	int const m = sizeof alphabet - 1; // subtract the final NUL
	for (int i = 0; i < m; ++i) {
		map->insert(std::make_pair(alphabet[i], alphabet[mod(i + n, m)]));
	}
	return map;
}

void Cipher::encrypt_decrypt(std::string& str_out) const {
	std::transform(str_out.begin(), str_out.end(), str_out.begin(), [=](char c) {
		auto it = map->find(c);
		return it == map->end() ? c : it->second;
		});
	return;
}
void Cipher::clear() {
	if (map == NULL)return;
	map->clear(); delete map; map = NULL;
}
Cipher::~Cipher() {
	this->clear();
}
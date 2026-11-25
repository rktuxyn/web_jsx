/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
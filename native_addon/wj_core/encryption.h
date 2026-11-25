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
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER

#if !defined(_encryption_h)
#	define _encryption_h
#	include <string>
#	include <unordered_map>
//https://codereview.stackexchange.com/questions/198226/simple-encryption-for-strings?newreg=ae31ff1ca51a4a5bbdcb2aed1bb6c705
class Cipher {
private:
	std::unordered_map<char, char>* map;
public:
	explicit Cipher(int n);
	void encrypt_decrypt(std::string& str_out) const;
	void clear();
	~Cipher();
private:
	static std::unordered_map<char, char>* make_map(int n);
};
#endif//!WEB_JSX_CLIENT_BUILD
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#	include "web_jsx_global.h"
#	include <regex>
#	include <iostream>
#	include <sstream>
void sow_web_jsx::format__path(std::string&str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
}
void sow_web_jsx::get_dir_from_path (const std::string& path_str, std::string&dir) {
	dir.clear();
	size_t found = path_str.find_last_of("/\\");
	if (found == std::string::npos)return;
	dir = path_str.substr(0, found);
}
//\r\n
std::istream& sow_web_jsx::get_line(std::istream& is, std::string& t) {
	t.clear();
	std::streambuf* sb = is.rdbuf();
	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			t += (char)c;
			return is;
		case '\r':
			t += (char)c;
			c = sb->sgetc();
			if (c == '\n') {
				sb->sbumpc();
				t += (char)c;
			}
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty()) {
				is.setstate(std::ios::eofbit);
			}
			return is;
		default:
			t += (char)c;
		}
	}
}
bool sow_web_jsx::strings_equal(
	const std::string& s1,
	const std::string& s2,
	size_t n
) {
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	bool good = (n <= s1.length() && n <= s2.length());
	std::string::const_iterator l1 = good ? (s1.begin() + n) : s1.end();
	std::string::const_iterator l2 = good ? (s2.begin() + n) : s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return good;
}
bool sow_web_jsx::strings_equal(
	const std::string& s1,
	const std::string& s2
) {
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	std::string::const_iterator l1 = s1.end();
	std::string::const_iterator l2 = s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return (s2.size() == s1.size()) ? true : false;
}
std::string sow_web_jsx::extract_between(
	const std::string& data,
	const std::string& separator1,
	const std::string& separator2
) {
	std::string result;
	std::string::size_type start, limit;

	start = data.find(separator1, 0);
	if (std::string::npos != start) {
		start += separator1.length();
		limit = data.find(separator2, start);
		if (std::string::npos != limit)
			result = data.substr(start, limit - start);
	}

	return result;
}
#if !(defined(_WIN32)||defined(_WIN64))
wchar_t* sow_web_jsx::ccr2ws(const char* s) {
	size_t len = strlen(s);
	std::unique_ptr<wchar_t[]> tmp(new wchar_t[len + 1]);
	return tmp.release();
}
#else
#pragma warning(push)
#pragma warning(disable:4996)
wchar_t* sow_web_jsx::ccr2ws(const char* mbstr) {
	mbstate_t state;
	memset(&state, 0, sizeof state);
	size_t len = sizeof(wchar_t) + mbsrtowcs(NULL, &mbstr, 0, &state);
	wchar_t* buf = new wchar_t[len];
	mbsrtowcs(buf, &mbstr, len, &state);
	return buf;
}
#pragma warning(pop)
#endif//!_WIN32
//#	include <fstream>
//size_t sow_web_jsx::read_file(const char* absolute_path, std::stringstream& ssstream, bool check_file_exists) {
//	//size_t r_length = -1;
//	std::ifstream* file_stream = new std::ifstream(absolute_path, std::ifstream::binary);
//	//std::ifstream file_stream(absolute_path, std::ifstream::binary);
//	if (!file_stream->is_open()) {
//		delete file_stream;
//		ssstream << "File not found in#" << absolute_path;
//		return -1;
//	}
//	file_stream->seekg(0, std::ios::end);//Go to end of stream
//	std::streamoff totalSize = file_stream->tellg();
//	size_t total_len = (size_t)totalSize;
//	file_stream->seekg(0, std::ios::beg);//Back to begain of stream
//	if (total_len == std::string::npos || total_len == 0)return TRUE;
//	do {
//		if (!file_stream->good())break;
//		char* in;
//		size_t read_len = totalSize > READ_CHUNK ? READ_CHUNK : totalSize;
//		in = new char[read_len];
//		file_stream->read(in, read_len);
//		totalSize -= read_len;
//		ssstream.write(in, read_len);
//		/* Free memory */
//		delete[]in;
//		if (totalSize <= 0) break;
//	} while (true);
//	file_stream->close(); delete file_stream;
//	return total_len;
//}
//size_t sow_web_jsx::read_file(const char*absolute_path, std::string&str, bool check_file_exists) {
//	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
//	size_t ret = sow_web_jsx::read_file(absolute_path, ssstream, check_file_exists);
//	str = std::string(ssstream.str());
//	std::stringstream().swap(ssstream);
//	return ret;
//}
//char* sow_web_jsx::read_file(const char* absolute_path, bool check_file_exists = true) {
//	if (check_file_exists) {
//		if (__file_exists(absolute_path) == false) {
//			return new char[8]{ "INVALID" };
//		}
//	}
//	FILE* stream;
//	errno_t err = fopen_s(&stream, absolute_path, "rb");
//	if (err != 0)return new char[8]{ "INVALID" };
//	fseek(stream, 0, SEEK_END);//Go to end of stream
//	size_t size = ftell(stream);
//	rewind(stream);//Back to begain of stream
//	char* chars = new char[size + 1];
//	chars[size] = '\0';
//	for (size_t i = 0; i < size;) {
//		i += fread(&chars[i], 1, size - i, stream);
//		if (ferror(stream)) {
//			fclose(stream);
//			return new char[8]{ "INVALID" };
//		}
//	}
//	fclose(stream);
//	stream = NULL;
//	return chars;
//}

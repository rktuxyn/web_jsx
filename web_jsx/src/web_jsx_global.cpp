/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:23 AM 1/23/2019
#include "web_jsx_global.h"
void sow_web_jsx::format__path(std::string&str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
};
void sow_web_jsx::get_dir_from_path (const std::string& path_str, std::string&dir) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
};
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
int sow_web_jsx::dir_exists(const char* dir) {
	struct stat stats;
	stat(path, &stats);
	// Check for file existence
	if (S_ISDIR(stats.st_mode))
		return 1;
	return 0;
};
int sow_web_jsx::create_directory(const char* dir) {
#error Not Implemented
};
#else
int sow_web_jsx::dir_exists(const char* dir) {
	wchar_t* wString = (wchar_t*)malloc(MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, dir, -1, wString, MAX_PATH);
	if (CreateDirectory(wString, NULL)) {
		free(wString);
		return 1;
	}
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		free(wString);
		return -1;
	}
	free(wString);
	return -3;
};
int sow_web_jsx::create_directory(const char* dir) {
	DWORD ftyp = GetFileAttributesA(dir);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
};
#endif
size_t sow_web_jsx::read_file(const char*absolute_path, std::stringstream&ssstream, bool check_file_exists) {
	if (check_file_exists) {
		if (__file_exists(absolute_path) == false) return -1;
	}
	FILE*fs;
	errno_t err = fopen_s(&fs, absolute_path, "rb");
	if (fs == NULL) return -1;
	if (err != 0) return -1;
	fseek(fs, 0, SEEK_END);
	size_t t_length = ftell(fs);
	rewind(fs);
	size_t read_length = 0;
	size_t r_length = 0;
	while (true) {
		char buff[READ_CHUNK];
		read_length = fread(buff, 1, READ_CHUNK, fs);
		if (ferror(fs)) {
			fclose(fs);
			return -1;
		}
		r_length += read_length;
		ssstream.write(buff, read_length);
		t_length -= read_length;
		if (t_length <= 0)break;
		//if (feof(fs))break;
	}
	fclose(fs);
	return r_length;
};
size_t sow_web_jsx::read_file(const char*absolute_path, std::string&str, bool check_file_exists) {
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(absolute_path, ssstream, check_file_exists);
	if (ret < 0)return ret;
	str = ssstream.str();
	std::stringstream().swap(ssstream);
	return ret;
};
char* sow_web_jsx::read_file(const char* absolute_path, bool check_file_exists = true) {
	char *chars = new char[8]{ "INVALID" };
	if (check_file_exists) {
		if (__file_exists(absolute_path) == false) {
			return chars;
		}
	}
	try {
		FILE*stream;
		errno_t err;
		err = fopen_s(&stream, absolute_path, "rb");
		if (stream == NULL) return chars;
		if (err != 0)return chars;
		fseek(stream, 0, SEEK_END);
		size_t size = ftell(stream);
		rewind(stream);
		chars = new char[size + 1];
		chars[size] = '\0';
		for (size_t i = 0; i < size;) {
			i += fread(&chars[i], 1, size - i, stream);
			if (ferror(stream)) {
				fclose(stream);
				return new char[8]{ "INVALID" };
			}
		}
		fclose(stream);
		stream = NULL;

	} catch (std::exception) {
		chars = new char[8]{ "INVALID" };
	}
	return chars;
};
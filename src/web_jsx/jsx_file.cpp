/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "jsx_file.h"
using namespace sow_web_jsx;
jsx_file::jsx_file(const char *path, const char*mode) {
	//https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=vs-2017
	is_flush = 0;
	err = fopen_s(&_fstream, path, mode);
	//_fsopen("filename.txt", "a+", _SH_DENYWR);
}

const char * jsx_file::read() {
	return nullptr;
}

size_t jsx_file::write(const char *buffer) {
	if (is_flush == 1)return -1;
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), _fstream);
	if (ferror(_fstream)) {
		this->flush();
		return -2;
	}
	return len;
}

void jsx_file::flush() {
	is_flush = 1;
	fclose(_fstream);
	fflush(_fstream);
}

sow_web_jsx::jsx_file::~jsx_file() {
	this->_fstream = NULL;
}

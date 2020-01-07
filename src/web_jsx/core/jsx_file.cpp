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
	this->is_flush = FALSE;
#if defined(_WINDOWS_)
	this->_fstream = _fsopen(path, mode, _SH_DENYWR);
	if (ferror(this->_fstream)) {
		this->err = TRUE;
	}
	else {
		this->err = FALSE;
	}
#else
	this->err = fopen_s(&this->_fstream, path, mode);
#endif
}

const char * jsx_file::read() {
	return nullptr;
}

size_t jsx_file::write(const char *buffer) {
	if (this->is_flush == TRUE)return -1;
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), this->_fstream);
	if (ferror(this->_fstream)) {
		this->err = TRUE;
		this->flush();
		return -2;
	}
	return len;
}

void jsx_file::flush() {
	if (this->is_flush == TRUE)return;
	this->is_flush = TRUE;
	fclose(this->_fstream);
	fflush(this->_fstream);
}

sow_web_jsx::jsx_file::~jsx_file() {
	this->flush(); this->_fstream = NULL;
}

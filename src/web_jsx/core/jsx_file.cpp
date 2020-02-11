/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma warning(disable : 4996)
#	include "jsx_file.h"
#if !defined(FALSE)
#	define FALSE               0
#endif

#if !defined(TRUE)
#	define TRUE                1
#endif

#if !defined(_ERROR)
#	define _ERROR             -1
#endif

using namespace sow_web_jsx;
jsx_file_mode get_mode(const char* mode) {
	if (strcmp(mode, "r") == 0)return jsx_file_mode::f_read;
	if (strcmp(mode, "w") == 0)return jsx_file_mode::f_write;
	if (strcmp(mode, "a") == 0)return jsx_file_mode::f_append;
	if (strcmp(mode, "r+") == 0)return jsx_file_mode::f_read_write;
	if (strcmp(mode, "w+") == 0)return jsx_file_mode::f_read_write;
	if (strcmp(mode, "a+") == 0)return jsx_file_mode::f_read_write;
	return jsx_file_mode::f_ukn;
}
jsx_file::jsx_file(const char *path, const char*mode) {
	_internal_error = NULL; this->_is_flush = TRUE;
	_file_mode = jsx_file_mode::f_ukn; _errorc = FALSE;
	this->open(path, mode);
}
int sow_web_jsx::jsx_file::is_flush()const {
	return _is_flush;
}
size_t jsx_file::read(int len, std::string& out) {
	if (this->_is_flush == TRUE)return -1;
	if (
		_file_mode == jsx_file_mode::f_read ||
		_file_mode == jsx_file_mode::f_read_write
		) {
		if (this->eof() == TRUE)return -1;
		size_t read_req_len = _total_length > len ? (_total_length - len) : _total_length;
		char* buff = new char[read_req_len + sizeof(char)];
		buff[read_req_len] = '\0';
		size_t read_len = fread(buff, 1, len, this->_fstream);
		if (ferror(this->_fstream)) {
			read_len = -1;
			panic("Error occured while reading file");
		}
		if (this->_errorc == FALSE) {
			out.clear();
			out = std::string(buff, read_req_len);
			_total_length -= read_len;
		}
		delete[]buff;
		return read_len;
	}
	return panic("You should not read file in current mood.");
}
int sow_web_jsx::jsx_file::eof(){
	if (_total_length == 0)return TRUE;
	return FALSE;
}
size_t jsx_file::open(const char* path, const char* mode) {
	if (this->_is_flush == FALSE) return -1;
	_file_mode = get_mode(mode);
	if (_file_mode == jsx_file_mode::f_ukn) {
		return panic("Invalid file mode defined...");
	}
	this->clear();
	//https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=vs-2017
	this->_is_flush = FALSE;
#if (defined(_WIN32) || defined(_WIN64))
	this->_fstream = _fsopen(path, mode, _SH_DENYWR);
	if (ferror(this->_fstream)) {
		this->clear();
		this->panic("Unable to open/create file...");
	}
	else {
		fseek(this->_fstream, 0, SEEK_END);//Go to end of stream
		_total_length = ftell(this->_fstream);
		rewind(this->_fstream);//Back to begain of stream
	}
#else
	errno_t err = fopen_s(&this->_fstream, path, mode);
	if ( err != 0) {
		this->panic("Unable to open/create file...");
	}
#endif
	return _errorc;
}
size_t jsx_file::write(const char *buffer) {
	if (this->_is_flush == TRUE)return -1;
	if (
		_file_mode == jsx_file_mode::f_write ||
		_file_mode == jsx_file_mode::f_append ||
		_file_mode == jsx_file_mode::f_read_write
		) {
		size_t len = fwrite(buffer, sizeof(char), strlen(buffer), this->_fstream);
		if (ferror(this->_fstream)) {
			this->clear();
			return panic("Error occured while writing file");
		}
		return len;
	}
	return panic("You should not write file in current mood.");
}

void jsx_file::flush() {
	if (this->_is_flush == TRUE)return;
	this->_is_flush = TRUE;
	fflush(this->_fstream);
	fclose(this->_fstream);
	this->_fstream = NULL;
}

const char* jsx_file::get_last_error() {
	if (has_error() == FALSE) return "No Error Found!!!";
	return const_cast<const char*>(_internal_error);
}

void sow_web_jsx::jsx_file::clear(){
	this->flush();
	if (_internal_error != NULL) {
		_errorc = FALSE;
		delete[]_internal_error;
	}
}

int sow_web_jsx::jsx_file::has_error()const {
	return _errorc < 0 ? TRUE : FALSE;
}

jsx_file::~jsx_file() {
	this->clear();
}
int jsx_file::panic(const char* error) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	/*_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);*/
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	this->_errorc = -1;
	return -1;
}
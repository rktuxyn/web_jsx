#include "zgzip.h"
#if !defined(_FSTREAM_)
#include <fstream>// std::ifstream
#endif//!_FSTREAM_
void gzip::free_zstream(z_stream* strm) {
	if (strm == NULL)return;
	delete strm; strm = NULL;
}
//[Deflate==>Compress]
gzip::gzip_deflate::gzip_deflate(int level) {
	_is_flush = FALSE; _total_size = NULL; _tcrc = NULL; _is_error = FALSE;
	if (level == FALSE)level = Z_BEST_SPEED;
	_fs = NULL;
	_internal_error = new char;
	_strm = new z_stream();
	_strm->zalloc = Z_NULL;
	_strm->zfree = Z_NULL;
	_strm->opaque = Z_NULL;
	int ret = deflateInit2_(_strm, level, Z_DEFLATED,
		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		ZLIB_VERSION, (int)sizeof(z_stream)
	);
	if (ret != Z_OK) {
		free_zstream(_strm);
		panic("Unable to initilize deflateInit", TRUE);
		return;
	}
	_strm_ret = Z_OK;
	_stream_flush = Z_NO_FLUSH;
}
gzip::gzip_deflate::~gzip_deflate() {
	if (_is_flush == TRUE)return;
	if (_strm != NULL) {
		(void)deflateEnd(_strm);
		free_zstream(_strm);
	}
	if (_internal_error != NULL) {
		delete[]_internal_error;
		_internal_error = NULL;
	}
	this->f_close_file();
}
int gzip::gzip_deflate::has_error() {
	return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
}
const char* gzip::gzip_deflate::get_last_error() {
	if (_is_error == TRUE || _is_error < 0) {
		return const_cast<const char*>(_internal_error);
	}
	return "No Error Found!!!";

}
int gzip::gzip_deflate::panic(const char* error, int error_code) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_is_error = error_code;
	return _is_error;
}
int gzip::gzip_deflate::panic(char* erro_msg){
	if (_internal_error != NULL)
		free(_internal_error);
	_internal_error = new char[strlen(erro_msg) + 1];
	strcpy(_internal_error, const_cast<const char*>(erro_msg));
	_is_error = TRUE;
	return _is_error;
}
int gzip::gzip_deflate::f_open_file(const std::string path) {
	this->f_close_file();
	this->_fs = new std::ofstream(path.c_str(), std::ofstream::out | std::ofstream::binary);
	if (!this->_fs->is_open()) {
		return panic("Unable to open file....", -1);
	}
	return TRUE;
}
void gzip::gzip_deflate::f_write_header() {
	if (this->_fs == NULL) {
		new std::runtime_error("FileStream not open yet....");
		return;
	}
	this->write_header(*this->_fs);
}
size_t gzip::gzip_deflate::f_write_file(const std::string file_path, int do_flush) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write_file(*this->_fs, file_path, do_flush);
}
size_t gzip::gzip_deflate::f_write(const char* buff, int do_flush) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write(*this->_fs, buff, do_flush);
}
int gzip::gzip_deflate::f_flush() {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	int ret = this->flush(*this->_fs);
	if (ret == FALSE || ret < 0)return ret;
	return ret;
}
void gzip::gzip_deflate::f_write_footer() {
	if (this->_fs == NULL) {
		new std::runtime_error("FileStream not open yet....");
		return;
	}
	this->write_footer(*this->_fs);
}
void gzip::gzip_deflate::f_close_file() {
	if (this->_fs == NULL)return;
	if (this->_fs->is_open()) {
		this->_fs->flush();
		this->_fs->close();
	}
	delete this->_fs;
	this->_fs = NULL;
}
//[/Deflate==>Compress]
//[Inflate==>Decompress]
gzip::gzip_inflate::gzip_inflate(){
	_is_flush = FALSE; _is_error = FALSE;
	_fs = NULL; _total_size = NULL;
	_internal_error = new char;
	_strm = new z_stream();
	_strm->zalloc = Z_NULL;
	_strm->zfree = Z_NULL;
	_strm->opaque = Z_NULL;
	
	//_strm->wrap = TRUE;
	int ret = inflateInit2(_strm, -MAX_WBITS);
	if (ret != Z_OK) {
		free_zstream(_strm);
		panic("Unable to initilize inflateInit", TRUE);
		return;
	}
	//inflateSetDictionary(_strm, );
	//gz_header gz_h;
	//inflateGetHeader(_strm, &gz_h);
	_strm_ret = Z_OK;
	_stream_flush = Z_NO_FLUSH;
}
gzip::gzip_inflate::~gzip_inflate(){
	if (_is_flush == TRUE)return;
	if (_strm != NULL) {
		(void)inflateEnd(_strm);
		free_zstream(_strm);
	}
	if (_internal_error != NULL) {
		delete[]_internal_error;
		_internal_error = NULL;
	}
	this->f_close_file();
}
int gzip::gzip_inflate::has_error() {
	return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
}
void gzip::gzip_inflate::f_close_file() {
	if (this->_fs == NULL)return;
	if (this->_fs->is_open()) {
		this->_fs->flush();
		this->_fs->close();
	}
	delete this->_fs;
	this->_fs = NULL;
}
size_t gzip::gzip_inflate::f_write_file(const std::string file_path, int do_flush) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write_file(*this->_fs, file_path, do_flush);
}
size_t gzip::gzip_inflate::f_write(const char* buff, int do_flush) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write(*this->_fs, buff, do_flush);
}
int gzip::gzip_inflate::f_open_file(const std::string path) {
	this->f_close_file();
	this->_fs = new std::ofstream(path.c_str(), std::ofstream::out | std::ofstream::binary);
	if (!this->_fs->is_open()) {
		return panic("Unable to open file....", -1);
	}
	return TRUE;
}
int gzip::gzip_inflate::f_flush() {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	int ret = this->flush(*this->_fs);
	if (ret == FALSE || ret < 0)return ret;
	return ret;
}
const char* gzip::gzip_inflate::get_last_error() {
	if (_is_error == TRUE || _is_error < 0) {
		return const_cast<const char*>(_internal_error);
	}
	return "No Error Found!!!";

}
int gzip::gzip_inflate::panic(const char* error, int error_code) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_is_error = error_code;
	return _is_error;
}
int gzip::gzip_inflate::panic(char* erro_msg){
	if (_internal_error != NULL)
		free(_internal_error);
	_internal_error = new char[strlen(erro_msg) + 1];
	strcpy(_internal_error, const_cast<const char*>(erro_msg));
	_is_error = TRUE;
	return _is_error;
}
//[/Inflate==>Decompress]

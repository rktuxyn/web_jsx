#include "zgzip.h"
#if !defined(_FSTREAM_)
#include <fstream>// std::ifstream
#endif//!_FSTREAM_
void gzip::free_zstream(z_stream* strm) {
	if (strm == NULL)return;
	delete strm; strm = NULL;
}
z_stream* gzip::create_z_stream() {
	z_stream* _strm = new z_stream();
	/* allocate deflate state */
	/*_strm->zalloc = Z_NULL;
	_strm->zfree = Z_NULL;
	_strm->opaque = Z_NULL;*/
	_strm->zalloc = (alloc_func)0;
	_strm->zfree = (free_func)0;
	_strm->opaque = (voidpf)0;
	return _strm;
}

int gzip::deflate_file(const std::string input_path, const std::string output_path, int level, std::string& error){
	z_stream* strm = create_z_stream();
	int ret = deflateInit(strm, level);
	if (ret != Z_OK) {
		free_zstream(strm);
		error = "Unable to initilize deflateInit";
		return FALSE;
	}
	std::ifstream* source = new std::ifstream(input_path.c_str(), std::ifstream::binary);
	if (!source->is_open()) {
		error = "Unable to open input file #";
		error.append(input_path.c_str());
		return FALSE;
	}
	std::ofstream* dest = new std::ofstream(output_path.c_str(), std::ofstream::out | std::ofstream::binary);
	if (!dest->is_open()) {
		error = "Unable to open output file #";
		error.append(output_path.c_str());
		return FALSE;
	}
	size_t totalSize = get_size_of_stream(*source);
	int flush = Z_NO_FLUSH; size_t read_len = 0;
	unsigned have;
	size_t			block_size = 0;
	const size_t	max_block_size = 10485760;/*(Max block size (1024*1024)*10) = 10 MB*/
	/* compress until end of stream */
	do {
		char* in;
		if (totalSize >= CHUNK) {
			read_len = CHUNK;
		}
		else {
			read_len = totalSize;
		}
		in = new char[read_len];
		source->read(in, read_len);
		strm->avail_in = (uInt)read_len;
		totalSize -= read_len;
		flush = totalSize <= 0 ? Z_FINISH : Z_NO_FLUSH;
		strm->next_in = (Bytef*)in;
		/* run deflate() on input until output buffer not full, finish
		  compression if all of source has been read in */
		do {
			char* out = new char[CHUNK];
			strm->avail_out = CHUNK;
			strm->next_out = (Bytef*)out;
			ret = deflate(strm, flush);    /* no bad return value */
			if (ret == Z_STREAM_ERROR) {
				/* state not clobbered */
				source->close(); delete source;
				dest->close(); delete dest;
				error.append(strm->msg);
				(void)deflateEnd(strm);
				free_zstream(strm);
				return FALSE;
			}
			have = CHUNK - strm->avail_out;
			block_size += have;
			dest->write(out, have);
			/* Free memory */
			delete[]out;
		} while (strm->avail_out == 0);
		/* Free memory */
		delete[]in;
		if (strm->avail_in != 0) {
			source->close(); delete source;
			dest->close(); delete dest;
			/* all input will be used */
			error = "all input will be used";
			(void)deflateEnd(strm);
			free_zstream(strm);
			return FALSE;
		}
		if (block_size >= max_block_size) {
			block_size = 0; dest->flush();
		}
	} while (flush != Z_FINISH);
	source->close(); delete source;
	dest->close(); delete dest;
	if (ret != Z_STREAM_END) {
		/* stream will be complete */
		error = "stream will be complete";
		ret = FALSE;
	}
	else {
		ret = TRUE;
	}
	 /* clean up and return */
	(void)deflateEnd(strm);
	free_zstream(strm);
	return ret;
}
int gzip::inflate_file(const std::string input_path, const std::string output_path, std::string& error){
	z_stream* strm = create_z_stream();
	int ret = inflateInit(strm);
	if (ret != Z_OK) {
		free_zstream(strm);
		error = "Unable to initilize deflateInit";
		return FALSE;
	}
	std::ifstream* source = new std::ifstream(input_path.c_str(), std::ifstream::binary);
	std::ofstream* dest = new std::ofstream(output_path.c_str(), std::ofstream::out | std::ofstream::binary);
	size_t totalSize = get_size_of_stream(*source);
	size_t			block_size = 0;
	const size_t	max_block_size = 10485760;/*(Max block size (1024*1024)*10) = 10 MB*/
	int eof = FALSE; size_t read_len = 0;
	unsigned have;
	do {
		char* in;
		if (totalSize >= CHUNK) {
			read_len = CHUNK;
		}
		else {
			read_len = totalSize;
		}
		in = new char[read_len];
		source->read(in, read_len);
		strm->avail_in = (uInt)read_len;
		totalSize -= read_len;
		eof = totalSize <= 0 ? TRUE : FALSE;
		strm->next_in = (Bytef*)in;
		/* run inflate() on input until output buffer not full, finish
		  decompression if all of source has been read in */
		do {
			char* out = new char[CHUNK];
			strm->avail_out = CHUNK;
			strm->next_out = (Bytef*)out;
			ret = inflate(strm, Z_NO_FLUSH);    /* no bad return value */
			if (ret != Z_OK) {
				switch (ret) {
				case Z_STREAM_ERROR:/* state not clobbered */
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					/* Free memory */
					delete[]out;
					source->close(); delete source;
					dest->close(); delete dest;
					error.append(strm->msg);
					(void)inflateEnd(strm);
					free_zstream(strm);
					return FALSE;
				}
			}
			have = CHUNK - strm->avail_out;
			dest->write(out, have);
			/* Free memory */
			delete[]out;
			block_size += have;
		} while (strm->avail_out == 0);
		/* Free memory */
		delete[]in;
		if (strm->avail_in != 0) {
			source->close(); delete source;
			dest->close(); delete dest;
			/* all input will be used */
			error = "all input will be used";
			(void)inflateEnd(strm);
			free_zstream(strm);
			return FALSE;
		}
		if (block_size >= max_block_size) {
			block_size = 0; dest->flush();
		}
	} while (eof != TRUE);
	source->close(); delete source;
	dest->close(); delete dest;
	/* clean up and return */
	(void)inflateEnd(strm);
	free_zstream(strm);
	return ret;
}
//[Deflate==>Compress]
gzip::gzip_deflate::gzip_deflate(int level) {
	_is_flush = FALSE; _total_size = NULL; _tcrc = NULL; _is_error = FALSE;
	if (level == FALSE)level = Z_BEST_SPEED;
	_fs = NULL;
	_internal_error = new char;
	_strm = create_z_stream();
	/* negative windowBits to deflateInit2_ means "no header" */
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
int gzip::gzip_deflate::panic(char* erro_msg) {
	if (_internal_error != NULL)
		free(_internal_error);
	_internal_error = new char[strlen(erro_msg) + 1];
	strcpy(_internal_error, const_cast<const char*>(erro_msg));
	_is_error = TRUE;
	return _is_error;
}
//[/Deflate==>Compress]
//[Inflate==>Decompress]
gzip::gzip_inflate::gzip_inflate(int window_bits){
	_is_flush = FALSE; _is_error = FALSE;
	_fs = NULL; _total_size = NULL;
	_internal_error = new char;
	_strm = create_z_stream();
	_no_footer = window_bits < 0 ? TRUE : FALSE;
	if (window_bits > 32) {
		panic("Invalid window bits", TRUE);
		return;
	}
	/* negative windowBits to inflateInit2_ means "no header" */
	//int ret = inflateInit2(_strm, MAX_WBITS + 16);
	int ret = inflateInit2(_strm, window_bits);
	//int ret = inflateInit(_strm);
	if (ret != Z_OK) {
		free_zstream(_strm);
		panic("Unable to initilize inflateInit", TRUE);
		return;
	}
	_strm_ret = Z_OK;
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
int gzip::gzip_inflate::f_open_file(const std::string path) {
	this->f_close_file();
	this->_fs = new std::ofstream(path.c_str(), std::ofstream::out | std::ofstream::binary);
	if (!this->_fs->is_open()) {
		return panic("Unable to open file....", -1);
	}
	return TRUE;
}
size_t gzip::gzip_inflate::f_write_file(const std::string file_path) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write_file(*this->_fs, file_path);
}
size_t gzip::gzip_inflate::f_write(const char* buff) {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	return this->write(*this->_fs, buff);
}
int gzip::gzip_inflate::f_flush() {
	if (this->_fs == NULL) {
		return panic("FileStream not open yet....", -1);
	}
	int ret = this->flush(*this->_fs);
	if (ret == FALSE || ret < 0)return ret;
	return ret;
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
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_zgzip_h)
#define _zgzip_h
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
//3:45 PM 11/24/2018
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Have to check !TODO
#else
#if !defined(_IOSTREAM_)
#include <iostream>
#endif//!_IOSTREAM_
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#endif//_WIN32||_WIN64/__unix__
#if !defined(_INC_STDIO)
#include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#endif//!_INC_STDIO
#if !defined(_XSTRING_)
#include <string>// !_XSTRING_// memcpy, memset
#endif //!_XSTRING_
#if !defined(ZLIB_H)
#include <zlib.h>
#endif//!ZLIB_H
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#if !defined(_INC_FCNTL)
#  include <fcntl.h>
#endif//_INC_FCNTL
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#elif defined(_WIN32) || defined(MSDOS) || defined(OS2)
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#else
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#endif//WIN32
#if !defined(_SSTREAM_)
#include <sstream> // std::stringstream
#endif//_SSTREAM_
#if !defined(CHUNK)
#define CHUNK 16384
#endif//!CHUNK
#if !defined(OS_CODE)
#  define OS_CODE  0x03
#endif//!OS_CODE
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif//!MAX_MEM_LEVEL
#if !defined(assert)
#define assert(expression) ((void)0)
#endif//!assert
namespace gzip {
	//Success run on 2:05 AM 1/19/2019
	//https://stackoverflow.com/questions/54256829/zlib-gzip-invalid-response-defined-in-web-browser-c
	static int gz_magic[2] = { 0x1f, 0x8b }; /* gzip magic header */
	template<class _out_stream>
	void write_magic_header(_out_stream&output) {
		char* dest = new char[10];
		sprintf(dest, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
		output.write(const_cast<const char*>(dest), 10);
		delete[]dest;
	}
	static void free_zstream(z_stream* strm) {
		if (strm == NULL)return;
		delete strm; strm = NULL;
	}
	template<class _out_stream>
	int deflate_stream(std::stringstream&source, _out_stream&dest, int level = Z_BEST_SPEED) {
		//6:08 AM 1/17/2019
		int ret, flush;
		unsigned have;
		z_stream* strm = new z_stream();
		/* allocate deflate state */
		strm->zalloc = Z_NULL;
		strm->zfree = Z_NULL;
		strm->opaque = Z_NULL;
		ret = deflateInit2_(strm, level, Z_DEFLATED,
			-MAX_WBITS,
			DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK) {
			free_zstream(strm);
			return ret;
		}
		source.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = source.tellg();
		std::streamoff utotalSize = totalSize;
		source.seekg(0, std::ios::beg);//Back to begain of stream
		int write_len = 0;
		//bool is_first = true;
		std::streamsize n;
		/* compress until end of stream */
		uLong tcrc = 0;
		do {
			char* in = new char[CHUNK];
			n = source.rdbuf()->sgetn(in, CHUNK);
			strm->avail_in = (uInt)n;
			tcrc = crc32(tcrc, (uint8_t*)in, (uInt)n);
			totalSize -= n;
			flush = totalSize <= 0 ? Z_FINISH : Z_NO_FLUSH;
			strm->next_in = (Bytef*)in;
			/* run deflate() on input until output buffer not full, finish
			  compression if all of source has been read in */
			do {
				char* out = new char[CHUNK];
				strm->avail_out = CHUNK;
				strm->next_out = (Bytef*)out;
				ret = deflate(strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = CHUNK - strm->avail_out;
				dest.write(out, have);
				write_len += have;
				delete[]out;
			} while (strm->avail_out == 0);
			assert(strm->avail_in == 0);     /* all input will be used */
			 /* Free memory */
			delete[]in;
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */
		 /* clean up and return */
		(void)deflateEnd(strm);
		free_zstream(strm);
		/* write gzip footer to out stream*/
		dest.write((char*)&tcrc, sizeof(tcrc));
		dest.write((char*)&utotalSize, sizeof(utotalSize));
		//std::ifstream ifs;
		return write_len;
	}
	template<class _out_stream>
	void compress_gzip (std::stringstream&source_stream, _out_stream&out_stream) {
		write_magic_header(out_stream);
		deflate_stream(source_stream, out_stream);
	}
	//2:28 PM 1/4/2020
	class gzip_deflate {
	public:
		gzip_deflate(int level);
		~gzip_deflate();
		template<class _out_stream>
		void write_header(_out_stream& dest);
		template<class _out_stream>
		size_t write(_out_stream&dest, const char* buff, int do_flush);
		template<class _out_stream, class _source_stream>
		size_t write(_out_stream& dest, _source_stream& source, int do_flush, int bypass);
		template<class _out_stream>
		size_t write_file(_out_stream& dest, const std::string file_path, int do_flush);
		template<class _out_stream>
		int flush(_out_stream& dest);
		int has_error();
		const char* get_last_error();
	private:
		z_stream* _strm;
		uLong _tcrc;
		int _strm_ret;
		int _stream_flush;
		int _is_flush;
		long _total_size;
		int _is_error;
		char* _internal_error;
		template<class _out_stream>
		size_t write(_out_stream& dest, char* in, size_t len, int do_flush, int bypass);
		int panic(const char* error, int error_code);
	};
	inline gzip_deflate::gzip_deflate(int level){
		_is_flush = FALSE; _total_size = NULL; _tcrc = NULL; _is_error = FALSE;
		if (level == FALSE)level = Z_BEST_SPEED;
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
			panic("Unable to initilize deflateInit2_", TRUE);
			return;
		}
		_strm_ret = Z_OK;
		_stream_flush = Z_NO_FLUSH;
	}
	inline gzip_deflate::~gzip_deflate(){
		if (_is_flush == TRUE)return;
		if (_strm != NULL) {
			delete _strm;_strm = NULL;
		}
		if (_internal_error != NULL) {
			delete[]_internal_error;
			_internal_error = NULL;
		}
	}
	template<class _out_stream>
	inline size_t gzip_deflate::write(_out_stream& dest, char* in, size_t len, int do_flush, int bypass) {
		if (bypass == FALSE) {
			if (_is_flush == TRUE)return FALSE;
			if (_is_error == TRUE)return -1;
			if (_stream_flush == Z_FINISH) {
				return panic("deflate::state Z_FINISH", -1);
			}
			if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
				return panic("deflate::Invalid stream end request.", -1);
			}
			_is_error = FALSE;
		}
		_stream_flush = do_flush;
		unsigned have;
		_strm->avail_in = (uInt)len;
		_tcrc = crc32(_tcrc, (uint8_t*)in, (uInt)len);
		_strm->next_in = (Bytef*)in;
		/* run deflate() on input until output buffer not full, finish
		  compression if all of source has been read in */
		int chunk = len > CHUNK ? CHUNK : (int)len;
		do {
			char* out = new char[chunk];
			_strm->avail_out = chunk;
			_strm->next_out = (Bytef*)out;
			_strm_ret = deflate(_strm, _stream_flush);    /* no bad return value */
			/* state not clobbered */
			if (_strm_ret == Z_STREAM_ERROR) {
				/* Free memory */
				delete[]out;
				return panic("deflate::state not clobbered", TRUE);
			}
			have = chunk - _strm->avail_out;
			dest.write(out, have);
			_total_size += have;
			/* Free memory */
			delete[]out;
		} while (_strm->avail_out == 0);
		/* all input will be used */
		if (_strm->avail_in != 0) {
			return panic("deflate::all input unable to use", TRUE);
		}
		return len;
	}
	template<class _out_stream>
	inline void gzip_deflate::write_header(_out_stream& dest){
		write_magic_header(dest);
	}
	template<class _out_stream>
	inline size_t gzip_deflate::write(_out_stream& dest, const char* buff, int do_flush){
		char* in = strdup(buff);
		size_t ret = this->write(dest, in, strlen(buff), do_flush, FALSE);
		/* Free memory */
		delete[]in;
		return ret;
	}
	template<class _out_stream, class _source_stream>
	inline size_t gzip_deflate::write(_out_stream& dest, _source_stream& source, int do_flush, int bypass){
		if (bypass == FALSE) {
			if (_is_flush == TRUE)return FALSE;
			if (_is_error == TRUE)return -1;
			if (_stream_flush == Z_FINISH) {
				return panic("deflate::state Z_FINISH", -1);
			}
			if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
				return panic("deflate::Invalid stream end request.", -1);
			}
		}
		source.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = source.tellg();
		size_t total_len = (size_t)totalSize;
		source.seekg(0, std::ios::beg);//Back to begain of stream
		int end_flush = do_flush == Z_FINISH ? Z_FINISH : Z_NO_FLUSH;
		do_flush = Z_NO_FLUSH;
		size_t ret; size_t read_len = 0; int eof = FALSE;
		do {
			char* in;
			read_len = totalSize > CHUNK ? CHUNK : totalSize;
			in = new char[read_len];
			source.read(in, read_len);
			//source.rdbuf()->sgetn(in, read_len);
			totalSize -= read_len;
			if (totalSize <= 0) {
				do_flush = end_flush; eof = TRUE;
			}
			ret = this->write(dest, in, read_len, do_flush, TRUE);
			/* Free memory */
			delete[]in;
			if (ret == FALSE || ret == std::string::npos || ret < 0)return ret;
		} while (eof == FALSE);
		return total_len;
	}
	template<class _out_stream>
	inline size_t gzip_deflate::write_file(_out_stream& dest, const std::string file_path, int do_flush){
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_stream_flush == Z_FINISH) {
			return panic("deflate::state Z_FINISH", -1);
		}
		if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
			return panic("deflate::Invalid stream end request.", -1);
		}
		if (_is_error != FALSE) _is_error = FALSE;
		std::ifstream file_stream(file_path.c_str(), std::ifstream::binary);
		if (!file_stream.is_open()) {
			return panic("Unable to open file....", -1);
		}
		size_t ret = this->write(dest, file_stream, do_flush, TRUE);
		file_stream.close();
		return ret;
		//file_stream.seekg(0, std::ios::end);//Go to end of stream
		//std::streamoff totalSize = file_stream.tellg();
		//size_t total_len = (size_t)totalSize;
		//file_stream.seekg(0, std::ios::beg);//Back to begain of stream
		//int end_flush = do_flush == Z_FINISH ? Z_FINISH : Z_NO_FLUSH;
		//do_flush = Z_NO_FLUSH;
		//size_t ret; size_t read_len = 0; int eof = FALSE;
		//do {
		//	char* in;
		//	read_len = totalSize > CHUNK ? CHUNK : totalSize;
		//	in = new char[read_len];
		//	file_stream.read(in, read_len);
		//	totalSize -= read_len;
		//	if (totalSize <= 0) {
		//		do_flush = end_flush; eof = TRUE;
		//	}
		//	ret = this->write(dest, in, read_len, do_flush, TRUE);
		//	/* Free memory */
		//	delete[]in;
		//	if (ret == FALSE || ret == std::string::npos || ret < 0) {
		//		file_stream.close();
		//		return ret;
		//	}
		//} while (eof == FALSE);
		//file_stream.close();
		//return total_len;
	}
	template<class _out_stream>
	inline int gzip_deflate::flush(_out_stream& dest){
		if (_is_flush)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_stream_flush != Z_FINISH) {
			return panic("deflate::state yet not Z_FINISH", -1);
		}
		/* stream will be complete */
		if (_strm_ret != Z_STREAM_END) {
			return panic("deflate::stream not completed yet", TRUE);
		}
		/* clean up and return */
		(void)deflateEnd(_strm);
		free_zstream(_strm);
		/* write gzip footer to out stream*/
		dest.write((char*)& _tcrc, sizeof(_tcrc));
		dest.write((char*)& _total_size, sizeof(_total_size));
		_is_flush = TRUE;
		return TRUE;
	}
	inline int gzip_deflate::has_error(){
		return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
	}
	inline const char* gzip_deflate::get_last_error() {
		if (_is_error == TRUE || _is_error < 0) {
			return const_cast<const char*>(_internal_error);
		}
		return "No Error Found!!!";
		
	}
	inline int gzip_deflate::panic(const char* error, int error_code){
		if (_internal_error != NULL)
			delete[]_internal_error;
		_internal_error = new char[strlen(error) + 1];
		strcpy(_internal_error, error);
		_is_error = error_code;
		return _is_error;
	}
	//Done 12:10 PM 1/7/2020
}; // namespace gzip
#endif//_zgzip_h
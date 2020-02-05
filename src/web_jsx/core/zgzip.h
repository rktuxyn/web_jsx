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
#	define _zgzip_h
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
//3:45 PM 11/24/2018
#	include	<iostream>
#	include	<stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#	include	<string>// !_XSTRING_// memcpy, memset
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#	include	<windows.h>
#	include	<fcntl.h>
#	include	<io.h>
#if !defined(OS_CODE)
#	define OS_CODE  0x00
#endif//!OS_CODE
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#	define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#else
#	define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#else
#	include	<io.h>
#if !defined(SET_BINARY_MODE)
#	define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#if !defined(OS_CODE)
#	define OS_CODE  0x03 /*Assume Unix*/
#endif//!OS_CODE
#endif//WIN32
#	include	<sstream> // std::stringstream
#	include	<fstream>// std::ifstream
#	include	<zlib.h>
#if !defined(CHUNK)
#	define CHUNK 16384
#endif//!CHUNK
/*
0x00	FAT filesystem (MS-DOS, OS/2, NT/Win32)
0x01	Amiga
0x02	VMS (or OpenVMS)
0x03	Unix
0x04	VM/CMS
0x05	Atari TOS
0x06	HPFS filesystem (OS/2, NT)
0x07	Macintosh
0x08	Z-System
0x09	CP/M
0x0a	TOPS-20
0x0b	NTFS filesystem (NT)
0x0c	QDOS
0x0d	Acorn RISCOS
0xff	unknown
*/
#if MAX_MEM_LEVEL >= 8
#	define DEF_MEM_LEVEL 8
#else
#	define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif//!MAX_MEM_LEVEL
#if !defined(assert)
#	define assert(expression) ((void)0)
#endif//!assert
//Read more http://www.zlib.net/zpipe.c
namespace gzip {
#	define	GZIP_MAGIC     "\037\213"	/* Magic header for gzip files, 1F 8B */
	//Success run on 2:05 AM 1/19/2019
	//https://stackoverflow.com/questions/54256829/zlib-gzip-invalid-response-defined-in-web-browser-c
	//static int gz_magic[2] = { 0x1f, 0x8b }; /* gzip magic header */
	template<class _out_stream>
	void write_magic_header(_out_stream&output) {
		char* dest = new char[10];
		sprintf(dest, "%c%c%c%c%c%c%c%c%c%c", GZIP_MAGIC[0], GZIP_MAGIC[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
		//output.write(const_cast<const char*>(dest), 10);
		output.write(dest, 10);
		delete[]dest;
	}
	template<class _out_stream>
	void write_crlf(_out_stream& output) {
		const char* crlf = "\r\n\r\n";
		output.write(crlf, strlen(crlf));
	}
	void free_zstream(z_stream* strm);
	z_stream* create_z_stream();
	template<class _stream>
	size_t get_size_of_stream(_stream& _strm) {
		_strm.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff length = _strm.tellg();
		_strm.seekg(0, std::ios::beg);//Back to begain of stream
		return static_cast<size_t>(length);
		//return (size_t)length;
	}
	int deflate_file(const std::string input_path, const std::string output_path, int level, std::string&error);
	int inflate_file(const std::string input_path, const std::string output_path, std::string& error);
	template<class _out_stream>
	int deflate_stream(std::stringstream&source, _out_stream&dest, int level = Z_BEST_SPEED) {
		//6:08 AM 1/17/2019
		int ret, flush;
		unsigned have;
		z_stream* strm = create_z_stream();
		/* negative windowBits to deflateInit2_ means "no header" */
		ret = deflateInit2_(strm, level, Z_DEFLATED,
			-MAX_WBITS,
			DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK) {
			free_zstream(strm);
			return ret;
		}
		size_t totalSize = get_size_of_stream(source);
		std::streamoff utotalSize = totalSize;
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
	//[Deflate==>Compress]
	//2:28 PM 1/4/2020
	class gzip_deflate {
	public:
		gzip_deflate(int level);
		~gzip_deflate();
		template<class _out_stream>
		void write_header(_out_stream& dest);
		template<class _out_stream>
		void write_footer(_out_stream& dest);
		template<class _out_stream>
		size_t write(_out_stream&dest, const char* buff, int do_flush);
		template<class _out_stream, class _source_stream>
		size_t write(_out_stream& dest, _source_stream& source, int do_flush, int bypass);
		template<class _out_stream>
		size_t write_file(_out_stream& dest, const std::string file_path, int do_flush);
		template<class _out_stream>
		int flush(_out_stream& dest);
		int f_open_file(const std::string path);
		size_t f_write_file(const std::string file_path, int do_flush);
		template<class _source_stream>
		size_t f_write(_source_stream& source, int do_flush);
		size_t f_write(const char* buff, int do_flush);
		int f_flush();
		void f_write_header();
		void f_write_footer();
		void f_close_file();
		int has_error();
		const char* get_last_error();
	private:
		std::ofstream* _fs;
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
		int panic(char* erro_msg);
	};
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
			//if (_strm_ret == Z_STREAM_ERROR) {
			//	/* Free memory */
			//	delete[]out;
			//	return panic(_strm->msg);
			//}
			if (_strm_ret != Z_OK) {
				switch (_strm_ret) {
				case Z_STREAM_ERROR:/* state not clobbered */
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					/* Free memory */
					delete[]out;
					return panic(_strm->msg);
				}
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
	inline void gzip_deflate::write_header(_out_stream& dest) {
		write_magic_header(dest);
	}
	template<class _out_stream>
	inline void gzip_deflate::write_footer(_out_stream& dest){
		if (_total_size == 0 || _total_size == std::string::npos)return;
		//write_crlf(dest);
		/* write gzip footer to out stream*/
		dest.write((char*)& _tcrc, sizeof(_tcrc));
		dest.write((char*)& _total_size, sizeof(_total_size));
		_total_size = 0; _tcrc = 0;
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
		size_t totalSize = get_size_of_stream(source);
		size_t total_len = totalSize;
		if (total_len == std::string::npos || total_len == 0)return total_len;
		int end_flush = do_flush == Z_FINISH ? Z_FINISH : Z_NO_FLUSH;
		do_flush = Z_NO_FLUSH;
		size_t ret; size_t read_len = 0; int eof = FALSE;
		do {
			if (!source.good())break;
			char* in;
			read_len = totalSize > CHUNK ? CHUNK : totalSize;
			in = new char[read_len];
			source.read(in, read_len);
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
		std::ifstream* file_stream = new std::ifstream(file_path.c_str(), std::ifstream::binary);
		if (!file_stream->is_open()) {
			return panic("Unable to open file....", -1);
		}
		size_t ret = this->write(dest, *file_stream, do_flush, TRUE);
		file_stream->close(); delete file_stream;
		return ret;
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
		_is_flush = TRUE;
		return TRUE;
	}
	template<class _source_stream>
	inline size_t gzip_deflate::f_write(_source_stream& source, int do_flush){
		if (this->_fs == NULL) {
			return panic("FileStream not open yet....", -1);
		}
		return this->write(*this->_fs, source, do_flush, FALSE);
	}
	//Done 12:10 PM 1/7/2020
	//[/Deflate==>Compress]
	//[Inflate==>Decompress]
	//Start 11:27 PM 1/8/2020
	class gzip_inflate {
	public:
		gzip_inflate(int window_bits);
		~gzip_inflate();
		template<class _out_stream>
		size_t write(_out_stream& dest, const char* buff);
		template<class _out_stream, class _source_stream>
		size_t write(_out_stream& dest, _source_stream& source, int bypass);
		template<class _out_stream>
		size_t write_file(_out_stream& dest, const std::string file_path);
		template<class _out_stream>
		int flush(_out_stream& dest);
		int f_open_file(const std::string path);
		size_t f_write_file(const std::string file_path);
		template<class _source_stream>
		size_t f_write(_source_stream& source);
		size_t f_write(const char* buff);
		int f_flush();
		void f_close_file();
		int has_error();
		const char* get_last_error();
	private:
		std::ofstream* _fs;
		z_stream* _strm;
		int _no_footer;
		int _strm_ret;
		int _is_flush;
		int _is_error;
		char* _internal_error;
		long _total_size;
		template<class _out_stream>
		size_t write(_out_stream& dest, char* in, size_t len, int do_flush, int bypass);
		int panic(const char* error, int error_code);
		int panic(char* erro_msg);
	};
	template<class _out_stream>
	inline size_t gzip_inflate::write(_out_stream& dest, char* in, size_t len, int do_flush, int bypass){
		if (bypass == FALSE) {
			if (_is_flush == TRUE)return FALSE;
			if (_is_error == TRUE)return -1;
			_is_error = FALSE;
		}
		unsigned have;
		_strm->avail_in = (uInt)len;
		_strm->next_in = (Bytef*)in;
		/* run inflate() on input until output buffer not full, finish
		  decompression if all of source has been read in */
		int chunk = len > CHUNK ? CHUNK : (int)len;
		//chunk = chunk * 2;
		do {
			char* out = new char[chunk];
			_strm->avail_out = chunk;
			_strm->next_out = (Bytef*)out;
			_strm_ret = inflate(_strm, Z_NO_FLUSH);    /* no bad return value */
			if (_strm_ret != Z_OK) {
				switch (_strm_ret) {
				case Z_STREAM_ERROR:/* state not clobbered */
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					/* Free memory */
					delete[]out;
					return panic(std::to_string(_total_size).append(_strm->msg).c_str(), TRUE);
					//return panic(_strm->msg);
				}
			}
			have = chunk - _strm->avail_out;
			dest.write(out, have);
			_total_size += have;
			/* Free memory */
			delete[]out;
		} while (_strm->avail_out == 0);
		return len;
	}
	template<class _out_stream>
	inline size_t gzip_inflate::write(_out_stream& dest, const char* buff) {
		char* in = strdup(buff);
		size_t ret = this->write(dest, in, strlen(buff), Z_NO_FLUSH, FALSE);
		/* Free memory */
		delete[]in;
		return ret;
	}
	template<class _out_stream, class _source_stream>
	inline size_t gzip_inflate::write(_out_stream& dest, _source_stream& source, int bypass) {
		if (bypass == FALSE) {
			if (_is_flush == TRUE)return FALSE;
			if (_is_error == TRUE)return -1;
		}
		size_t totalSize = get_size_of_stream(source);
		size_t total_len = totalSize;
		if (total_len == std::string::npos || total_len == 0)return total_len;
		int do_flush = Z_NO_FLUSH;
		size_t ret; size_t read_len = 0; int eof = FALSE;
		int header_len = 10;
		do {
			if (!source.good())break;
			char* in;
			if (_no_footer == FALSE) {
				read_len = totalSize > CHUNK ? CHUNK : totalSize;
				if (totalSize > header_len && read_len > header_len) {
					read_len = read_len - header_len;
				}
			}
			else {
				read_len = totalSize > CHUNK ? CHUNK : totalSize;
			}
			
			in = new char[read_len];
			source.read(in, read_len);
			totalSize -= read_len;
			if (totalSize <= 0) {
				do_flush = Z_FINISH; eof = TRUE;
			}
			if (read_len <= header_len && totalSize <= header_len) {
				/* Assume footer block */
				/* Free memory */
				delete[]in; break;
			}
			ret = this->write(dest, in, read_len, do_flush, TRUE);
			/* Free memory */
			delete[]in;
			if (ret == FALSE || ret == std::string::npos || ret < 0)return ret;
		} while (eof == FALSE);
		return total_len;
	}
	template<class _out_stream>
	inline size_t gzip_inflate::write_file(_out_stream& dest, const std::string file_path) {
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_is_error != FALSE) _is_error = FALSE;
		std::ifstream* file_stream = new std::ifstream(file_path.c_str(), std::ifstream::binary);
		if (!file_stream->is_open()) {
			return panic("Unable to open file....", -1);
		}
		size_t ret = this->write(dest, *file_stream, TRUE);
		file_stream->close(); delete file_stream;
		return ret;
	}
	template<class _out_stream>
	inline int gzip_inflate::flush(_out_stream& dest) {
		if (_is_flush)return FALSE;
		if (_is_error == TRUE)return -1;
		/* stream will be complete */
		if (_strm_ret != Z_STREAM_END) {
			return panic("inflate::stream not completed yet", TRUE);
		}
		_total_size = NULL;
		/* clean up and return */
		(void)inflateEnd(_strm);
		free_zstream(_strm);
		_is_flush = TRUE;
		return TRUE;
	}
	template<class _source_stream>
	inline size_t gzip_inflate::f_write(_source_stream& source) {
		if (this->_fs == NULL) {
			return panic("FileStream not open yet....", -1);
		}
		return this->write(*this->_fs, source, FALSE);
	}
	//[/Inflate==>Decompress]
	//End 12:39 AM 1/11/2020 Inflate And Deflate
}; // namespace gzip
#endif//_zgzip_h
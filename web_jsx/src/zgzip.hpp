/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#pragma once
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
#if !defined(ZUTIL_H)
//#include <zutil.h>
#endif//!ZUTIL_H
#if !defined(_SSTREAM_)
#include <sstream> // std::stringstream
#endif//_SSTREAM_
#if !defined(CHUNK)
#define CHUNK 16384
#endif//!CHUNK
#if !defined(OS_CODE)
#  define OS_CODE  0x03  /* assume Unix */
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
	void _write_magic_header(_out_stream&output) {
		char*dest = (char*)malloc(10);
		sprintf(dest, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
		output.write(const_cast<const char*>(dest), 10);
		free(dest);
	};
	template<class _out_stream>
	int _deflate_stream(std::stringstream&source, _out_stream&dest, int level = Z_BEST_SPEED) {
		//6:08 AM 1/17/2019
		int ret, flush;
		unsigned have;
		z_stream strm;
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit2_(&strm, level, Z_DEFLATED,
			-MAX_WBITS,
			DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK)
			return ret;
		source.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = source.tellg();
		source.seekg(0, std::ios::beg);//Back to begain of stream
		int write_len = 0;
		//bool is_first = true;
		std::streamsize n;
		/* compress until end of stream */
		uLong tcrc = 0;
		do {
			char in[CHUNK];
			n = source.rdbuf()->sgetn(in, CHUNK);
			strm.avail_in = (uInt)n;
			tcrc = crc32(tcrc, (uint8_t*)in, (uInt)n);
			totalSize -= n;
			flush = totalSize <= 0 ? Z_FINISH : Z_NO_FLUSH;
			strm.next_in = (Bytef*)in;
			/* run deflate() on input until output buffer not full, finish
			  compression if all of source has been read in */
			do {
				char out[CHUNK];
				strm.avail_out = CHUNK;
				strm.next_out = (Bytef*)out;
				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = CHUNK - strm.avail_out;
				dest.write(out, have);
				write_len += have;
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */
			 /* done when last data in file processed */
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */
		 /* clean up and return */
		(void)deflateEnd(&strm);
		/* write gzip footer to out stream*/
		dest.write((char*)&tcrc, sizeof(tcrc));
		dest.write((char*)&totalSize, sizeof(totalSize));
		return write_len;
	};
	template<class _out_stream>
	void compress_gzip (std::stringstream&source_stream, _out_stream&out_stream) {
		//_setmode(_fileno(stdout), _O_BINARY);
		SET_BINARY_MODE(stdout);
		_write_magic_header(out_stream);
		_deflate_stream(source_stream, out_stream);
	};
}; // namespace gzip
#endif//_zgzip_h
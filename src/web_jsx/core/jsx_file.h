/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//5:19 PM 3/30/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_jsx_file_h)
#	define _jsx_file_h
//#	include	<fstream>
#	include <io.h> 
#	include <string>
namespace sow_web_jsx {
	enum jsx_file_mode {
		f_read = 0x0001,
		f_write = 0x0002,
		f_append = 0x0003,
		f_read_write = 0x0004,
		f_ukn = -1
	};
	class jsx_file {
	private:
		char* _internal_error;
		//std::ifstream* _read_stream;
		//std::ofstream* _write_stream;
		FILE*_fstream;
		jsx_file_mode _file_mode;
		size_t _total_length;
		int panic(const char* error);
	public:
		int is_flush;
		errno_t err;
		jsx_file(const char*, const char*);
		size_t open(const char* path, const char* mode);
		size_t read(int len, std::string&out);
		int eof();
		size_t write(const char*);
		void flush();
		const char* get_last_error();
		void clear();
		~jsx_file();
	};
}
#endif//_jsx_file_h
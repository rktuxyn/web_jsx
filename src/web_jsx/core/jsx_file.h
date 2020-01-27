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
#	include "web_jsx_global.h"
#	include "v8_util.h"
#	include <v8.h>
namespace sow_web_jsx {
	class jsx_file {
	private:
		FILE*_fstream;
	public:
		int is_flush;
		errno_t err;
		jsx_file(const char*, const char*);
		const char* read();
		size_t write(const char*);
		void flush();
		~jsx_file();
	};
}
#endif//_jsx_file_h
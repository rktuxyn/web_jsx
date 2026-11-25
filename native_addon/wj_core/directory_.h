/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(directory__h)
#	define directory__h
#	include <vector>
#	include	<regex>
#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj
//3:21 PM 12/24/2018
namespace fsys_web_jsx {
	int read_directory_files(const char* name, std::vector<char*>&directory);
	int read_directory_sub_directory(const char* name, std::vector<const char*>&directory, const char* ext);
	int read_directory_sub_directory(const char* name, std::vector<std::string>&directory, const char* ext);
	int read_directory_sub_directory_x(const char* name, std::vector<std::string>&directory, const std::regex&pattern);
	int delete_dir(const char * name);
	int dir_exists(const char* dir);
	int create_directory(const char* dir);
	int rename_dir(const char* old_dir, const char* new_dir);

};
#endif//directory__h
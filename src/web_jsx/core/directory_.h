/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(directory__h)
#define directory__h
#if !defined(_std_wrapper_h)
#include "std_wrapper.hpp"
#endif//!std_wrapper_h
#if !defined(_REGEX_)
#include <regex>
#endif// !_REGEX_
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Not Implemented
#else
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#if !defined(DIRENT_H)
#include<dirent.h>
#endif//DIRENT_H
#if !defined(_INC_DIRECT)
#include <direct.h>
#endif//!_INC_DIRECT
#endif//_WIN32||_WIN64
//3:21 PM 12/24/2018
namespace sow_web_jsx {
	int read_directory_files(const char* name, std::vector<char*>&directory);
	int read_directory_sub_directory(const char* name, std::vector<const char*>&directory, const char* ext);
	int read_directory_sub_directory(const char* name, std::vector<std::string>&directory, const char* ext);
	int read_directory_sub_directory_x(const char* name, std::vector<std::string>&directory, const std::regex&pattern);
	int delete_dir(const char * name);
	int dir_exists(const char* dir);
	int create_directory(const char* dir);

};
#endif//directory__h
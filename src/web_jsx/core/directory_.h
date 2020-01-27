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
#	define directory__h
#	include "std_wrapper.hpp"
#	include <vector>
//3:21 PM 12/24/2018
namespace sow_web_jsx {
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
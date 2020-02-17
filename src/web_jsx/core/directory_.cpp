/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include	"directory_.h"
#include	<regex>
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Not Implemented
#else
#include	<windows.h>
#include	<dirent.h>
#include	<direct.h>
#endif//_WIN32||_WIN64
//3:21 PM 12/24/2018
bool __is_match_extension_x (const std::string& path_str, const std::regex & pattern) {
	return std::regex__ismatch(path_str, pattern);
}
bool __is_match_extension (
	const std::string& path_str, 
	const char* ext
) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return false;
	return strcmp(path_str.substr(found + 1).c_str(), ext) == 0;
}
#if !(defined(_WIN32)||defined(_WIN64))
int sow_web_jsx::read_directory_files(const char* name, std::vector<char*>&directory) {
#error Not Implemented
}
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<const char*>&directory, const char* ext) {
#error Not Implemented
}
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<std::string>&directory, const char* ext) {
#error Not Implemented
}
int sow_web_jsx::dir_exists(const char* dir) {
	struct stat stats;
	stat(dir, &stats);
	// Check for file existence
	if (S_ISDIR(stats.st_mode))
		return 1;
	return 0;
}
int sow_web_jsx::create_directory(const char* dir) {
#error Not Implemented
}
int sow_web_jsx::delete_dir(const char * name) {
#error Not Implemented
}
int sow_web_jsx::read_directory_sub_directory_x(const char * name, std::vector<std::string>& directory, const std::regex& pattern) {
#error Not Implemented
}
#else
int sow_web_jsx::read_directory_files(
	const char* name, 
	std::vector<char*>&directory
) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	struct dirent *entry;
	/* read all the files and directories within directory */
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_DIR) continue;
		char* path = new char[_MAX_PATH];
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		directory.push_back(path);
	}
	delete entry; entry = NULL;
	closedir (dir);
	return EXIT_SUCCESS;
}
int sow_web_jsx::read_directory_sub_directory(
	const char* name, 
	std::vector<const char*>&directory, 
	const char* ext
) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	bool has_ext = strlen(ext) != 0 && strcmp(ext, "A") != 0;
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
			char* path =(char *)malloc(_MAX_PATH);
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			read_directory_sub_directory(const_cast<const char*>(path), directory, ext);
			free(path);
			continue;
		};
		if (has_ext) {
			if (__is_match_extension(entry->d_name, ext) == false)continue;
		}
		char* path = (char *)malloc(_MAX_PATH);
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		directory.push_back(const_cast<const char*>(path));
		free(path);
	}
	closedir (dir);
	return EXIT_SUCCESS;
}
int sow_web_jsx::read_directory_sub_directory(
	const char* name, 
	std::vector<std::string>&directory, 
	const char* ext
) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	bool has_ext = strlen(ext) != 0 && strcmp(ext, "A") != 0;
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (entry->d_type == DT_DIR) {
			if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)
				continue;
			std::string* path = new std::string(name);
			path->append(d_name);
			path->append("\\");
			read_directory_sub_directory(path->c_str(), directory, ext);
			_free_obj(path);
			continue;
		};
		if (has_ext) {
			if (__is_match_extension(d_name, ext) == false)continue;
		}
		std::string* path = new std::string(name);
		path->append(d_name);
		directory.push_back(path->c_str());
		_free_obj(path);
	}
	delete entry; entry = NULL;
	closedir (dir);
	return EXIT_SUCCESS;
}
int sow_web_jsx::dir_exists(const char* dir) {
	DWORD attribs = ::GetFileAttributesA(dir);
	if (attribs == INVALID_FILE_ATTRIBUTES)return -2;
	if (attribs && (attribs & FILE_ATTRIBUTE_DIRECTORY/*16*/))return 1;
	return -1;
}
/*int sow_web_jsx::dir_exists(const char* dir) {
	wchar_t* wString = (wchar_t*)malloc(MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, dir, -1, wString, MAX_PATH);
	if (CreateDirectory(wString, NULL)) {
		free(wString);
		return 1;
	}
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		free(wString);
		return -1;
	}
	free(wString);
	return -3;
}*/
int sow_web_jsx::create_directory(const char* dir) {
	DIR* d = opendir(dir);
	if (d != NULL) {
		closedir(d);
		return -1;
	}
	if (_mkdir(dir) == 0)
		return 2;
	return -2;
}
int sow_web_jsx::delete_dir(const char * name) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	struct dirent *entry;
	int error = 0;
	while ((entry = read_dir (dir)) != NULL) {
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)continue;
		std::string* d_path = new std::string(name);
		d_path->append(d_name);
		d_path->append("\\");
		error = sow_web_jsx::delete_dir(d_path->c_str());
		_free_obj(d_path);
		if (error == EXIT_FAILURE)break;
	}
	delete entry; entry = NULL;
	closedir (dir);
	if (error == EXIT_FAILURE)return EXIT_FAILURE;
	if (_rmdir(name) == 0)
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}
int sow_web_jsx::rename_dir(const char* old_dir, const char* new_dir) {
	int result = rename(old_dir, new_dir);
	return result;
}
int sow_web_jsx::read_directory_sub_directory_x(const char * name, std::vector<std::string>& directory, const std::regex& pattern) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_UNKNOWN)continue;
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (entry->d_type == DT_DIR) {
			if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)
				continue;
			std::string* path = new std::string(name);
			path->append(d_name);
			path->append("\\");
			read_directory_sub_directory_x(path->c_str(), directory, pattern);
			_free_obj(path);
			continue;
		}
		if (__is_match_extension_x(d_name, pattern) == false)continue;
		std::string* path= new std::string(name);
		path->append(d_name);
		directory.push_back(path->c_str());
		_free_obj(path);
	}
	delete entry; entry = NULL;
	closedir (dir);
	return EXIT_SUCCESS;
}
#endif//!_WINDOWS_
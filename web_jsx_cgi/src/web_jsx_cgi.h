/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:10 AM 11/23/2018
#pragma once
#pragma warning(disable : 4996)
#if !defined(_web_jsx_cgi_h)
#define _web_jsx_cgi_h
#if !defined(_web_jsx_cgi_global_h)
#include "web_jsx_cgi_global.h"
#endif//!_web_jsx_cgi_global_h
#if !defined(_npgsql_h)
#include <npgsql.h>
#endif//!_npgsql_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
#if !defined(_util_h)
#include "util.h"
#endif//!_util_h
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
//#define __TEST__
//#undef __TEST__
using namespace std;
using namespace sow_web_jsx::js_compiler;
#if !(defined(_WIN32) || defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
bool is_user_interactive();
void print_info();
#else
#if !defined(_WINCON_)
#include <Wincon.h>
#endif//_WINCON_
#define FOREGROUND_BLACK			0x0000 // text color contains black.
#define FOREGROUND_YELLOW			0x0006 // text color contains DarkYellow.
#define FOREGROUND_DARK_YELLOW		0x0007 // text color contains DarkYellow.
#define FOREGROUND_LIGHT_GREEN		0XA // text color contains LightGreen.
#define FOREGROUND_LIGHT_RED		0XC // text color contains LightGreen.
BOOL is_user_interactive();
WORD get_current_console_color(HANDLE hConsole);
void print_info();
#endif//!_WIN32||_WIN64
int main(int argc, char *argv[], char*envp[]);
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#endif//!_web_jsx_cgi_global_h
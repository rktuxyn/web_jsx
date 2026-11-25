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
//6:10 AM 11/23/2018
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#pragma warning(disable : 4996)
#if !defined(_web_jsx_fcgi_h)
#	define _web_jsx_fcgi_h
#	include "web_jsx_app_core.h"
#if defined(FAST_CGI_APP)
namespace fsys_web_jsx {
	namespace fcgi_request {
		int request_handler(
			const char*execute_path, 
			const char*path, int is_spath,
			const char* web_app_root,
			int is_interactive
		);
	}
}
#endif//!FAST_CGI_APP
#endif//!_web_jsx_global_h

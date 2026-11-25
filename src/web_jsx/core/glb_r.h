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
//2:34 AM 4/17/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER

#if !defined(_glb_r_h)
#	define _glb_r_h

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

namespace fsys_web_jsx {
	/*[function pointers]*/
	typedef void (*add_resource_func)();
	/*[/function pointers]*/
	void free_resource();
	EXPORT_WJSX void register_resource(add_resource_func func);
}

#endif//_glb_r_h
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
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

namespace sow_web_jsx {
	/*[function pointers]*/
	typedef void (*add_resource_func)();
	/*[/function pointers]*/
	void free_resource();
	EXPORT_WJSX void register_resource(add_resource_func func);
}

#endif//_glb_r_h
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//11:26 PM 2/27/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_listener_win_h)
#	define _listener_win_h
int create_nonblocking_threads(const char* execute_path, int is_fserver);
#endif
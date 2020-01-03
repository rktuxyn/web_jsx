/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//11:50 PM 12/13/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_mysql_wrapper_h)
#define _mysql_wrapper_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
#if !defined(_v8_util_h)
#include "v8_util.h"
#endif//!v8_util_h
#if !defined(_my_sql_h)
#include <my_sql.h>
#endif//!_my_sql_h
namespace sow_web_jsx {
	void mysql_bind(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx);
}
#endif//!_mysql_wrapper_h
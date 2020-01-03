/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//10:42 PM 12/13/2019
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_npgsql_wrapper_h)
#define _npgsql_wrapper_h
#if !defined(_v8_util_h)
#include "v8_util.h"
#endif//!v8_util_h
#if !defined(_npgsql_h)
#include <npgsql.h>
#endif//!_npgsql_h
#if !defined(_npgsql_pool_h)
#include <npgsql_pool.h>
#endif//!_npgsql_pool_h
#if !defined(_npgsql_tools_h)
#include <npgsql_tools.h>
#endif//_npgsql_tools_h
namespace sow_web_jsx {
	//
	// Summary:
	//		Executes the stored procedure @param sp_str and returns the first row as json string
	//		Opens a database connection with the connection settings specified by the @param con_str.
	//		@param con_str define the npgsql connection string
	//		@param sp_str define the specific stored procedure name
	//		@param ctx_str define the json string as stored procedure first param
	//		@param form_data_str define the json string as stored procedure second param
	//		@returns The first column of the first row as json string. e.g. {_ret_val number; _ret_msg string; _ret_data_table json string}
	void npgsql_execute_io(const v8::FunctionCallbackInfo<v8::Value>& args);
	void npgsql_execute_scalar(const v8::FunctionCallbackInfo<v8::Value>& args);
	void npgsql_execute_query(const v8::FunctionCallbackInfo<v8::Value>& args);
	//
	// Summary:
	//		Executes the query @param query_str and give each row in @param func
	//		Opens a database connection with the connection settings specified by the @param con_str.
	//		@param con_str define the npgsql connection string
	//		@param query_str define the PgSQL query string
	//		@param func define the Function callback with param @param index is the number row and @param row is the data row array [columns]
	void npgsql_data_reader(const v8::FunctionCallbackInfo<v8::Value>& args);
	void npgsql_bind(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx);
}
#endif//!_npgsql_wrapper_h
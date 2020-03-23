/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "npgsql_wrapper.h"
#	include <npgsql.h>
#	include <npgsql_pool.h>
#	include <npgsql_tools.h>
#	include <npgsql_query.h>
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
//11:33 PM 12/13/2019
using namespace sow_web_jsx;
template<class _pg_connection>
int create_pgsql_connection(v8::Isolate* isolate, _pg_connection* pgsql, const char* conn) {
	int rec = pgsql->connect(conn);
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		return rec;
	}
	return rec;
}
//
// Summary:
//		Executes the stored procedure @param sp_str and returns the first row as json string
//		Opens a database connection with the connection settings specified by the @param con_str.
//		@param con_str define the npgsql connection string
//		@param sp_str define the specific stored procedure name
//		@param ctx_str define the json string as stored procedure first param
//		@param form_data_str define the json string as stored procedure second param
//		@returns The first column of the first row as json string. e.g. {_ret_val number; _ret_msg string; _ret_data_table json string}
template<class _pg_query>
int _npgsql_execute_io(
	v8::Isolate* isolate,
	_pg_query* pgsql,
	const char* sp,
	const char* user_ctx,
	const char* form_data,
	v8::Handle<v8::Object> v8_result
) {
	std::map<std::string, char*>* result = new std::map<std::string, char*>();
	int rec = pgsql->execute_io(sp, user_ctx, form_data, *result);
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		result->clear(); delete result;
		return rec;
	}
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	for (auto itr = result->begin(); itr != result->end(); ++itr) {
		v8_result->Set(
			ctx,
			v8_str(isolate, itr->first.c_str()),
			v8_str(isolate, const_cast<const char*>(itr->second))
		);
	}
	result->clear(); delete result;
	return 0;
}
template<class _pg_query>
int _npgsql_execute_scalar(
	v8::Isolate* isolate,
	_pg_query* pgsql,
	const char* sp,
	v8::Local<v8::Array> paramArray,
	v8::Handle<v8::Object> v8_result
) {
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	std::list<npgsql_params*>* sql_param = new std::list<npgsql_params*>();
	for (uint32_t i = 0, l = paramArray->Length(); i < l; i++) {
		v8::Local<v8::Value> obj_val = paramArray->Get(ctx, i).ToLocalChecked();
		if (obj_val->IsNullOrUndefined() || !obj_val->IsObject())continue;
		//{"name":"login_id","value":"rajibs","db_type":22}
		v8::Local<v8::Object>v8_obj = v8::Handle<v8::Object>::Cast(obj_val);
		v8::Local<v8::Value> db_type_val = v8_obj->Get(ctx, v8_str(isolate, "npgsql_db_type")).ToLocalChecked();
		if (db_type_val->IsNullOrUndefined() || !db_type_val->IsNumber()) {
			throw_js_error(isolate, "Param npgsql_db_type required!!!");
			return -1;
		}
		int db_type = static_cast<int>(db_type_val->ToNumber(ctx).ToLocalChecked()->Value());
		v8::Local<v8::Value> v8_val = v8_obj->Get(ctx, v8_str(isolate, "parameter_direction")).ToLocalChecked();
		if (v8_val->IsNullOrUndefined() || !v8_val->IsNumber()) {
			throw_js_error(isolate, "Param parameter_direction required!!!");
			return -1;
		}
		std::string str_val;
		int p_direction = static_cast<int>(v8_val->ToNumber(ctx).ToLocalChecked()->Value());
		if (p_direction == parameter_direction::Output ||
			p_direction == parameter_direction::ReturnValue) {
			get_prop_value(isolate, ctx, v8_obj, "parameter_name", str_val);
			sql_param->push_back(new npgsql_params(
				/*name*/ str_val.c_str(),
				/*dtype*/(npgsql_db_type)db_type,
				/*pd*/(parameter_direction)p_direction
			));
		}
		else {
			get_prop_value(isolate, ctx, v8_obj, "parameter_name", str_val);
			std::string data;
			get_prop_value(isolate, ctx, v8_obj, "data", data);
			sql_param->push_back(new npgsql_params(
				/*name*/ str_val.c_str(),
				/*dtype*/(npgsql_db_type)db_type,
				/*pd*/(parameter_direction)p_direction,
				/*data*/data.c_str()
			));
		}
		obj_val.Clear();
		v8_obj.Clear();
	}
	//native_string sp(isolate, args[1]);
	std::map<std::string, char*>* result = new std::map<std::string, char*>();
	int rec = pgsql->execute_scalar(sp, *sql_param, *result);
	sql_param->clear(); delete sql_param;
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		result->clear(); delete result;
		return rec;
	}
	for (auto itr = result->begin(); itr != result->end(); ++itr) {
		v8_result->Set(
			ctx,
			v8_str(isolate, itr->first.c_str()),
			v8_str(isolate, const_cast<const char*>(itr->second))
		);
	}
	result->clear(); delete result;
	return rec;
}
//
// Summary:
//		Executes the query @param query_str and give each row in @param func
//		Opens a database connection with the connection settings specified by the @param con_str.
//		@param con_str define the npgsql connection string
//		@param query_str define the PgSQL query string
//		@param func define the Function callback with param @param index is the number row and @param row is the data row array [columns]
template<class _pg_query>
int _npgsql_data_reader(
	v8::Isolate* isolate,
	_pg_query* pgsql,
	const char* query,
	v8::Local<v8::Object>global,
	v8::Local<v8::Function> callback
) {
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	std::list<npgsql_param_type*>* _sql_param = new std::list<npgsql_param_type*>();
	int rec = pgsql->execute_scalar_l(query, *_sql_param, [&isolate, &callback, &global, &ctx](int i, std::vector<char*>& rows) {
		std::string* row_str = new std::string();
		json_array_stringify_s(rows, *row_str);
		v8::Handle<v8::Value> arg[2] = {
			v8::Number::New(isolate, i),
			v8_str(isolate, row_str->c_str())
		};
		callback->Call(ctx, global, 2, arg);
		delete row_str;
		return;
		});
	delete _sql_param;
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
	}
	return rec;
}
//
// Summary:
//		Executes the stored procedure @param sp_str and returns the first row as json string
//		Opens a database connection with the connection settings specified by the @param con_str.
//		@param con_str define the npgsql connection string
//		@param sp_str define the specific stored procedure name
//		@param ctx_str define the json string as stored procedure first param
//		@param form_data_str define the json string as stored procedure second param
//		@returns The first column of the first row as json string. e.g. {_ret_val number; _ret_msg string; _ret_data_table json string}
V8_JS_METHOD(npgsql_execute_io) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		
		throw_js_error(isolate, "Connection string required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Stored procedure required!!!");
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Authentication context required!!!");
		return;
	}
	if (!args[3]->IsString() || args[3]->IsNullOrUndefined()) {
		throw_js_error(isolate, "form_data required!!!");
		return;
	}
	npgsql* pgsql = new npgsql();
	native_string vcon(isolate, args[0]);
	int rec = create_pgsql_connection(isolate, pgsql, vcon.c_str());
	vcon.clear();
	if (rec < 0) {
		pgsql->close(); delete pgsql;
		return;
	}
	native_string sp(isolate, args[1]);
	native_string user_ctx(isolate, args[2]);
	native_string form_data(isolate, args[3]);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	rec = _npgsql_execute_io(isolate, pgsql,
		sp.c_str(), user_ctx.c_str(),
		form_data.c_str(),
		v8_result
	);
	sp.clear(); user_ctx.clear(); form_data.clear();
	if (rec >= 0) {
		args.GetReturnValue().Set(v8_result);
	}
	v8_result.Clear();
	pgsql->close();
	delete pgsql;

}
V8_JS_METHOD(npgsql_execute_scalar) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Connection string required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Stored procedure required!!!");
		return;
	}
	if (!args[2]->IsArray() || args[2]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Param Array required!!!");
		return;
	}
	npgsql* pgsql = new npgsql();
	//malloc(sizeof pgsql);
	native_string vcon(isolate, args[0]);
	//v8::String::Utf8Value vcon_str(isolate, args[0]);
	int rec = create_pgsql_connection(isolate, pgsql, vcon.c_str());
	vcon.clear();
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		pgsql->close(); delete pgsql;
		return;
	}
	native_string sp(isolate, args[1]);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	v8::Local<v8::Array> paramArray = v8::Handle<v8::Array>::Cast(args[2]);
	rec = _npgsql_execute_scalar(isolate, pgsql, sp.c_str(), paramArray, v8_result);
	paramArray.Clear(); sp.clear();
	if (rec >= 0) {
		args.GetReturnValue().Set(v8_result);
	}
	v8_result.Clear();
	pgsql->close(); delete pgsql;
	return;
}
V8_JS_METHOD(npgsql_execute_query) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Connection string required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Query string required!!!");
		return;
	}
	npgsql* pgsql = new npgsql();
	//malloc(sizeof pgsql);
	native_string vcon(isolate, args[0]);
	//v8::String::Utf8Value vcon_str(isolate, args[0]);
	int rec = create_pgsql_connection(isolate, pgsql, vcon.c_str());
	vcon.clear();
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		pgsql->close(); delete pgsql;
		return;
	}
	native_string query(isolate, args[1]);
	const char* result = pgsql->execute_query(query.c_str(), rec);
	query.clear();
	if (rec < 0) {
		throw_js_error(isolate, pgsql->get_last_error());
		pgsql->close();
		delete pgsql;
		return;
	};
	pgsql->close();
	delete pgsql;
	if (((result != NULL) && (result[0] == '\0')) || (result == NULL)) {
		args.GetReturnValue().Set(v8::Number::New(isolate, 1));
		return;
	}
	args.GetReturnValue().Set(v8_str(isolate, result));
	return;
}
//
// Summary:
//		Executes the query @param query_str and give each row in @param func
//		Opens a database connection with the connection settings specified by the @param con_str.
//		@param con_str define the npgsql connection string
//		@param query_str define the PgSQL query string
//		@param func define the Function callback with param @param index is the number row and @param row is the data row array [columns]
V8_JS_METHOD(npgsql_data_reader) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Connection string required!!!");
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		throw_js_error(isolate, "Query string required!!!");
		return;
	}
	if (!args[2]->IsFunction()) {
		throw_js_error(isolate, "Callback required!!!");
		return;
	}
	npgsql* pgsql = new npgsql();
	//malloc(sizeof pgsql);
	native_string vcon(isolate, args[0]);
	//v8::String::Utf8Value vcon_str(isolate, args[0]);
	int rec = create_pgsql_connection(isolate, pgsql, vcon.c_str());
	vcon.clear();
	if (rec < 0) {
		pgsql->close();
		delete pgsql;
		return;
	}
	//5:38 AM 11/21/2018
	native_string query(isolate, args[1]);
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[2]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	v8::Local<v8::Object>global = args.Holder();
	rec = _npgsql_data_reader(isolate, pgsql, query.c_str(), global, callback);
	query.clear(); global.Clear(); cb.Reset(); callback.Clear();
	if (rec >= 0) {
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
	}
	pgsql->close();  delete pgsql;
}
npgsql_query* _create_query(v8::Isolate* isolate, npgsql_connection* pg_con) {
	if (pg_con->conn_state() != connection_state::OPEN) {
		throw_js_error(isolate, "No active connection state found...");
		return NULL;
	}
	pg_connection_pool* cpool = pg_con->create_connection_pool();
	if (cpool->error_code < 0) {
		throw_js_error(isolate, cpool->error_msg);
		pg_con->exit_nicely(cpool);
		return NULL;
	}
	return new npgsql_query(cpool);
}
npgsql_connection* get_conn_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
	npgsql_connection* pg_conn = sow_web_jsx::unwrap<npgsql_connection>(args);
	if (pg_conn == NULL) {
		throw_js_error(args.GetIsolate(), "npgsql_connection object disposed...");
		return NULL;
	}
	return pg_conn;
}
void sow_web_jsx::npgsql_export(v8::Isolate* isolate, v8::Handle<v8::Object> target) {
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args.IsConstructCall()) {
			iso->ThrowException(v8::Exception::TypeError(v8_str(iso, "Cannot call constructor as function!!!")));
			return;
		}
		v8::Local<v8::Object> obj = args.This();
		npgsql_connection* pg_conn = new npgsql_connection();
		obj->SetInternalField(0, v8::External::New(iso, pg_conn));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(iso, obj);
		pobj.SetWeak<npgsql_connection*>(&pg_conn, [](const v8::WeakCallbackInfo<npgsql_connection*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	appTemplate->SetClassName(v8_str(isolate, "PgSql"));
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = appTemplate->PrototypeTemplate();
	set_prototype(isolate, prototype, "connect", [](js_method_args) {
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			throw_js_error(args.GetIsolate(), "Connection string should not left blank!!!");
			return;
		}
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		v8::Isolate* isolate = args.GetIsolate();
		native_string vcon(isolate, args[0]);
		int rec = create_pgsql_connection(isolate, pg_conn, vcon.c_str());//pg_conn->connect(vcon.c_str());
		vcon.clear();
		if (rec < 0)return;
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "re_connect", [](js_method_args) {
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		int rec = pg_conn->connect();
		if (rec < 0) {
			throw_js_error(args.GetIsolate(), pg_conn->get_last_error());
			return;
		}
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "execute_scalar", [](js_method_args) {
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Stored procedure required!!!");
			return;
		}
		if (!args[1]->IsArray() || args[1]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Param Array required!!!");
			return;
		}
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		v8::Isolate* isolate = args.GetIsolate();
		npgsql_query* pg_query = _create_query(isolate, pg_conn);
		if (pg_query == NULL)return;
		int rec = 0;
		native_string sp(isolate, args[0]);
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8::Local<v8::Array> paramArray = v8::Handle<v8::Array>::Cast(args[1]);
		rec = _npgsql_execute_scalar(isolate, pg_query, sp.c_str(), paramArray, v8_result);
		pg_query->free_connection();
		paramArray.Clear(); sp.clear();
		if (rec >= 0) {
			args.GetReturnValue().Set(v8_result);
		}
		v8_result.Clear();
		delete pg_query;
	});
	set_prototype(isolate, prototype, "execute_query", [](js_method_args) {
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Query string required!!!");
			return;
		}
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		v8::Isolate* isolate = args.GetIsolate();
		npgsql_query* pg_query = _create_query(isolate, pg_conn);
		if (pg_query == NULL)return;
		int rec = 0;
		native_string query(isolate, args[0]);
		const char* result = pg_query->execute_query(query.c_str(), rec);
		query.clear(); pg_query->free_connection();
		if (rec < 0) {
			throw_js_error(isolate, pg_query->get_last_error());
			delete pg_query;
			return;
		};
		if (result == NULL || strlen(result) == 0) {
			args.GetReturnValue().Set(v8::Number::New(isolate, 1));
			delete pg_query;
			return;
		}
		args.GetReturnValue().Set(v8_str(isolate, result));
		delete pg_query;
		return;
	});
	set_prototype(isolate, prototype, "data_reader", [](js_method_args) {
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Query string required!!!");
			return;
		}
		if (!args[1]->IsFunction()) {
			throw_js_error(args.GetIsolate(), "Callback required!!!");
			return;
		}
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		v8::Isolate* isolate = args.GetIsolate();
		npgsql_query* pg_query = _create_query(isolate, pg_conn);
		if (pg_query == NULL)return;
		int rec = 0;
		native_string query(isolate, args[0]);
		v8::Persistent<v8::Function> cb;
		cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[1]));
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
		v8::Local<v8::Object>global = args.Holder();
		rec = _npgsql_data_reader(
			isolate, pg_query, query.c_str(), global, callback
		);
		pg_query->free_connection(); delete pg_query;
		query.clear(); global.Clear(); cb.Reset(); callback.Clear();
		if (rec >= 0) {
			args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		}
	});
	set_prototype(isolate, prototype, "execute_io", [](js_method_args) {
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Stored procedure required!!!");
			return;
		}
		if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "Authentication context required!!!");
			return;
		}
		if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
			throw_js_error(args.GetIsolate(), "form_data required!!!");
			return;
		}
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		v8::Isolate* isolate = args.GetIsolate();
		npgsql_query* pg_query = _create_query(isolate, pg_conn);
		if (pg_query == NULL)return;
		int rec = 0;
		native_string sp(isolate, args[0]);
		native_string user_ctx(isolate, args[1]);
		native_string form_data(isolate, args[2]);
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		rec = _npgsql_execute_io(isolate, pg_query,
			sp.c_str(), user_ctx.c_str(),
			form_data.c_str(),
			v8_result
		);
		pg_query->free_connection(); delete pg_query;
		sp.clear(); user_ctx.clear(); form_data.clear();
		if (rec < 0)return;
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
	});
	set_prototype(isolate, prototype, "release_connection", [](js_method_args) {
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		pg_conn->close_all_connection();
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "exit_nicely", [](js_method_args) {
		npgsql_connection* pg_conn = get_conn_instance(args);
		if (pg_conn == NULL)return;
		pg_conn->exit_all();
		args.GetReturnValue().Set(args.Holder());
	});
	v8::Local<v8::Context>context = isolate->GetCurrentContext();
	target->Set(context, v8_str(isolate, "PgSql"), appTemplate->GetFunction(context).ToLocalChecked());
	v8::Handle<v8::Object> npgsql_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, npgsql_object, "execute_io", npgsql_execute_io);
	wjsx_set_method(isolate, npgsql_object, "execute_scalar", npgsql_execute_scalar);
	wjsx_set_method(isolate, npgsql_object, "execute_query", npgsql_execute_query);
	wjsx_set_method(isolate, npgsql_object, "data_reader", npgsql_data_reader);
	wjsx_set_object(isolate, target, "npgsql", npgsql_object);
}
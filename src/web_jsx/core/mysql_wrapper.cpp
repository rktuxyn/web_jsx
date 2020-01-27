/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "mysql_wrapper.h"
#	include <npgsql_tools.h>
#	include <my_sql.h>
void sow_web_jsx::mysql_bind(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx) {
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args.IsConstructCall()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, "Cannot call constructor as function!!!")));
			return;
		}
		v8::Local<v8::Object> obj = args.This();
		my_sql* sql = new my_sql();
		obj->SetInternalField(0, v8::External::New(iso, sql));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(iso, obj);
		pobj.SetWeak<my_sql*>(&sql, [](const v8::WeakCallbackInfo<my_sql*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	tpl->SetClassName(v8_str(isolate, "MySql"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
	prototype->Set(isolate, "connect", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		try {
			if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8_str(iso, "Object required!!!")));
				return;
			}
			v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
			v8::Local<v8::Context>ctx = iso->GetCurrentContext();
			native_string host(iso, config->Get(ctx, v8_str(iso, "host")).ToLocalChecked());
			if (host.is_empty()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(iso, "Database Host required!!!")));
				return;
			}
			native_string database(iso, config->Get(ctx, v8_str(iso, "database")).ToLocalChecked());
			if (database.is_empty()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(iso, "Database name required!!!")));
				return;
			}
			native_string user(iso, config->Get(ctx, v8_str(iso, "user")).ToLocalChecked());
			if (user.is_empty()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(iso, "User name required!!!")));
				return;
			}
			native_string password(iso, config->Get(ctx, v8_str(iso, "password")).ToLocalChecked());
			if (password.is_empty()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(iso, "Password required!!!")));
				return;
			}
			v8::Local<v8::Value> port_v8 = config->Get(ctx, v8_str(iso, "port")).ToLocalChecked();
			if (port_v8->IsNullOrUndefined() || !port_v8->IsNumber()) {
				iso->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(iso, "Database port number required!!!")));
				return;
			}
			my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
			mysqlw::connection_details* con = new mysqlw::connection_details();
			con->database = new std::string(database.c_str());// "test";
			con->host = new std::string(host.c_str());//"localhost";
			con->user = new std::string(user.c_str());//"root";
			con->password = new std::string(password.c_str());//"mysql123";
			con->unix_socket = NULL;
			con->port = static_cast<int>(port_v8->ToNumber(ctx).ToLocalChecked()->Value());
			con->clientflag = 0;
			if (sql->connect(con) == connection_state::CLOSED) {
				iso->ThrowException(v8::Exception::Error(
					v8::String::NewFromUtf8(iso, sql->get_last_error())));
				return;
			};
			args.GetReturnValue().Set(args.Holder());
		}
		catch (std::exception& e) {
			iso->ThrowException(v8::Exception::Error(
				v8_str(iso, e.what())));
			return;
		}
		catch (...) {
			iso->ThrowException(v8::Exception::Error(
				v8_str(iso, "Unknown error...")));
			return;
		}
	}));
	prototype->Set(isolate, "switch_database", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, "Database name required!!!")));
			return;
		}
		native_string database_name(iso, args[0]);
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		if ( sql->switch_database(database_name.c_str()) < 0) {
			database_name.clear();
			iso->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(iso, sql->get_last_error())));
			return;
		}
		database_name.clear();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "re_connect", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		if (sql->connect() == connection_state::CLOSED) {
			iso->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(iso, sql->get_last_error())));
			return;
		};
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "close_all_connection", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		sql->close_all_connection();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "exec", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, "Sql statement required...")));
			return;
		}
		native_string query_str(iso, args[0]);
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		const char* result = sql->execute(query_str.c_str());
		query_str.clear();
		if ( sql->has_error() ) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, sql->get_last_error())));
			return;
		}
		if (((result != NULL) && (result[0] == '\0')) || (result == NULL)) {
			args.GetReturnValue().Set(v8::Number::New(iso, 0));
		}
		else {
			args.GetReturnValue().Set(v8_str(iso, result));
		}
	}));
	prototype->Set(isolate, "execute_query", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		v8::Isolate* iso = args.GetIsolate();
		if (sql->state() == connection_state::CLOSED) {
			iso->ThrowException(v8::Exception::Error(
				v8_str(iso, "No active connection state found...")));
			return;
		}
		if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, "Sql statement required...")));
			return;
		}
		if (args[1]->IsNullOrUndefined() || !args[1]->IsFunction()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, "Callback required...")));
			return;
		}
		v8::Persistent<v8::Function> cb;
		cb.Reset(iso, v8::Local<v8::Function>::Cast(args[1]));
		v8::Local<v8::Object>global = args.Holder();
		v8::Local<v8::Context> ctx = iso->GetCurrentContext();
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(iso, cb);
		native_string query_str(iso, args[0]);
		int rs = sql->execute(query_str.c_str(), [&iso, &callback, &global, &ctx](int index, std::vector<char*>& rows) {
			std::string* row_str = new std::string();
			json_array_stringify_s(rows, *row_str);
			v8::Handle<v8::Value> arg[2] = {
				v8::Number::New(iso, index),
				v8_str(iso, row_str->c_str())
			};
			callback->Call(ctx, global, 2, arg);
			delete row_str;
			return;
		});
		query_str.clear();
		if (rs < 0) {
			iso->ThrowException(v8::Exception::TypeError(
				v8_str(iso, sql->get_last_error())));
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(iso, (double)rs));
	}));
	prototype->Set(isolate, "clear", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		my_sql* sql = sow_web_jsx::unwrap<my_sql>(args);
		//v8::Isolate* iso = args.GetIsolate();
		sql->exit_all();
		/* Return this */
		args.GetReturnValue().Set(args.Holder());
	}));
	ctx->Set(isolate, "MySql", tpl);
}
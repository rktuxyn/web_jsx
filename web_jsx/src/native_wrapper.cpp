/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:46 AM 11/21/2018
#include "native_wrapper.h"
std::string* _root_dir;
void __get_server_map_path(const char* req_path, std::string&output) {
	output.append(_root_dir->data());
	output.append(req_path);
	output = std::regex_replace(output, std::regex("(?:/)"), "\\");
};
static v8::Local<v8::String> ___get_msg(v8::Isolate* isolate, const char* a, const char*b) {
	char *msg = (char*)malloc(strlen(a) + strlen(b));
	sprintf(msg, "%s%s", a, b);
	v8::Local<v8::String> val = v8::String::NewFromUtf8(isolate, const_cast<const char*>(msg));
	free(msg);
	return val;
};

void npgsql_execute_io(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Connection string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Stored procedure required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Authentication context required!!!")));
		return;
	}
	if (!args[3]->IsString() || args[3]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "form_data required!!!")));
		return;
	}
	npgsql* pgsql = new npgsql();
	malloc(sizeof pgsql);
	int rec = !pgsql->is_iniit ? -1 : 0;
	if (rec < 0) {
		//std::cout << "UNABLE TO LOAD PGSQL LIB==>" << pgsql->get_last_error() << "\r\n";
		free(pgsql);
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unable to initialize npgsql instance!!!")));
		return;
	}
	v8::String::Utf8Value vcon_str(isolate, args[0]);
	rec = pgsql->connect(*vcon_str);
	if (rec < 0 || pgsql->conn_state != connection_state::OPEN) {
		//std::cout << "Unable to connect db==>" << pgsql->get_last_error() << "\r\n";
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	std::map<std::string, char*>* result = new std::map<std::string, char*>();
	malloc(sizeof result);
	v8::String::Utf8Value sp_str(isolate, args[1]);
	v8::String::Utf8Value ctx_str(isolate, args[2]);
	v8::String::Utf8Value form_data_str(isolate, args[3]);
	//int npgsql::execute_io(const char * sp, const char * ctx, const char * form_data, std::map<std::string, char*>& result)
	rec = pgsql->execute_io(*sp_str, *ctx_str, *form_data_str, *result);
	if (rec < 0 ) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql); free(result);
		return;
	};
	pgsql->close();
	free(pgsql);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	for (auto itr = result->begin(); itr != result->end(); ++itr) {
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, itr->first.c_str()), 
			v8::String::NewFromUtf8(isolate, const_cast<const char*>(itr->second))
		);
	}
	delete result;
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear();
};
void npgsql_execute_scalar(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not implemented yet!!!")));
	return;
};
void npgsql_execute_non_query(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not implemented yet!!!")));
};
void npgsql_data_reader(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Connection string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Query string required!!!")));
		return;
	}
	if (!args[2]->IsFunction()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Callback required!!!")));
		return;
	}
	std::list<npgsql_param_type*>* _sql_param = new std::list<npgsql_param_type*>();
	v8::Persistent<v8::Function> cb;
	
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[2]));
	npgsql* pgsql = new npgsql();
	malloc(sizeof pgsql);
	int rec = !pgsql->is_iniit ? -1 : 0;
	if (rec < 0) {
		free(pgsql);
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Unable to initialize npgsql instance!!!")));
		return;
	}
	v8::String::Utf8Value vcon_str(isolate, args[0]);
	rec = pgsql->connect(*vcon_str);
	if (rec < 0 || pgsql->conn_state != connection_state::OPEN) {
		//std::cout << "Unable to connect db==>" << pgsql->get_last_error() << "\r\n";
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	v8::String::Utf8Value query_str(isolate, args[1]);
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	//5:38 AM 11/21/2018
	v8::Local<v8::Object>global = context->Global();
	rec = pgsql->execute_scalar_l(*query_str, *_sql_param, [&isolate, &callback, &global](int i, std::vector<char*>&rows) {
		std::string* row_str = new std::string();
		json_array_stringify_s(rows, *row_str);
		v8::Handle<v8::Value> arg[2] = { 
			v8::Number::New(isolate, i), 
			sow_web_jsx::v8_str(isolate, row_str->c_str())
		};
		callback->Call(global, 2, arg);
		return;
	});
	pgsql->close();
	if (rec < 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	free(pgsql);
	context.Clear(); global.Clear();
	callback.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void response_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Response string required!!!")));
		return;
	}
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	std::cout << *utf8_str << std::endl;
	return;
};
v8::Local<v8::Context> sow_web_jsx::wrapper::get_context(v8::Isolate* isolate) {
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> inner_object = v8::ObjectTemplate::New(isolate);
	inner_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	inner_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	inner_object->Set(isolate, "execute_non_query", v8::FunctionTemplate::New(isolate, npgsql_execute_non_query));
	inner_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate,npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", inner_object);
	v8_global->Set(isolate, "response_write", v8::FunctionTemplate::New(isolate, response_write));
	return v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
};
int is_flush = 0;
std::stringstream body_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
std::map<std::string, std::string>* _headers;
std::vector<std::string>* _cookies;
std::vector<std::string>* _http_status;
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
void ___add_http_status(std::vector<std::string>&http_status, std::string&values) {
	if (!http_status.empty()) {
		http_status.clear();
	}
	http_status.push_back(values);
};
int ___write_http_status(std::vector<std::string>&http_status, bool check_status = false) {
	int rec = 0;
	for (size_t i = 0; i < http_status.size(); ++i) {
		if (rec == 0) {
			std::string&str = http_status[i];
			if (str.find("401") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("403") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("404") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("501") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("301") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("304") != std::string::npos) {
				rec = -1;
			}
			else if (str.find("303") != std::string::npos) {
				rec = -1;
			}
		}
		if (!check_status) {
			std::cout << http_status[i] << "\n";
		}
	}
	return rec;
};
void ___add_header(std::map<std::string, std::string>&header, const std::string&key, const std::string&values) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase (it);
	}
	header[key] = values;
};
void __remove_header(std::map<std::string, std::string>&header, const std::string&key) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase (it);
	}
}
int _is_gzip_encoding(std::map<std::string, std::string>&header) {
	if (header.empty())return 0;
	int rec = 0;
	auto it = header.find("Content-Encoding");
	if (it != header.end()) {
		if (it->second == "gzip" || it->second == "deflate") {
			return 1;
		}
	}
	return rec;
};
void __write_header(std::map<std::string, std::string>&header) {
	if (header.empty())return;
	for (auto it = header.begin(); it != header.end(); ++it) {
		//if (it->second == "gzip")continue;
		std::cout << it->first << ":" << it->second << "\n";
	}
	return;
};
void __write_cookies(std::vector<std::string>&cookies) {
	if (cookies.empty())return;
	for (size_t i = 0; i < cookies.size(); ++i){
		std::cout << cookies[i] << "\n";
	}
};
int __write_file(const char*path, const char*buffer) {
	FILE*fstream;
	errno_t err;
	err = fopen_s(&fstream, path, "w+");
	if (err != 0) return -1;
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), fstream);
	if (ferror(fstream)) {
		fclose(fstream);
		fstream = NULL;
		return -1;
	}
	fclose (fstream);
	fstream = NULL;
	return static_cast<int>(len);
};
void read_directory_regx(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Match pattern required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	v8::String::Utf8Value utf_ext_str(isolate, args[1]);
	auto reg = new std::regex(*utf_ext_str);//"(html|aspx|jsx|php)"
	rec = sow_web_jsx::read_directory_sub_directory_x(abs_path->data(), *directorys, *reg);
	free(reg);
	free(abs_path);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (rec == EXIT_FAILURE) {
		free(directorys);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			___get_msg(isolate, "Could not open directory==>", abs_path->data())
		);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	std::vector<std::string>&json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set((int)i, sow_web_jsx::v8_str(isolate, json_obj[i].data()));
	}
	free(directorys);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
};
void read_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	if (args[1]->IsString() || !args[1]->IsNullOrUndefined()) {
		v8::String::Utf8Value utf_ext_str(isolate, args[1]);
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->data(), *directorys, *utf_ext_str);
	}
	else {
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->data(), *directorys, "A");
	}
	
	free(abs_path);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (rec == EXIT_FAILURE) {
		free(directorys);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			___get_msg(isolate, "Could not open directory==>", abs_path->data())
		);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	std::vector<std::string>&json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l= json_obj.size(); i < l; ++i) {
		directory_v8_array->Set((int)i, sow_web_jsx::v8_str(isolate, json_obj[i].data()));
	}
	free(directorys);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
};
void native_read_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	char*data = sow_web_jsx::read_file(abs_path->c_str(), true);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (strcmp(data, "INVALID") == 0) {
		delete data;
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			___get_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->data())
		);
		args.GetReturnValue().Set(v8_result);
		delete abs_path;
		return;
	}
	delete abs_path;
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "message"),
		v8::String::NewFromUtf8(isolate, "Success...")
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "data"),
		v8::String::NewFromUtf8(isolate, const_cast<const char*>(data))
	);
	args.GetReturnValue().Set(v8_result);
	delete data;
	return;
};
void native_write_from_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), body_stream,  true);
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(___get_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->data())));
		delete abs_path;
		return;
	}
	delete abs_path;
	return;
};
void native_write_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File data required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	v8::String::Utf8Value utf_data_str(isolate, args[1]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	FILE*fstream;
	errno_t err;
	err = fopen_s(&fstream, abs_path->data(), "w+");
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (err != 0) {
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			___get_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path->data())
		);
		free(abs_path);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	const char* buffer = *utf_data_str;
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), fstream);
	if (ferror(fstream)) {
		fclose(fstream);
		fstream = NULL;
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			___get_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path->data())
		);
		free(abs_path);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	free(abs_path);
	fclose (fstream);
	fstream = NULL;
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, static_cast<int>(len))
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "message"),
		v8::String::NewFromUtf8(isolate, "Success...")
	);
	args.GetReturnValue().Set(v8_result);
	return;
};
//#include <fstream>
//using namespace pdf_ext;
//DONE ON 2:25 AM 11/29/2018
void __v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>&out_put) {
	v8::Local<v8::Array> property_names = v8_obj->GetOwnPropertyNames();
	uint32_t length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		v8::Local<v8::Value> key = property_names->Get(i);
		v8::Local<v8::Value> value = v8_obj->Get(key);
		if (value->IsNullOrUndefined())continue;
		if (key->IsString() && value->IsString()) {
			v8::String::Utf8Value utf8_key(isolate, key);
			v8::String::Utf8Value utf8_value(isolate, value);
			auto key_str = new std::string(*utf8_key);
			auto val_str = new std::string(*utf8_value);
			//const char* key = *utf8_key;
			//const char* value = *utf8_value;
			//std::cout << key_str->data() << "\r\n" << val_str->data();
			//throw new std::exception(key);
			out_put[key_str->data()] = val_str->data();
			delete key_str; delete val_str;
		}
	}
};
//create_directory
//6:23 AM 12/5/2018
void native_create_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Server path required!!!")));
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string();
	__get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::create_directory(abs_dir->data());
	if (rec > 0) {
		args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Success"));
	}
	else {
		if (rec == -1) {
			args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Directory already exists!!!"));
		}
		else {
			args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Unknown error please retry!!!"));
		}
	}
	delete abs_dir;
	return;
	
};
void __async_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	
	v8::Isolate *isolate(args.GetIsolate());
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsFunction()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Function required!!!")));
		return;
	}
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	//v8::Isolate::Scope isolate_scope(isolate);
	//v8::Locker locker(isolate);
	isolate->Enter();
	std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		try {
			v8::HandleScope			handleScope(isolate);
			v8::Isolate::Scope		isolate_scope(isolate);
			v8::Local<v8::Context> context = isolate->GetCurrentContext();
			v8::Context::Scope		context_scope(context);
			v8::Local<v8::Object>global = context->Global();
			v8::Handle<v8::Value> arg[1] = {
				v8::Number::New(isolate, 0)
			};
			//v8::TryCatch try_catch(isolate);
			callback->Call(global, 1, arg);
			//if (try_catch.HasCaught()) { return -1; }
			context.Clear(); global.Clear();
			isolate->Exit();
			//v8::Unlocker l(iso);
			try {
				std::stringstream ss;
				ss << std::this_thread::get_id();
				uint64_t id = std::stoull(ss.str());
				return (int)id;
			} catch (...) {
				return 1;
			}
		} catch (...) {
			return -1;
		}
	});
	int rec = result.get();
	cb.Reset(); 
	callback.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
template <typename... ParamTypes>
void __setTimeOut(int milliseconds, std::function<void(ParamTypes...)> func, ParamTypes... parames) {
	std::async(std::launch::async, [=]() {
		Sleep(milliseconds);
		func(parames...);
	});
};
template <typename... ParamTypes, class funcs>
void __set_TimeOut(int milliseconds, funcs func, ParamTypes... parames) {
	std::async(std::launch::async, [=]() {
		Sleep(milliseconds);
		func(parames...);
	});
};
void set_time_out(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate *isolate(args.GetIsolate());
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsFunction()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Function required!!!")));
		return;
	}
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	//v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	//v8::Isolate::Scope isolate_scope(isolate);
	//v8::Locker locker(isolate);
	isolate->Enter();
	std::function<void(int)> func2 = [&](int param) {
		
	};
	__setTimeOut<int>(2000, func2, 10000);
	__set_TimeOut<int>(2000, [](int param) {

	}, 10000);
};
void generate_pdf(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//11:14 PM 12/4/2018
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (is_flush > 0) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
			return;
		}
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_path_str = config->Get(sow_web_jsx::v8_str(isolate, "path"));
		if (v8_path_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Output path required!!!")));
			return;
		}
		v8::Local<v8::Value> v8_url_str = config->Get(sow_web_jsx::v8_str(isolate, "url"));
		bool form_body = false;
		v8::Local<v8::Boolean> b = v8::Local<v8::Boolean>::Cast(config->Get(sow_web_jsx::v8_str(isolate, "from_body")));
		if (b->IsTrue()) {
			form_body = true;
		}
		else {
			if (!args[1]->IsString() || args[0]->IsNullOrUndefined()) {
				if (!v8_url_str->IsString() && v8_url_str->IsNullOrUndefined()) {
					config.Clear();
					isolate->ThrowException(v8::Exception::TypeError(
						v8::String::NewFromUtf8(isolate, "Body string required!!!")));
					return;
				}
			}
		}
		v8::String::Utf8Value utf8_path_str(isolate, v8_path_str);
		auto abs_path = new std::string();
		__get_server_map_path(*utf8_path_str, *abs_path);
		auto dir = new std::string();
		sow_web_jsx::get_dir_from_path(*abs_path, *dir);
		if (!sow_web_jsx::dir_exists(dir->data())) {
			isolate->ThrowException(v8::Exception::Error(___get_msg(isolate, dir->data(), " does not exists!!! Please at first create directory.")));
			delete dir; delete abs_path; config.Clear();
			return;
		}
		delete dir;
		v8::Local<v8::Value> v8_global_settings_str = config->Get(sow_web_jsx::v8_str(isolate, "global_settings"));
		auto wgs_settings = new std::map<const char*, const char*>();
		if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
			__v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
			v8_global_settings_str.Clear();
			v8_global_settings_object.Clear();
		}
		v8::Local<v8::Value> v8_object_settings_str = config->Get(sow_web_jsx::v8_str(isolate, "object_settings"));
		auto wos_settings = new std::map<const char*, const char*>();
		if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
			__v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
			v8_object_settings_str.Clear();
			v8_object_settings_object.Clear();
		}
		config.Clear();
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		malloc(sizeof pdf_gen);
		int rec = 0;
		rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
		delete wgs_settings; delete wos_settings;
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, pdf_gen->get_status_msg())
			);
			args.GetReturnValue().Set(v8_result);
			v8_result.Clear();
			delete pdf_gen;
			return;
		}

		if (!form_body) {
			if (v8_url_str->IsString() && !v8_url_str->IsNullOrUndefined()) {
				v8::String::Utf8Value utf8_url_str(isolate, v8_url_str);
				rec = pdf_gen->generate_from_url(*utf8_url_str, abs_path->c_str());
			}
			else {
				v8::String::Utf8Value utf8_body_str(isolate, args[1]);
				rec = pdf_gen->generate_to_path(*utf8_body_str, abs_path->c_str());
			}

		}
		else {
			auto str = new std::string(body_stream.str());
			malloc(sizeof str);
			rec = pdf_gen->generate_to_path(str->data(), abs_path->c_str());
			delete str;
		}
		delete abs_path;

		if (rec > 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, "Success")
			);
		}
		else {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, pdf_gen->get_status_msg())
			);
		}
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		delete pdf_gen;
		return;
	} catch (std::exception&e) {
		args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, e.what()));
	}
};
void generate_pdf_from_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	try {
		if (___write_http_status(*_http_status, true) < 0) {
			args.GetReturnValue().Set(v8::Number::New(isolate, -1));
			return;
		};
		std::string*str = new std::string(body_stream.str());
		std::stringstream().swap(body_stream);
		malloc(sizeof str);
		std::string str_output;
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		malloc(sizeof pdf_gen);
		
		pdf_gen->init(true);
		int rec = pdf_gen->generate(str->c_str(), str_output);
		delete str;
		if (rec < 0) {
			pdf_gen->dispose();
			body_stream << pdf_gen->get_status_msg();
			args.GetReturnValue().Set(v8::Number::New(isolate, rec));
			return;
		}
		//body_stream << "\r\n";
		body_stream << str_output;
		std::string().swap(str_output);
		//body_stream << "\r\n\r\n";
		___add_header(*_headers, "wkhtmltopdf_version", pdf_gen->version);
		pdf_gen->dispose();
		delete pdf_gen;
		___add_header(*_headers, "Accept-Ranges", "bytes");
		___add_header(*_headers, "Content-Type", "application/pdf");
		//___add_header(*_headers, "Content-Disposition", "attachment;filename=\"auto.pdf\"");
		//___add_header(*_headers, "Content-Type", "application/octet-stream");
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		//delete data; 
	} catch (std::runtime_error&e) {
		std::stringstream().swap(body_stream);
		body_stream << "PDF Generation failed!!!\r\n";
		body_stream << "ERROR==>" << e.what();
		args.GetReturnValue().Set(v8::Number::New(isolate, -1));
	}
	return;
};
void set_cookie(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (args[0]->IsNullOrUndefined()) {
		_cookies->push_back("Set-Cookie: sow=rktuxyn; Expires=Sat, 15 Jan 2050 07:28:00 GMT; Secure; HttpOnly\n");
		args.GetReturnValue().Set(v8::Number::New(isolate, -1));
		return;
	}
	v8::String::Utf8Value resp_cookie_str(isolate, args[0]);
	const char* cook_val = *resp_cookie_str;
	auto itr = std::find(_cookies->begin(), _cookies->end(), cook_val);
	if (itr != _cookies->end()) {
		_cookies->erase(itr);
	}
	_cookies->push_back(cook_val);
	args.GetReturnValue().Set(v8::Number::New(isolate, 1));
};
void http_status(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "HTTP Status Code Required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value status_code_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	desc->append(" ");
	desc->append(*status_code_str);
	desc->append(" ");
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		desc->append("Unspecified Description");
	}
	else {
		v8::String::Utf8Value dec_str(isolate, args[2]);
		desc->append(*dec_str);
	}
	___add_http_status(*_http_status, *desc);
	delete desc;
	return;
};
void response_write_header(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Header Key string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Header Description string required!!!")));
		return;
	}
	v8::String::Utf8Value key_str(isolate, args[0]);
	v8::String::Utf8Value description_str(isolate, args[1]);
	const char* ckey = *key_str;
	/*if (strcmp(ckey, "X-Powered-By") == 0 || strcmp(ckey, "X-Process-By") == 0 || strcmp(ckey, "Content-Encoding") == 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, ___get_msg("Reserve header key not allowed ==>", ckey))));
		return;
	}*/
	const char* desc = *description_str;
	___add_header(*_headers, ckey, desc);
	return;
};
void response_write_x(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (args[0]->IsNullOrUndefined())return;
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	body_stream << *utf8_str;
	return;
};
void http_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (is_flush > 0) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
			return;
		}
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_url_str = config->Get(sow_web_jsx::v8_str(isolate, "url"));
		if (v8_url_str->IsNullOrUndefined() || !v8_url_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Url should be string!!!")));
			return;
		}
		v8::Local<v8::Value> v8_method_str = config->Get(sow_web_jsx::v8_str(isolate, "method"));
		v8::String::Utf8Value utf_method_str(isolate, v8_method_str);
		std::string method(*utf_method_str);
		if (v8_method_str->IsNullOrUndefined() || !v8_method_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Method should be string!!!")));
			return;
		}
		v8::Local<v8::Value>v8_payload_val = config->Get(sow_web_jsx::v8_str(isolate, "body"));
		if (method == "POST") {
			if (v8_payload_val->IsNullOrUndefined() || !v8_payload_val->IsString()) {
				isolate->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(isolate, "Payload required for POST Request!!!")));
				return;
			}
		}
		v8::String::Utf8Value utf_url_str(isolate, v8_url_str);
		http_client::http_request* http = new http_client::http_request(*utf_url_str, true);
		v8_url_str.Clear();
		v8::Local<v8::Value>v8_header_str = config->Get(sow_web_jsx::v8_str(isolate, "header"));
		if (!v8_header_str->IsNullOrUndefined() && v8_header_str->IsArray()) {
			v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_header_str);
			for (uint32_t i = 0; i < harr->Length(); i++) {
				v8::Local<v8::Value>v8_val = harr->Get(i);
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key (isolate, v8_val);
				http->set_header(*key);
			}
			v8_header_str.Clear();
		}
		v8::Local<v8::Value>v8_cookie_str = config->Get(sow_web_jsx::v8_str(isolate, "cookie"));
		if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsString()) {
			v8::String::Utf8Value utf_cook_str (isolate, v8_cookie_str);
			http->set_cookie(*utf_cook_str);
		}
		else if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsArray()) {
			v8::Local<v8::Array> carr = v8::Local<v8::Array>::Cast(v8_cookie_str);
			std::string* cookies = new std::string();
			for (uint32_t i = 0; i < carr->Length(); i++) {
				v8::Local<v8::Value>v8_val = carr->Get(i);
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key (isolate, v8_val);
				if (i == 0) {
					cookies->append(*key); continue;
				}
				cookies->append("; ");
				cookies->append(*key);
			}
			http->set_cookie(cookies->c_str());
			delete cookies;
			v8_cookie_str.Clear();
		}
		std::string* resp_header = new std::string();
		std::string* resp_body = new std::string();
		int rec = 0;
		if (method == "POST") {
			v8::String::Utf8Value v8_payload_str (isolate, v8_payload_val);
			//rec = http->post(*v8_payload_str, *resp_header, *resp_body);
			const char* payload_str = *v8_payload_str;
			std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
				return http->post(payload_str, *resp_header, *resp_body);
			});
			rec = result.get();
			v8_payload_val.Clear();
		}
		else {
			std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
				return http->get(*resp_header, *resp_body);;
			});
			rec = result.get();
			//rec = http->get(*resp_header, *resp_body);
		}
		config.Clear();
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, http->get_last_error())
			);
		}
		else {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, "success")
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "response_header"),
				v8::String::NewFromUtf8(isolate, resp_header->c_str())
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "response_body"),
				v8::String::NewFromUtf8(isolate, resp_body->c_str())
			);
		}
		delete http; delete resp_header; delete resp_body;
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		return;
	} catch (std::runtime_error&e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
		return;
	} catch (...) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Unknown!!!"));
	}
	
};
void encrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args);
void decrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args);
//#include <iosfwd>
//5:08 PM 11/28/2018
/*
HTTP/1.1 301 Moved Permanently
Location: http://www.example.org/index.asp
*/
void __sleep_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Sleep time Required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	int64_t milliseconds = num->ToInteger(isolate)->Value();
	Sleep((DWORD)milliseconds);
};
void response_redirect(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Redirect location required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value url_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	//desc->append(" 301 Moved Permanently\n");
	desc->append(" 303 See Other\n");
	desc->append("Location: ");
	desc->append(*url_str);
	___add_http_status(*_http_status, *desc);
	delete desc;
};
void encrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
	//
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
};
void decrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
	//
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
};
void require(const v8::FunctionCallbackInfo<v8::Value>& args) {
	
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
//
#endif//!__client_build
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	__get_server_map_path(*utf_abs_path_str, *abs_path);
	std::string source_str("");
#if defined(__client_build)
	//
#else
	size_t ret = sow_web_jsx::read_file(abs_path->c_str(), source_str, true);
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::TypeError(___get_msg(isolate, "Invlalid script path defined!!!", abs_path->data())));//!TODO ADD SCRIPT PATH
		delete abs_path;
		return;
	}
#endif//!__client_build
	delete abs_path;
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	v8::Local<v8::ObjectTemplate> module_object = v8::ObjectTemplate::New(isolate);
	module_object->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8_global->Set(isolate, "module", module_object);
	v8_global->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	/*[IO/file read/write]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	io_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write]*/
	/*[crypto]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	//
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, __async_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, __sleep_func));
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
	v8::Context::Scope context_scope(context);
	// Compile the source code.
	v8::Local<v8::Script> script =
		v8::Script::Compile(context, sow_web_jsx::v8_str(isolate, source_str.data())).ToLocalChecked();
	std::string().swap(source_str);
	// Run the script to get the result.
	script->Run(context);
	v8::Local<v8::Object> jsGlobal =
		context->Global()->GetPrototype().As<v8::Object>();
	v8::Local<v8::Object> modules = v8::Handle<v8::Object>::Cast(jsGlobal->Get(sow_web_jsx::v8_str(isolate, "module")));
	args.GetReturnValue().Set(modules->Get(sow_web_jsx::v8_str(isolate, "exports")));
	modules.Clear(); script.Clear();
	jsGlobal.Clear(); context.Clear();
	v8_global.Clear();
	return;
};
/*[zgip]*/
void ___gzip_compress_write() {
	is_flush = 1;
	if (___write_http_status(*_http_status, true) < 0) {
		std::stringstream().swap(body_stream);
		___write_http_status(*_http_status);
		__write_cookies(*_cookies);
		delete _http_status; delete _headers; delete _cookies;
		std::cout << "\r\n";
		fflush(stdout);
		return;
	};
	delete _http_status;
	__write_header(*_headers);
	delete _headers;
	__write_cookies(*_cookies);
	delete _cookies;
	std::ios::sync_with_stdio(false);
	std::cout << "\r\n";
	std::future<void> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		gzip::compress_gzip(body_stream, std::cout);
		std::stringstream().swap(body_stream);
	});
	result.get();
	fflush(stdout);
};
/*[/zgip]*/
//9:32 PM 11/22/2018
void response_body_flush(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	delete _root_dir;
	if ( _is_gzip_encoding(*_headers) == 1 ) {
		___gzip_compress_write();
		return;
	}
	is_flush = 1;
	if ( ___write_http_status(*_http_status, true ) < 0) {
		std::stringstream().swap(body_stream);
		___write_http_status(*_http_status);
		__write_cookies(*_cookies);
		delete _http_status; delete _headers; delete _cookies;
		std::cout << "\r\n";
		fflush(stdout);
		return;
	}
	std::ios::sync_with_stdio(false);
	delete _http_status;
	__write_header(*_headers);
	delete _headers;
	__write_cookies(*_cookies);
	delete _cookies;
	fflush(stdout);
	std::cout << "\r\n";
	SET_BINARY_MODE(stdout);
	std::copy(std::istreambuf_iterator<char>(body_stream),
		std::istreambuf_iterator<char>(),
		std::ostream_iterator<char>(std::cout)
	);
	fflush(stdout);
	std::stringstream().swap(body_stream);
	return;
};
void response_clear(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unable to clear. Body already flushed!!!")));
		return;
	}
	std::stringstream().swap(body_stream);
};
void get_response_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Unable to read response. Body already flushed!!!")));
		return;
	}
	std::string* str = new std::string(body_stream.str());
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, str->c_str()));
	delete str;
};
void encrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "IV required!!!")));
		return;
	}
	v8::String::Utf8Value utf_plain_text(isolate, args[0]);
	v8::String::Utf8Value utf_key(isolate, args[1]);
	v8::String::Utf8Value utf_iv(isolate, args[2]);
	try {
		std::string* encrypted_text = new std::string();
		int rec = crypto::encrypt(*utf_plain_text, *utf_key, *utf_iv, *encrypted_text);
		if (rec < 0) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Unable to encrypt plain text.")));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, encrypted_text->c_str()));
		}
		delete encrypted_text;
	} catch (std::runtime_error&e) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, e.what())));
	}
};
void decrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (is_flush > 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Unable to read response. Body already flushed!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "IV required!!!")));
		return;
	}
	v8::String::Utf8Value utf_encrypted_text(isolate, args[0]);
	v8::String::Utf8Value utf_key(isolate, args[1]);
	v8::String::Utf8Value utf_iv(isolate, args[2]);
	try {
		//void encrypt(const char*plain_text, const char*key, const char *iv, std::string&encrypt_text);
		std::string* decrypted_text = new std::string();
		int rec = crypto::decrypt(*utf_encrypted_text, *utf_key, *utf_iv, *decrypted_text);
		if (rec < 0) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Unable to decrypt encrypted text.")));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, decrypted_text->c_str()));
		}
		delete decrypted_text;
	} catch (std::runtime_error&e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
	}
};
//9:32 PM 11/22/2018
v8::Local<v8::Context> sow_web_jsx::wrapper::get_context(v8::Isolate* isolate, std::map<std::string, std::map<std::string, std::string>>& ctx) {
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		auto key = itr->first;
		auto obj = itr->second;
		if (key == "global") {
			for (auto gitr = obj.begin(); gitr != obj.end(); ++gitr) {
				ctx_object->Set(isolate, (gitr->first).c_str(), v8_str(isolate, gitr->second.c_str()));
			}
			_root_dir = new std::string(obj["root_dir"]);
			continue;
		}
		v8::Local<v8::ObjectTemplate> object = v8::ObjectTemplate::New(isolate);
		for (auto oitr = obj.begin(); oitr != obj.end(); ++oitr) {
			object->Set(isolate, (oitr->first).c_str(), v8_str(isolate, oitr->second.c_str()));
		}
		ctx_object->Set(isolate, key.c_str(), object);
	}
	/*[context.respons....]*/
	v8::Local<v8::ObjectTemplate> response_object = v8::ObjectTemplate::New(isolate);
	response_object->Set(isolate, "_write", v8::FunctionTemplate::New(isolate, response_write_x));
	response_object->Set(isolate, "header", v8::FunctionTemplate::New(isolate, response_write_header));
	response_object->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, native_write_from_file));
	v8::Local<v8::ObjectTemplate> body_object = v8::ObjectTemplate::New(isolate);
	body_object->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, response_body_flush));
	body_object->Set(isolate, "write_as_pdf", v8::FunctionTemplate::New(isolate, generate_pdf_from_body));
	response_object->Set(isolate, "body", body_object);
	response_object->Set(isolate, "clear", v8::FunctionTemplate::New(isolate, response_clear));
	response_object->Set(isolate, "status", v8::FunctionTemplate::New(isolate, http_status));
	response_object->Set(isolate, "cookie", v8::FunctionTemplate::New(isolate, set_cookie));
	response_object->Set(isolate, "_redirect", v8::FunctionTemplate::New(isolate, response_redirect));
	ctx_object->Set(isolate, "response", response_object);
	/*[/context.respons....]*/
	v8_global->Set(isolate, "context", ctx_object);
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	io_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_non_query", v8::FunctionTemplate::New(isolate, npgsql_execute_non_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	/*[/NpgSql....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	v8_global->Set(isolate, "__get_response_body", v8::FunctionTemplate::New(isolate, get_response_body));
	v8_global->Set(isolate, "__create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, __async_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, __sleep_func));
	_headers = new std::map<std::string, std::string>();
	malloc(sizeof _headers);
	_cookies = new std::vector<std::string>();
	malloc(sizeof _cookies);
	_http_status = new std::vector<std::string>();
	malloc(sizeof _http_status);
	return v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
}
v8::Local<v8::Context> sow_web_jsx::wrapper::get_console_context(v8::Isolate * isolate, const char * root_dir, const char * path_translated, const char * arg) {
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8_global->Set(isolate, "__root_dir", v8_str(isolate, root_dir));
	v8_global->Set(isolate, "__path_translated", v8_str(isolate, path_translated));
	v8_global->Set(isolate, "__arg", v8_str(isolate, arg));
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	io_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_non_query", v8::FunctionTemplate::New(isolate, npgsql_execute_non_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	/*[/NpgSql....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	/*[__async]*/
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, __async_func));
	/*[__async]*/
	/*[__sleep]*/
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, __sleep_func));
	/*[__sleep]*/
	_root_dir = new std::string(root_dir);
	v8_global->Set(isolate, "__clear", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		delete _root_dir;
	}));
	v8_global->Set(isolate, "__print", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(iso, "String Required!!!")));
			return;
		}
		v8::String::Utf8Value utf_msg_str(iso, args[0]);
		std::cout << *utf_msg_str << "\r\n";
		return;
	}));
	return v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
}
;
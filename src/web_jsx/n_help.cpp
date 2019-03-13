/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "n_help.h"
using namespace sow_web_jsx;
void n_help::add_http_status(std::vector<std::string>&http_status, std::string&values) {
	if (!http_status.empty()) {
		http_status.clear();
	}
	http_status.push_back(values);
};
int n_help::write_http_status(std::vector<std::string>&http_status, bool check_status = false) {
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
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
void n_help::add_header(std::map<std::string, std::string>&header, const std::string&key, const std::string&values) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase (it);
	}
	header[key] = values;
};
void n_help::remove_header(std::map<std::string, std::string>&header, const std::string&key) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase (it);
	}
};
int n_help::is_gzip_encoding(std::map<std::string, std::string>&header) {
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
void n_help::write_header(std::map<std::string, std::string>&header) {
	if (header.empty())return;
	for (auto it = header.begin(); it != header.end(); ++it) {
		std::cout << it->first << ":" << it->second << "\n";
	}
	return;
};
void n_help::write_cookies(std::vector<std::string>&cookies) {
	if (cookies.empty())return;
	for (size_t i = 0; i < cookies.size(); ++i) {
		std::cout << cookies[i] << "\n";
	}
};
void n_help::v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>&out_put) {
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
			out_put[key_str->data()] = val_str->data();
			delete key_str; delete val_str;
		}
	}
};
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:00 PM 12/24/2019
#include "http_payload.h"
#if !defined(_web_jsx_global_h)
#include "web_jsx_global.h"
#endif//_web_jsx_global_h
#if !defined(DATA_READ_CHUNK)
#define DATA_READ_CHUNK 8192
#endif//!DATA_READ_CHUNK
#pragma warning(disable : 4996)//Disable strcpy warning
using namespace sow_web_jsx;
/*[Help]*/
class http_posted_file {
public:
	http_posted_file(
		const std::string& disposition,
		const std::string& name,
		const std::string& file_name,
		const std::string& c_type
	);
	~http_posted_file();
	void store(const std::string& buff);
	double save_as(const char* path);
	const char* get_data();
	const char* get_content_disposition();
	const char* get_name();
	const char* get_file_name();
	const char* get_content_type();
private:
	//multipart_header* _head;
	std::ostringstream _stream;
	std::string _fcontent_disposition;
	std::string _fname;
	std::string _ffile_name;
	std::string _fcontent_type;
};
typedef struct POSTED_FILES {
	POSTED_FILES* next;
	http_posted_file* file;
}posted_files;
class http_payload {
public:
	http_payload(const std::string& content_type, size_t content_length);
	~http_payload();
	int read_all();
	bool is_maltipart();
	int has_error();
	int get_total_file();
	const char* get_body();
	const char* get_last_error();
	template<class _func>
	void read_files(_func cb) {
		if (_posted_files == NULL)return;
		posted_files* pf;
		for (pf = _posted_files; pf; pf = pf->next) {
			cb(pf->file);
		}
		return;
	}
private:
	bool _is_disposed;
	int _is_multipart;
	int _is_read_end;
	int _file_count;
	size_t _content_length;
	posted_files* _posted_files;
	int _errc;
	char* _internal_error;
	std::string _content_type;
	//std::string _form_data;
	std::ostringstream _stream;
	int panic(const char* error, int code);
	void parse_mime(const std::string& data);
};
bool strings_equal(
	const std::string& s1,
	const std::string& s2,
	size_t n
){
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	bool good = (n <= s1.length() && n <= s2.length());
	std::string::const_iterator l1 = good ? (s1.begin() + n) : s1.end();
	std::string::const_iterator l2 = good ? (s2.begin() + n) : s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return good;
}
bool strings_equal(
	const std::string& s1,
	const std::string& s2
){
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	std::string::const_iterator l1 = s1.end();
	std::string::const_iterator l2 = s2.end();
	while (p1 != l1 && p2 != l2) {
		if (std::toupper(*(p1++)) != std::toupper(*(p2++)))
			return false;
	}
	return (s2.size() == s1.size()) ? true : false;
}
std::string extract_between(const std::string& data,
	const std::string& separator1,
	const std::string& separator2
){
	std::string result;
	std::string::size_type start, limit;

	start = data.find(separator1, 0);
	if (std::string::npos != start) {
		start += separator1.length();
		limit = data.find(separator2, start);
		if (std::string::npos != limit)
			result = data.substr(start, limit - start);
	}

	return result;
}
char hex_to_char(char first, char second){
	int digit;
	digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0'));
	digit *= 16;
	digit += (second >= 'A' ? ((second & 0xDF) - 'A') + 10 : (second - '0'));
	return static_cast<char>(digit);
}
std::string form_urldecode(const std::string& src){
	std::string result;
	std::string::const_iterator iter;
	char c;
	for (iter = src.begin(); iter != src.end(); ++iter) {
		switch (*iter) {
		case '+':
			result.append(1, ' ');
			break;
		case '%':
			// Don't assume well-formed input
			if (std::distance(iter, src.end()) >= 2
				&& std::isxdigit(*(iter + 1)) && std::isxdigit(*(iter + 2))) {
				c = *++iter;
				result.append(1, hex_to_char(c, *++iter));
			}
			// Just pass the % through untouched
			else {
				result.append(1, '%');
			}
			break;

		default:
			result.append(1, *iter);
			break;
		}
	}

	return result;
}
http_posted_file* parse_header(const std::string& data){
	std::string end = "\r\n";
	std::string part = data.substr(0, data.find_first_of(end, 0));
	std::string disposition;
	disposition = extract_between(part, "Content-Disposition: ", ";");
	std::string name;
	//name = extract_between(data, "name=\"", "\"");
	name = extract_between(part, "name=\"", ";");
	std::string filename;
	filename = extract_between(part, "filename=\"", "\"");
	part = data.substr(part.size() + end.size());
	std::string cType;
	cType = extract_between(part, "Content-Type: ", "\r\n\r\n");
	// This is hairy: Netscape and IE don't encode the filenames
	// The RFC says they should be encoded, so I will assume they are.
	filename = form_urldecode(filename);
	return new http_posted_file(disposition, name, filename, cType);
}
http_posted_file::http_posted_file(const std::string& disposition,
	const std::string& name,
	const std::string& file_name,
	const std::string& c_type
) {
	//this->_data = NULL;
	_fcontent_disposition = disposition.c_str();
	_fname = name.c_str();
	_ffile_name = file_name.c_str();
	_fcontent_type = c_type.c_str();
	//std::cout << "fcontent_disposition:" << _fcontent_disposition << std::endl;
	//std::cout << "fname:" << _fname << std::endl;
	//std::cout << "ffile_name:" << _ffile_name << std::endl;
	//std::cout << "fcontent_type:" << _fcontent_type << std::endl;
}
http_posted_file::~http_posted_file() {
	if (_stream.tellp() != std::streampos(0)) {
		_stream.clear();
		std::ostringstream().swap(_stream);
	}
	_fcontent_disposition.clear();
	_fname.clear(); _ffile_name.clear();
	_fcontent_type.clear();
}
const char* http_posted_file::get_content_disposition() {
	return this->_fcontent_disposition.c_str();
}
const char* http_posted_file::get_name() {
	return this->_fname.c_str();
}
const char* http_posted_file::get_file_name() {
	return this->_ffile_name.c_str();
}
const char* http_posted_file::get_content_type() {
	return this->_fcontent_type.c_str();
}
void http_posted_file::store(const std::string& buff) {
	int len = (int)buff.size();
	if (_fcontent_type.empty() || (_fcontent_type.find("text") != std::string::npos)) {
		_stream.write(buff.c_str(), len);
	}
	else {
		//_stream.write(reinterpret_cast<const char*>(&len), sizeof(int));
		_stream.write(buff.c_str(), len * sizeof(char));
	}
}
const char* http_posted_file::get_data() {
	if (_stream.tellp() == std::streampos(0)) {
		return NULL;
	}
	if (_fcontent_type.empty() || (_fcontent_type.find("text") != std::string::npos)) {
		return _stream.str().c_str();
	}
	return NULL;
}
double http_posted_file::save_as(const char* path) {
	if (_stream.tellp() == std::streampos(0)) {
		return -4;
	}
	FILE* fstream;
	errno_t err;
	err = fopen_s(&fstream, path, "w+");
	if (err != 0 || fstream == NULL) {
		return -1;
	}
	//_stream.rdbuf();
	//auto buf = _stream.rdbuf();
	size_t write_len = fwrite(_stream.str().c_str(), 1, _stream.str().size(), fstream);
	if (ferror(fstream)) {
		fclose(fstream);
		return -2;
	}
	fclose(fstream);
	return static_cast<double>(write_len);
}
/*[/Help]*/
http_payload::http_payload(
	const std::string&content_type, 
	size_t content_length
){
	_posted_files = NULL;
	_is_read_end = 0;
	_file_count = 0;
	_content_length = content_length;
	_is_multipart = 0;
	_errc = 0; _is_disposed = false;
	_internal_error = new char;
	if (_content_length > 0) {
		std::string multipart_type = "multipart/form-data";
		if (strings_equal(multipart_type, content_type,
			multipart_type.length())) {
			_is_multipart = 1;
		}
		multipart_type.clear();
	}
	else {
		panic("No payload found in current request", -1);
	}
	_content_type = content_type;
}
http_payload::~http_payload(){
	if (_is_disposed)return;
	_is_disposed = true;
	if (_posted_files != NULL) {
		posted_files* pf;
		for (pf = _posted_files; pf; pf = pf->next) {
			delete pf->file;
		}
		_posted_files = NULL;
	}
	_is_multipart = 0; _is_read_end = 0; _file_count = 0;
	_content_length = NULL;
	if (_internal_error != NULL) {
		delete[]_internal_error;
	}
	_errc = 0;
	if (!_content_type.empty()) {
		_content_type.clear();
		std::string().swap(_content_type);
	}
	//std::string _form_data;
	if (_stream.tellp() != std::streampos(0)) {
		_stream.clear();
		std::ostringstream().swap(_stream);
	}
}
#if !defined(_FCGI_STDIO)
#include <fcgi_stdio.h>
#endif//!_FCGI_STDIO
#if !defined(FCGIO_H)
#include "fcgio.h"
#endif//!FCGIO_H
std::ostringstream read_payload(size_t content_length) {
	std::ostringstream std_input;
	size_t len = 0;
	//DATA_READ_CHUNK should be 8192 according to FCGX_Accept_r(FCGX_Request *reqDataPtr) line 2154 file fcgiapp.c
	while (true) {
		char* buff;
		if (content_length > DATA_READ_CHUNK) {
			buff = new char[DATA_READ_CHUNK + 1];
			len = DATA_READ_CHUNK;
		}
		else {
			buff = new char[content_length + 1];
			len = content_length;
		}
		buff[len] = '\0';
#if defined(FAST_CGI_APP)
		std::cin.read(buff, len);
#else
		fread(buff, 1, len, stdin);
#endif//!FAST_CGI_APP
		//strdup(buff);
		if (buff != NULL && buff[0] != '\0') {
			//std_input.write(const_cast<const char*>(buff), len);
			std_input.write(buff, len);
			delete[]buff;
			content_length -= len;
			if (content_length <= 0)break;
		}
		else {
			delete[]buff;
		}
	}
	//in_stream.clear();
	fclose(stdin);
	return std_input;
}
int http_payload::read_all(){
	if (_errc < 0 || _is_disposed == true)return 0;
	if (_is_read_end > 0)return 0;
	if (_content_length <= 0) {
		return panic("No payload found in current request", -1);
	}
	try {
		if (SET_BINARY_MODE_IN() == -1) {
			std::string err("ERROR: while converting cin to binary:");
			err.append(strerror(errno));
			return panic(err.c_str(), -10);
		}
		_stream = read_payload(_content_length);
		if (_stream.tellp() == std::streampos(0)) {
			return panic("Unable to read payload from current request", -1);
		}
		_is_read_end = 1;
		if ( this->is_maltipart() ) {
			std::string 		bType = "boundary=";
			std::string::size_type 	pos = _content_type.find(bType);
			// generate the separators
			std::string sep1 = _content_type.substr(pos + bType.length());
			sep1.append("\r\n");
			sep1.insert(0, "--");
			std::string form_data = _stream.str();
			// Find the data between the separators
			std::string::size_type start = form_data.find(sep1);
			std::string::size_type sepLen = sep1.length();
			std::string::size_type oldPos = start + sepLen;
			while (true) {
				pos = form_data.find(sep1, oldPos);
				// If sep1 wasn't found, the rest of the data is an item
				if (std::string::npos == pos)
					break;
				// parse the data
				parse_mime(form_data.substr(oldPos, pos - oldPos));
				// update position
				oldPos = pos + sepLen;
			}
		}
		return 1;
	}
	catch (std::exception& e) {
		return panic(e.what(), -501);
	}
	catch (...) {
		return panic("Unknown error...", -501);
	}
	
	
}
bool http_payload::is_maltipart(){
	return _is_multipart != 0;
}
int http_payload::has_error(){
	return _errc < 0;
}
const char* http_payload::get_last_error(){
	if (_errc >= 0 || _internal_error == NULL) return "No Error Found!!!";
	return const_cast<const char*>(_internal_error);
}
int http_payload::panic(const char* error, int code){
	if (_internal_error != NULL)
		delete[]_internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_errc = code;
	return _errc;
}
void http_payload::parse_mime(const std::string& data){
	// Find the header
	std::string end = "\r\n\r\n";
	std::string::size_type headLimit = data.find(end, 0);
	// Detect error
	if (std::string::npos == headLimit)
		throw std::runtime_error("Malformed input");
	// Extract the value - there is still a trailing CR/LF to be subtracted off
	std::string::size_type valueStart = headLimit + end.length();
	std::string value = data.substr(valueStart, data.length() - valueStart - 2);
	//multipart_header head = parse_header(data.substr(0, valueStart));
	_file_count++;
	posted_files* pf = new posted_files();
	pf->file = parse_header(data.substr(0, valueStart));//new http_posted_file(parse_header(data.substr(0, valueStart)));
	//value = data.substr(0, valueStart);
	pf->file->store(value);
	pf->next = _posted_files;
	_posted_files = pf;
	//value.clear(); std::string().swap(value);
}
int http_payload::get_total_file() {
	return _file_count;
}
const char* http_payload::get_body() {
	if (_is_disposed)return '\0';
	if (_stream.tellp() == std::streampos(0))return '\0';
	return _stream.str().c_str();
}
v8::Local<v8::Object> get_file_obj(v8::Isolate* isolate, http_posted_file* pf) {
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate);
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "save_as", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Absolute path required!!!")));
			return;
		}
		native_string absolute_v8_str(isolate, args[0]);
		std::string std_str(absolute_v8_str.c_str());
		sow_web_jsx::format__path(std_str);
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		double ret = app->save_as(std_str.c_str());
		if (ret > 0) {
			absolute_v8_str.clear();
			args.GetReturnValue().Set(v8::Number::New(isolate, ret));
			return;
		}
		if (ret == -4) {
			std_str = "No data found in current file object..";
		}
		else if (ret == -1) {
			std_str = "Unable to open/create new file in " + std_str;
		}
		else if (ret == -2) {
			std_str = "Unable to write data to file...";
		}
		absolute_v8_str.clear();
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, std_str.c_str())));
		return;
	}, isolate->GetCurrentContext()->Global()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_data", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_data();
		if (data == NULL || (data != NULL && data == '\0')) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, "No data found in current file object.")));
			return;
		}
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, isolate->GetCurrentContext()->Global()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_disposition", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_content_disposition();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, isolate->GetCurrentContext()->Global()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_name();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, isolate->GetCurrentContext()->Global()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_file_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_file_name();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, isolate->GetCurrentContext()->Global()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_type", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_content_type();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, isolate->GetCurrentContext()->Global()));

	v8::Handle<v8::Object> exports = appTemplate->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	/*exports->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "content_disposition", v8::NewStringType::kNormal).ToLocalChecked(), v8::String::NewFromUtf8(isolate, pf->get_content_disposition())).ToChecked();
	exports->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), v8::String::NewFromUtf8(isolate, pf->get_name())).ToChecked();
	exports->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "file_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::String::NewFromUtf8(isolate, pf->get_file_name())).ToChecked();
	exports->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "content_type", v8::NewStringType::kNormal).ToLocalChecked(), v8::String::NewFromUtf8(isolate, pf->get_content_type())).ToChecked();*/
	exports->SetAlignedPointerInInternalField(0, pf);
	v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, exports);
	pobj.SetWeak<http_posted_file*>(&pf, [](const v8::WeakCallbackInfo<http_posted_file*>& data) {
		delete[] data.GetParameter();
	}, v8::WeakCallbackType::kParameter);
	return exports;
}
void sow_web_jsx::read_http_posted_file(const v8::FunctionCallbackInfo<v8::Value>& args){
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "Contenent length required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			sow_web_jsx::v8_str(isolate, "ContentType required!!!")));
		return;
	}
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate);
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_all", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int ret = app->read_all();
		v8::Isolate* isolate = args.GetIsolate();
		if (ret < 0) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, app->get_last_error())));
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, args.Holder()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "is_maltipart", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		bool ret = app->is_maltipart();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret));
	}, args.Holder()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_body", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_body();
		if (data == NULL || (data != NULL && data == '\0')) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, "No data found in current context.")));
			return;
		}
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, args.Holder()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "has_error", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int ret = app->has_error();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret < 0 ? true : false));
	}, args.Holder()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "total_file", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int ret = app->get_total_file();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Number::New(isolate, ret ));
	}, args.Holder()));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_files", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		//v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
		v8::Persistent<v8::Function> cb;
		cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
		//v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, args[0]);
		app->read_files([&isolate, &callback](http_posted_file* pf) {
			v8::Local<v8::Object> file_obj = get_file_obj(isolate, pf);
			v8::Local<v8::Value> argv[] = { file_obj };
			callback->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 1, argv);
			argv->Clear();
		});
		callback.Clear();
	}, args.Holder()));
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> localApp = appTemplate->GetFunction(ctx).ToLocalChecked()->NewInstance(ctx).ToLocalChecked();
	{
		v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
		size_t content_length = static_cast<size_t>(num->Value());
		native_string ctype(isolate, args[1]);
		http_payload* hp = new http_payload(ctype.c_str(), content_length);
		localApp->SetAlignedPointerInInternalField(0, hp);
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, localApp);
		pobj.SetWeak<http_payload*>(&hp, [](const v8::WeakCallbackInfo<http_payload*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	}
	args.GetReturnValue().Set(localApp);
}

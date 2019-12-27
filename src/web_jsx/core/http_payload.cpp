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
	bool is_empty_header();
	bool is_text_content();
	void clear();
	const char* get_data();
	const char* get_content_disposition();
	const char* get_name();
	const char* get_file_name();
	const char* get_content_type();
	size_t get_file_size();
private:
	bool _is_disposed;
	std::unique_ptr<std::vector<char>> _stream;
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
	int save_to_file(const char* dir);
	bool is_maltipart();
	int has_error();
	int get_total_file();
	const char* get_body();
	const char* get_last_error();
	void clear();
	template<class _func>
	int read_files(_func cb) {
		int rec = 0;
		if (_posted_files == NULL)return rec;
		posted_files* pf;
		for (pf = _posted_files; pf; pf = pf->next) {
			cb(pf->file); rec++;
		}
		return rec;
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
	std::unique_ptr<std::vector<char>> _stream;
	int panic(const char* error, int code);
	void parse_mime(const std::string& data);
};
bool strings_equal(
	const std::string& s1,
	const std::string& s2,
	size_t n
) {
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
) {
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
) {
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
char hex_to_char(char first, char second) {
	int digit;
	digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0'));
	digit *= 16;
	digit += (second >= 'A' ? ((second & 0xDF) - 'A') + 10 : (second - '0'));
	return static_cast<char>(digit);
}
std::string form_urldecode(const std::string& src) {
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
http_posted_file* parse_header(const std::string& data) {
	std::string end = "\r\n";
	std::string part = data.substr(0, data.find_first_of(end, 0));
	std::string disposition = extract_between(part, "Content-Disposition: ", ";");
	std::string name = extract_between(part, "name=\"", ";");
	std::string filename = extract_between(part, "filename=\"", "\"");
	part = data.substr(part.size() + end.size());
	std::string cType = extract_between(part, "Content-Type: ", "\r\n\r\n");
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
	_is_disposed = false;
	_stream = std::make_unique<std::vector<char>>();
}
http_posted_file::~http_posted_file() {
	this->clear();
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
void http_posted_file::clear() {
	if (_is_disposed)return;
	_is_disposed = true;
	if (_stream != NULL) {
		_stream->clear();
		_stream.release();
		_stream = NULL;
	}
	_fcontent_disposition.clear();
	_fname.clear(); _ffile_name.clear();
	_fcontent_type.clear();
}
size_t http_posted_file::get_file_size() {
	return _stream->size();
}
void http_posted_file::store(const std::string& buff) {
	size_t len = buff.size();
	_stream->reserve(len);
	const char* buffer = buff.c_str();
	std::copy(buffer, buffer + len, std::back_inserter(*_stream.get()));
	//std::transform(std::begin(buff), std::end(buff), &_stream[0], [](const auto& s) { return s.c_str(); });
}
const char* http_posted_file::get_data() {
	if (_stream->empty()) {
		return NULL;
	}
	if (is_text_content()) {
		return std::string(_stream->begin(), _stream->end()).c_str();
	}
	return NULL;
}
double http_posted_file::save_as(const char* path) {
	if (_stream->empty()) {
		return -4;
	}
	std::ofstream binary_file(path, std::ios::binary | std::ios::out);
	if (binary_file.is_open()) {
		size_t totalSize = get_file_size();
		std::copy(_stream->begin(), _stream->end(), std::ostreambuf_iterator<char>(binary_file));
		binary_file.close();
		std::ofstream().swap(binary_file);
		return static_cast<double>(totalSize);
	}
	std::ofstream().swap(binary_file);
	return static_cast<double>(-1);
}
bool http_posted_file::is_empty_header() {
	return _fcontent_type.empty();
}
bool http_posted_file::is_text_content() {
	if (_fcontent_type.empty() || (_fcontent_type.find("text") != std::string::npos))return true;
	return false;
}
/*[/Help]*/
http_payload::http_payload(
	const std::string& content_type,
	size_t content_length
) {
	_posted_files = NULL;
	_is_read_end = 0;
	_file_count = 0;
	_content_length = content_length;
	_is_multipart = 0;
	_errc = 0; _is_disposed = false;
	_internal_error = new char;
	_stream = std::make_unique<std::vector<char>>();
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
void http_payload::clear() {
	if (_is_disposed)return;
	_is_disposed = true;
	if (_posted_files != NULL) {
		posted_files* pf;
		while (_posted_files) {
			pf = _posted_files;
			if (_posted_files->next != NULL) {
				_posted_files = std::move(_posted_files->next);
			}
			else {
				_posted_files = NULL;
			}
			pf->file->clear();
			delete pf->file;
			delete pf;
		}
		if (_posted_files != NULL)
			delete _posted_files;
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
	if (_stream != NULL) {
		_stream->clear();
		_stream.release();
		_stream = NULL;
	}
}
http_payload::~http_payload() {
	this->clear();
}
#if defined(FAST_CGI_APP)
#if !defined(_FCGI_STDIO)
#include <fcgi_stdio.h>
#endif//!_FCGI_STDIO
#if !defined(FCGIO_H)
#include "fcgio.h"
#endif//!FCGIO_H
void read_payload(std::unique_ptr<std::vector<char>>& std_input, size_t content_length) {
	std_input->reserve(content_length);
	char buff;
	while (std::cin.get(buff)) {
		// Process c here.
		std_input->insert(std_input->end(), buff);
	}
	fclose(stdin);
	std::cin.clear();
	//fflush(stdin);
}
#else
#if !defined(DATA_READ_CHUNK)
#define DATA_READ_CHUNK 8192
#endif//!DATA_READ_CHUNK
void read_payload(std::vector<char> & std_input, size_t content_length) {
	std_input.reserve(content_length);
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
		fread(buff, 1, len, stdin);
		if (buff != NULL && buff[0] != '\0') {
			std_input.insert(std_input.end(), buff, buff + len);
			delete[]buff;
			content_length -= len;
			if (content_length <= 0)break;
		}
		else {
			delete[]buff;
		}
	}
	fclose(stdin);
}
#endif//!FAST_CGI_APP
int http_payload::save_to_file(const char* dir) {
	int rec = this->read_all();
	if (rec < 0)return rec;
	std::string dir_str(dir);
	sow_web_jsx::format__path(dir_str);
	rec = this->read_files([&dir_str](http_posted_file* file) {
		std::string path(dir_str.c_str());
		path.append(file->get_file_name());
		file->save_as(path.c_str());
		file->clear(); path.clear();
		std::string().swap(path);
	});
	std::string().swap(dir_str);
	return rec;
}
int http_payload::read_all() {
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
		read_payload(_stream, _content_length);
		if (_stream->empty()) {
			return panic("Unable to read payload from current request", -1);
		}
		_is_read_end = 1;
		if (this->is_maltipart()) {
			
			std::string 		bType = "boundary=";
			size_t 	pos = _content_type.find(bType);
			// generate the separators
			std::string sep1 = _content_type.substr(pos + bType.size());
			sep1.append("\r\n");
			sep1.insert(0, "--");
			std::unique_ptr<std::string> form_data = std::make_unique<std::string>(_stream->begin(), _stream->end());
			// Find the data between the separators
			size_t start = form_data->find(sep1);
			size_t sepLen = sep1.size();
			size_t oldPos = start + sepLen;
			while (true) {
				pos = form_data->find(sep1, oldPos);
				// If sep1 wasn't found, the rest of the data is an item
				if (std::string::npos == pos)
					break;
				// parse the data
				parse_mime(form_data->substr(oldPos, pos - oldPos));
				// update position
				oldPos = pos + sepLen;
			}
			sep1.clear(); bType.clear();
			form_data->clear(); form_data.release();
			form_data = NULL;
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
void http_payload::parse_mime(const std::string& data) {
	// Find the header
	std::string end = "\r\n\r\n";
	size_t headLimit = data.find(end, 0);
	// Detect error
	if (std::string::npos == headLimit)
		throw std::runtime_error("Malformed input");
	// Extract the value - there is still a trailing CR/LF to be subtracted off
	size_t valueStart = headLimit + end.length();
	_file_count++;
	posted_files* pf = new posted_files();
	pf->file = parse_header(data.substr(0, valueStart));
	if (pf->file->is_empty_header()) {
		pf->file->clear();
		delete pf->file;
		delete pf;
	}
	else {
		std::unique_ptr<std::string> value = std::make_unique<std::string>(data.substr(valueStart, data.length() - valueStart - 2));
		pf->file->store(*value); value->clear(); value.release();
		value = NULL;
		if (_posted_files != NULL) {
			pf->next = std::move(_posted_files);
		}
		else {
			pf->next = NULL;
		}
		_posted_files = pf;
	}
}
bool http_payload::is_maltipart() {
	return _is_multipart != 0;
}
int http_payload::has_error() {
	return _errc < 0;
}
const char* http_payload::get_last_error() {
	if (_errc >= 0 || _internal_error == NULL) return "No Error Found!!!";
	return const_cast<const char*>(_internal_error);
}
int http_payload::panic(const char* error, int code) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);
	_errc = code;
	return _errc;
}

int http_payload::get_total_file() {
	return _file_count;
}
const char* http_payload::get_body() {
	if (_is_disposed)return '\0';
	if (_stream->empty())return '\0';
	return std::string(_stream->begin(), _stream->end()).c_str();
}
v8::Local<v8::Object> get_file_obj(v8::Isolate* isolate, http_posted_file* pf) {
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate);
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::Object>global = isolate->GetCurrentContext()->Global();
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
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_data", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		if (app->is_text_content()) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, "Readable chracter not avilable for this content type.")));
			return;
		}
		const char* data = app->get_data();
		if (data == NULL || (data != NULL && data == '\0')) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::v8_str(isolate, "No data found in current file object.")));
			return;
		}
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_disposition", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_content_disposition();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_file_size", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		size_t len = app->get_file_size();
		args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(len)));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_name();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_file_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_file_name();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_type", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = (http_posted_file*)args.Holder()->GetAlignedPointerFromInternalField(0);
		const char* data = app->get_content_type();
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, data));
		return;
	}, global));

	v8::Handle<v8::Object> exports = appTemplate->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	exports->SetAlignedPointerInInternalField(0, pf);
	return exports;
}
void sow_web_jsx::read_http_posted_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
	v8::Local<v8::Object>global = args.Holder();
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "save_to_file", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Directory required!!!")));
			return;
		}
		native_string save_dir(isolate, args[0]);
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int len = app->save_to_file(save_dir.c_str()); save_dir.clear();
		args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(len)));
		return;
	}, global));
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
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "is_maltipart", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		bool ret = app->is_maltipart();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret));
	}, global));
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
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "has_error", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int ret = app->has_error();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret < 0 ? true : false));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "total_file", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		int ret = app->get_total_file();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_files", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::v8_str(isolate, "Callback should be a Function instance!!!")));
			return;
		}
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		//v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
		v8::Persistent<v8::Function> cb;
		cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
		int ret = app->read_files([&isolate, &callback](http_posted_file* pf) {
			v8::Local<v8::Object> file_obj = get_file_obj(isolate, pf);
			v8::Local<v8::Value> argv[] = { file_obj };
			callback->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 1, argv);
			argv->Clear();
			/* Marks this JS object invalid */
			file_obj->SetAlignedPointerInInternalField(0, nullptr);
		});
		callback.Clear();
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "release", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = (http_payload*)args.Holder()->GetAlignedPointerFromInternalField(0);
		if (app == NULL ) {
			v8::Isolate* isolate = args.GetIsolate();
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::v8_str(isolate, "`http_payload` Object already released!!!")));
			return;
		}
		app->clear();
		delete app;
		/* Marks this JS object invalid */
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
		args.GetReturnValue().Set(args.Holder());
	}, global));
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Object> localApp = appTemplate->GetFunction(ctx).ToLocalChecked()->NewInstance(ctx).ToLocalChecked();
	{
		v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
		size_t content_length = static_cast<size_t>(num->Value());
		native_string ctype(isolate, args[1]);
		http_payload* hp = new http_payload(ctype.c_str(), content_length);
		localApp->SetAlignedPointerInInternalField(0, hp);
	}
	args.GetReturnValue().Set(localApp);
}

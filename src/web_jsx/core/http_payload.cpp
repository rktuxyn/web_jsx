/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:00 PM 12/24/2019
#	include "http_payload.h"
#	include "web_jsx_global.h"
#	include <stdlib.h>/* srand, rand */
#	include <cstdlib>
#	include <ctime>
#pragma warning(disable : 4996)//Disable strcpy warning
using namespace sow_web_jsx;
/*[Help]*/
class http_posted_file {
public:
	explicit http_posted_file(
		const char* disposition,
		const char* name,
		const char* file_name,
		const char* c_type
	);
	~http_posted_file();
	void store(const char* path);
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
	void set_file_size(size_t file_size);
private:
	bool _is_disposed;
	std::string _fcontent_disposition;
	std::string _fname;
	std::string _ffile_name;
	std::string _fcontent_type;
	bool _is_moved;
	size_t _file_size;
	std::string _temp_file;
};
typedef struct POSTED_FILES {
	POSTED_FILES* next;
	http_posted_file* file;
}posted_files;
class http_payload {
public:
	explicit http_payload(const std::string& content_type, size_t content_length);
	~http_payload();
	int read_all();
	int read_all(const char* temp_dir);
	int save_to_file(const char* dir);
	bool is_multipart();
	bool is_read_end();
	int has_error();
	int get_total_file();
	int get_body(std::string&body);
	const char* get_last_error();
	void clear();
	template<class _func>
	int read_files(_func cb);
	template<class _func>
	int read_line(_func cb);
private:
	bool _is_disposed;
	int _is_multipart;
	int _is_read_end;
	int _file_count;
	bool _ismemory;
	size_t _content_length;
	posted_files* _posted_files;
	int _errc;
	char* _internal_error;
	std::string _content_type;
	std::stringstream _stream;
	size_t get_stream_size();
	int panic(const char* error, int code);
};

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
	return new http_posted_file(disposition.c_str(), name.c_str(), filename.c_str(), cType.c_str());
}
http_posted_file::http_posted_file(
	const char* disposition,
	const char* name,
	const char* file_name,
	const char* c_type
) :
	_fcontent_disposition(disposition),
	_fname(name), _ffile_name(file_name),
	_fcontent_type(c_type) {
	_is_disposed = false;
	_is_moved = false; _file_size = 0;
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
	if (_is_moved == false) {
		if (!_temp_file.empty()) {
			if (__file_exists(_temp_file.c_str())) {
				std::remove(_temp_file.c_str());
			}
		}
	}
	_fcontent_disposition.clear();
	_fname.clear(); _ffile_name.clear();
	_fcontent_type.clear(); _temp_file.clear();
	_file_size = 0;
}
void http_posted_file::set_file_size(size_t file_size) {
	_file_size = file_size;
}
size_t http_posted_file::get_file_size() {
	return _file_size;
}
const char* http_posted_file::get_data() {
	if (_is_moved)return NULL;
	if (is_text_content()) {
		std::ifstream file(_temp_file.c_str(), std::ifstream::ate | std::ifstream::binary);
		if (file.is_open()) {
			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<char> buffer(size);
			const char* file_str = NULL;
			if (file.read(buffer.data(), size)) {
				file_str = std::string(buffer.begin(), buffer.end()).c_str();
			}
			buffer.clear();
			file.close();
			return file_str;
		}
	}
	return NULL;
}
void http_posted_file::store(const char* path) {
	this->_temp_file = std::string(path);
}
double http_posted_file::save_as(const char* path) {
	if (_is_moved == true)return 0;
	if (__file_exists(_temp_file.c_str()) == false)return -1;
	if (__file_exists(path) == true)std::remove(path);
	if (std::rename(_temp_file.c_str(), path) < 0)return -501;
	_is_moved = true;
	return (double)_file_size;
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
	_internal_error = NULL;
	_ismemory = false;
	if (_content_length > 0) {
		std::string multipart_type = "multipart/form-data";
		if (strings_equal(multipart_type, content_type,
			multipart_type.length())) {
			_is_multipart = 1;
		}
		else {
			_ismemory = true;
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
		while (_posted_files) {
			posted_files* pf = _posted_files;
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
	if (_ismemory) {
		size_t size = get_stream_size();
		if (size == 0 || size == std::string::npos) {
			_stream.clear();
			std::stringstream().swap(_stream);
		}
	}
	
}
http_payload::~http_payload() {
	this->clear();
}
template<class _func>
int http_payload::read_files(_func cb) {
	int rec = 0;
	if (_posted_files == NULL)return rec;
	posted_files* pf;
	for (pf = _posted_files; pf; pf = pf->next) {
		cb(pf->file); rec++;
	}
	return rec;
}
std::string random_string(int len) {
	/* initialize random seed: */
	srand((unsigned int)time(NULL));
	std::string format = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	size_t flen = format.size();
	std::string out_str;
	while (out_str.size() != len) {
		int pos = ((rand() % (flen - 1)));
		out_str += format.substr(pos, 1);
	}
	format.clear();
	srand(1);
	return out_str;
}
#if defined(FAST_CGI_APP)
#	include <fcgi_stdio.h>
#	include "fcgio.h"
#if !defined(DATA_READ_CHUNK)
#	define DATA_READ_CHUNK 8192
#endif//!DATA_READ_CHUNK
template<class _func>
int http_payload::read_line(_func cb) {
	if (_content_length == 0) {
		return panic("No payload found in current request", -1);
	}
	if (this->is_multipart() == true) {
		return panic("Multipart posted file not allowed...", -1);
	}
	if (_is_read_end > 0) {
		return panic("Posted data already read end... Please use get_body instead.", -1);
	}
	if (SET_BINARY_MODE_IN() == -1) {
		std::string err("ERROR: while converting cin to binary:");
		err.append(strerror(errno));
		return panic(err.c_str(), -10);
	}
	_is_read_end = 1;
	int rec = 0;
	std::string line;
	std::istream::sentry sentry(std::cin, true);
	while (sow_web_jsx::get_line(std::cin, line)) {
		cb(line.c_str(), line.size()); rec++;
		if (std::cin.eof())break;
	}
	if (!line.empty())line.clear();
	fclose(stdin);
	return rec;
}
void read_payload(std::stringstream&stream, const size_t content_length) {
	size_t c_len = content_length;
	//DATA_READ_CHUNK should be 8192 according to FCGX_Accept_r(FCGX_Request *reqDataPtr) line 2215 file fcgiapp.c
	stream.clear();
	do {
		size_t len = 0;
		char* buff;
		if (c_len > DATA_READ_CHUNK) {
			buff = new char[DATA_READ_CHUNK + 1];
			len = DATA_READ_CHUNK;
		}
		else {
			buff = new char[c_len + 1];
			len = c_len;
		}
		buff[len] = '\0';
		std::cin.read(buff, len);
		stream.write(buff, len);
		delete[]buff;
		c_len -= len;
	} while (!std::cin.eof());
	fclose(stdin);
}
#endif//!FAST_CGI_APP

int http_payload::read_all(const char* temp_dir) {
	if (_is_disposed)return 0;
	if (_errc < 0 )return _errc;
	if (_is_read_end > 0)return 0;
	if (_content_length == 0) {
		return panic("No payload found in current request", -1);
	}
	if (this->is_multipart() == false) {
		return panic("Multipart posted file required...", -1);
	}
	if (SET_BINARY_MODE_IN() == -1) {
		std::string err("ERROR: while converting cin to binary:");
		err.append(strerror(errno));
		return panic(err.c_str(), -10);
	}
	_ismemory		= false;
	_is_read_end	= 1;
	std::string		bType = "boundary=";
	size_t			pos = _content_type.find(bType);
	// generate the separators
	std::string		sep = _content_type.substr(pos + bType.size());
	sep.insert(0, "--");
	std::string		line;
	int				file_count = 0;
	bool			start = false;
	std::string		file_name;
	std::ofstream	file;
	std::string		temp_dir_str(temp_dir);
	sow_web_jsx::format__path(temp_dir_str);
	size_t			sep_len = sep.size();
	bool			is_saved = true;
	std::istream::sentry sentry(std::cin, true);
	size_t			file_total_size = 0;
	size_t			block_size = 0;
	size_t			read_size = 0;
	const size_t	max_block_size = 10485760;/*(Max block size (1024*1024)*10) = 10 MB*/
	/* initialize random seed: */
	//srand(time(NULL));
	while (sow_web_jsx::get_line(std::cin, line)){
		if (std::cin.good() == false)break;
		if (std::cin.fail() == true)break;
		if (line.size() < sep_len || line.find(sep) == std::string::npos) {
			if (!start)continue;
			read_size = line.size();
			file.write(line.c_str(), read_size);
			file_total_size += read_size;
			block_size += read_size;
			if (std::cin.eof()) {
				//_posted_files->file->set_file_size(static_cast<size_t>(file.tellp()));
				_posted_files->file->set_file_size(file_total_size);
				file.flush(); file.close();
				/*is_saved = true; start = false;
				file_total_size = 0; block_size = 0;*/
				break;
			}
			else {
				if (block_size >= max_block_size) {
					file.flush(); block_size = 0;
				}
			}
			continue;
		}
		if (std::cin.eof()) {
			if (start) {
				//_posted_files->file->set_file_size(static_cast<size_t>(file.tellp()));
				_posted_files->file->set_file_size(file_total_size);
				file.flush(); file.close();
				is_saved = true; file_total_size = 0; //block_size = 0;
			}
			//start = false; 
			break;
		}
		if (start) {
			//_posted_files->file->set_file_size(static_cast<size_t>(file.tellp()));
			_posted_files->file->set_file_size(file_total_size);
			file.flush(); file.close();
			start = false; is_saved = true; file_total_size = 0; block_size = 0;
		}
		int info_count = 0;
		line.clear();
		std::string header_info("");
		while (true) {
			info_count++;
			if (sow_web_jsx::get_line(std::cin, line).eof())break;
			header_info.append(line.c_str());
			if (info_count >= 2)break;
		}
		if (sow_web_jsx::get_line(std::cin, line).eof())break;//Last CRLF
		header_info.append(line.c_str());
		posted_files* pf = new posted_files();
		pf->file = parse_header(header_info);
		if (pf->file->is_empty_header()) {
			pf->file->clear();
			delete pf->file;
			delete pf;
			continue;
		}
		if (!file_name.empty())file_name.clear();
		file_name = std::string(temp_dir_str.c_str());
		file_name.append(random_string(15)).append(std::to_string(file_count));
		file.open(file_name, std::ofstream::out | std::ofstream::binary);
		if (file.is_open() == false) {
			std::string err("Unable to open file to ->");
			err.append(temp_dir);
			return panic(err.c_str(), -1);
		}
		is_saved = false;
		start = true;
		pf->file->store(file_name.c_str());
		if (_posted_files != NULL) {
			pf->next = std::move(_posted_files);
		}
		else {
			pf->next = NULL;
		}
		_posted_files = pf;
		header_info.clear();
		file_count++;
	}
	fclose(stdin);
	if (is_saved == false) {
		_posted_files->file->set_file_size(file_total_size);
		file.flush(); file.close();
	}
	temp_dir_str.clear();
	if (!line.empty())line.clear();
	return file_count;
}
int http_payload::save_to_file(const char* dir) {
	int rec = this->read_all(dir);
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
size_t http_payload::get_stream_size() {
	_stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = _stream.tellg();
	_stream.seekg(0, std::ios::beg);//Back to begain of stream
	return (size_t)totalSize;
}
int http_payload::read_all() {
	if (_errc < 0 || _is_disposed == true)return 0;
	if (_is_read_end > 0)return 0;
	if (_content_length == 0) {
		return panic("No payload found in current request", -1);
	}
	try {
		if (this->is_multipart()) {
			return panic("Multipart not allowed here... Please provide TEMP_DIRECTORY...", -1);
		}
		if (SET_BINARY_MODE_IN() == -1) {
			std::string err("ERROR: while converting cin to binary:");
			err.append(strerror(errno));
			return panic(err.c_str(), -10);
		}
		_is_read_end = 1;
		_stream = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		read_payload(_stream, _content_length);
		size_t size = get_stream_size();
		if (size == 0 || size == std::string::npos) {
			return panic("Unable to read payload from current request", -1);
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
bool http_payload::is_multipart() {
	return _is_multipart != 0;
}
bool http_payload::is_read_end(){
	return _is_read_end != 0;
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
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	/*_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);*/
	_errc = code;
	return _errc;
}
int http_payload::get_total_file() {
	if (is_multipart())return _file_count;
	return panic("File count allowed only Multipart posted stream....", -1);
	
}
int http_payload::get_body(std::string& body) {
	if (_is_disposed)return FALSE;
	if (_is_read_end <= 0)return FALSE;
	if (is_multipart())return FALSE;
	size_t size = get_stream_size();
	if (size == 0 || size == std::string::npos)return FALSE;
	body = _stream.str().c_str();
	return TRUE;
}
v8::Local<v8::Object> get_file_obj(v8::Isolate* isolate, http_posted_file* pf) {
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate);
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::Object>global = isolate->GetCurrentContext()->Global();
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "save_as", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Absolute path required!!!")));
			return;
		}
		native_string absolute_v8_str(isolate, args[0]);
		std::string std_str(absolute_v8_str.c_str());
		sow_web_jsx::format__path(std_str);
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
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
		else if (ret == -501) {
			std_str = "Unable to move file. Error#";
			std_str.append(strerror(errno));
		}
		absolute_v8_str.clear();
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, std_str.c_str())));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_data", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		if (app->is_text_content()) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Readable chracter not avilable for this content type.")));
			return;
		}
		const char* data = app->get_data();
		if (strlen(data) == 0) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "No data found in current file object.")));
			return;
		}
		args.GetReturnValue().Set(v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_disposition", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		const char* data = app->get_content_disposition();
		args.GetReturnValue().Set(v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_file_size", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		size_t len = app->get_file_size();
		args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(len)));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		const char* data = app->get_name();
		args.GetReturnValue().Set(v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_file_name", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		const char* data = app->get_file_name();
		args.GetReturnValue().Set(v8_str(isolate, data));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_content_type", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_posted_file* app = static_cast<http_posted_file*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		const char* data = app->get_content_type();
		args.GetReturnValue().Set(v8_str(isolate, data));
		return;
	}, global));

	v8::Handle<v8::Object> exports = appTemplate->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
	exports->SetAlignedPointerInInternalField(0, pf);
	v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, exports);
	pobj.SetWeak<http_posted_file*>(&pf, [](const v8::WeakCallbackInfo<http_posted_file*>& data) {
		delete[] data.GetParameter();
	}, v8::WeakCallbackType::kParameter);
	return exports;
}
void sow_web_jsx::read_http_posted_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "Contenent length required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8_str(isolate, "ContentType required!!!")));
		return;
	}
	v8::Local<v8::FunctionTemplate> appTemplate = v8::FunctionTemplate::New(isolate);
	appTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::Object>global = args.Holder();
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "save_to_file", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Directory required!!!")));
			return;
		}
		native_string save_dir(isolate, args[0]);
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		int len = app->save_to_file(save_dir.c_str()); save_dir.clear();
		args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(len)));
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_line", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Callback should be a Function instance!!!")));
			return;
		}
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		v8::Persistent<v8::Function> cb;
		cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
		int ret = app->read_line([&isolate, &callback](const char* data, size_t size) {
			v8::Local<v8::Value> argv[2] = {
				v8::String::NewFromUtf8(isolate, data),
				v8::Number::New(isolate, static_cast<double>(size))
			};
			callback->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 2, argv);
			argv->Clear();
		});
		callback.Clear();
		if (ret < 0) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, app->get_last_error())));
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_all", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		v8::Isolate* isolate = args.GetIsolate();
		int ret = 0;
		if (args[0]->IsString()) {
			native_string temp_dir(isolate, args[0]);
			ret = app->read_all(temp_dir.c_str());
		}
		else {
			ret = app->read_all();
		}
		if (ret < 0) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, app->get_last_error())));
			return;
		}
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "is_multipart", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		bool ret = app->is_multipart();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "get_body", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		if (app->is_multipart()) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Multipart posted file not allowed.")));
			return;
		}
		if (app->is_read_end() == false) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "No posted data read yet from current context. Please use before read_all...")));
			return;
		}
		std::string body;
		if (app->get_body(body) == FALSE) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "No data found in current context.")));
			return;
		}
		args.GetReturnValue().Set(v8_str(isolate, body.c_str()));
		body.clear(); std::string().swap(body);
		return;
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "has_error", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		int ret = app->has_error();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Boolean::New(isolate, ret < 0 ? true : false));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "total_file", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		int ret = app->get_total_file();
		v8::Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}, global));
	appTemplate->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, "read_files", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Callback should be a Function instance!!!")));
			return;
		}
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
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
		http_payload* app = static_cast<http_payload*>(args.Holder()->GetAlignedPointerFromInternalField(0));
		if (app == NULL ) {
			v8::Isolate* isolate = args.GetIsolate();
			isolate->ThrowException(v8::Exception::Error(
				v8_str(isolate, "`http_payload` Object already released!!!")));
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
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, localApp);
		pobj.SetWeak<http_payload*>(&hp, [](const v8::WeakCallbackInfo<http_payload*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	}
	args.GetReturnValue().Set(localApp);
}
void sow_web_jsx::write_file_from_payload(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Contenent length required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "ContentType required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Upload directory required!!!")));
		return;
	}
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	size_t content_length = static_cast<size_t>(num->Value());
	native_string ctype(isolate, args[1]);
	native_string upload_dir(isolate, args[2]);
	http_payload* app = new http_payload(ctype.c_str(), content_length);
	int rec = app->read_all(upload_dir.c_str());
	if (rec < 0) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, app->get_last_error())));
	}
	else {
		rec = app->save_to_file(upload_dir.c_str());
		if (rec < 0) {
			isolate->ThrowException(v8::Exception::Error(v8_str(isolate, app->get_last_error())));
		}
	}
	if (rec >= 0) {
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
	}
	app->clear(); delete app; ctype.clear(); upload_dir.clear();
}
void sow_web_jsx::read_payload(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Contenent length required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "ContentType required!!!")));
		return;
	}
	if (!args[2]->IsFunction() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Callback required!!!")));
		return;
	}
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
	v8::Local<v8::Number> num = args[0]->ToNumber(ctx).ToLocalChecked();
	size_t content_length = static_cast<size_t>(num->Value());
	native_string ctype(isolate, args[1]);
	http_payload* app = new http_payload(ctype.c_str(), content_length);
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[2]);
	int ret = app->read_line([&isolate, &callback](const char* data, size_t size) {
		v8::Local<v8::Value> argv[2] = {
			v8::String::NewFromUtf8(isolate, data),
			v8::Number::New(isolate, static_cast<double>(size))
		};
		callback->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 2, argv);
		argv->Clear();
	});
	callback.Clear();
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, app->get_last_error())));
	}
	else {
		args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	}
	app->clear(); delete app; ctype.clear();
}
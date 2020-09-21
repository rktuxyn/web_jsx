/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "bitmap.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include <web_jsx/base64.h>
#	include <web_jsx/wjsx_env.h>
//#pragma warning (disable : 4231)
//#pragma warning(disable : 4996)
#	include <iostream>
#	include <fstream>
#	include <sstream>
#	include <string>
#	include <vector>

#if !defined(READ_CHUNK)
#	define READ_CHUNK		16384
#endif//!READ_CHUNK

typedef struct {
	uint8_t r, g, b, a;
} rgb32;
#pragma pack(2)
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} bitmap_file_header;
#pragma pack()
#pragma pack(2)
typedef struct {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int16_t biXPelsPerMeter;
	int16_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} bitmap_info_header;
#pragma pack()
#pragma pack(2)
typedef struct {
	bitmap_file_header fHeader;
	bitmap_info_header iHeader;
} bitmap_header;
#pragma pack()
//Read more about convert image_format
//https://www.codeproject.com/Articles/1300/CxImage
enum image_format {
	BMP = 0,
	PNG = 1,
	JPEG = 2,
	JPG = 3,
	GIF = 4,
	TIFF = 5,
	TIF = 5
};
class bitmap {
private:
	bitmap_header* _header;
	uint8_t* _pixels;
	int _is_error;
	char* _internal_error;
	int _is_loaded;
	std::string* _input_file_path;
	image_format _format;
	int panic(const char* error, int error_code);
	template<class _source_stream>
	int load_from_stream(_source_stream& stream);
	template<class _source_stream>
	int save_to_stream(_source_stream& stream, int is_memory, image_format format);
	int save_to_vector(std::vector<char>& dest, image_format format);
	//int set_pixel(uint8_t* _pixels, rgb32* pixel, uint32_t x, uint32_t y);
public:
	explicit bitmap(image_format format);
	explicit bitmap(const char* path, image_format format);
	//creates a deep copy of the source image.
	bitmap(const bitmap& other);
	~bitmap();
	int load(const char* path);
	int from_base64(const char* data);
	int to_base64(std::string& out);
	int save(const char* path, image_format format);
	rgb32* get_pixel(uint32_t x, uint32_t y) const;
	int set_pixel(rgb32* pixel, uint32_t x, uint32_t y);
	int resize(bitmap& dest);
	int resize(const uint32_t new_width, const uint32_t new_height);
	int create_canvas(const uint32_t width, const uint32_t heigh);
	uint32_t get_width() const;
	uint32_t get_height() const;
	image_format get_format() const;
	int has_error();
	int is_loaded() const;
	const char* get_last_error();
	void free_memory();
	void reset_rgb();
	void clear();
	void dump_data();
	uint8_t* data()const;
	bitmap_header* header()const;
	// Assignment operator creates a deep copy of the source image.
	//bitmap& operator = (const bitmap& other);
};
template<class _source_stream>
inline int bitmap::load_from_stream(_source_stream& stream) {
	if (_header == NULL) {
		_header = new bitmap_header;
	}
	stream.read(reinterpret_cast<char*>(&_header->fHeader), sizeof(_header->fHeader));
	if (_header->fHeader.bfType != 0x4d42) {
		return this->panic("Invalid format. Only bitmaps are supported.", TRUE);
	}
	stream.read(reinterpret_cast<char*>(&_header->iHeader), sizeof(_header->iHeader));
	if (_header->iHeader.biCompression != 0) {
		return this->panic("Invalid bitmap. Only uncompressed bitmaps are supported.", TRUE);
	}
	if (_header->iHeader.biBitCount != 24) {
		return this->panic("Invalid bitmap. Only 24bit bitmaps are supported.", TRUE);
	}
	stream.seekg(_header->fHeader.bfOffBits, std::ios::beg);
	_pixels = new uint8_t[_header->fHeader.bfSize - _header->fHeader.bfOffBits];
	stream.read(reinterpret_cast<char*>(&_pixels[0]), _header->fHeader.bfSize - _header->fHeader.bfOffBits);
	size_t buff_len = (_header->iHeader.biWidth * _header->iHeader.biHeight * sizeof(rgb32));
	uint8_t* temp = new uint8_t[buff_len];
	uint8_t* in = _pixels;
	rgb32* out = reinterpret_cast<rgb32*>(temp);
	int padding = (_header->iHeader.biSizeImage - _header->iHeader.biWidth * _header->iHeader.biHeight * 3) / _header->iHeader.biHeight;
	for (int i = 0; i < _header->iHeader.biHeight; ++i, in += padding) {
		for (int j = 0; j < _header->iHeader.biWidth; ++j) {
			out->b = *(in++);
			out->g = *(in++);
			out->r = *(in++);
			out->a = 0xFF;
			++out;
		}
	}
	delete[] _pixels;
	_pixels = temp;
	_is_loaded = TRUE;
	return _is_loaded;
}
template<class _source_stream>
inline int bitmap::save_to_stream(_source_stream& stream, int is_memory, image_format format) {
	stream.write(reinterpret_cast<char*>(&_header->fHeader), sizeof(_header->fHeader));
	stream.write(reinterpret_cast<char*>(&_header->iHeader), sizeof(_header->iHeader));
	stream.seekp(_header->fHeader.bfOffBits, std::ios::beg);
	uint8_t* temp = new uint8_t[_header->fHeader.bfSize - _header->fHeader.bfOffBits];
	uint8_t* out = temp;
	rgb32* in = reinterpret_cast<rgb32*>(_pixels);
	//int nPadding = ((_header->iHeader.biWidth / 4) + 1) * 4;
	int padding = (_header->iHeader.biSizeImage - _header->iHeader.biWidth * _header->iHeader.biHeight * 3) / _header->iHeader.biHeight;
	for (int i = 0; i < _header->iHeader.biHeight; ++i, out += padding) {
		for (int j = 0; j < _header->iHeader.biWidth; ++j) {
			*(out++) = in->b;
			*(out++) = in->g;
			*(out++) = in->r;
			++in;
		}
	}
	stream.write(reinterpret_cast<char*>(&temp[0]), _header->fHeader.bfSize - _header->fHeader.bfOffBits);
	delete[] temp;
	return TRUE;
}
//12:43 AM 1/11/2020
void insert_vct(std::vector<char>& dest, char* data, size_t len) {
	dest.insert(dest.end(), data, len + data);
}
bitmap::bitmap(image_format format) {
	_internal_error = NULL; _is_error = FALSE; _is_loaded = FALSE;
	_header = new bitmap_header; _pixels = NULL; _input_file_path = NULL;
	_format = format;
	if (_format != BMP) {
		this->panic("Unsupported Image format.", TRUE);
	}
}
bitmap::bitmap(const char* path, image_format format){
	_internal_error = NULL; _is_error = FALSE; _is_loaded = FALSE;
	_header = new bitmap_header; _pixels = NULL;
	_input_file_path = new std::string(path);
	_format = format;
	if (_format != BMP) {
		this->panic("Unsupported Image format.", TRUE);
	}
	return;
}
//creates a deep copy of the source image.
bitmap::bitmap(const bitmap& other){
	if (other.is_loaded() == FALSE)return;
	this->free_memory();
	_header = new bitmap_header;
	memcpy(_header, other.header(), sizeof(bitmap_header));
	size_t len = _header->iHeader.biWidth * _header->iHeader.biHeight * sizeof(rgb32);
	_pixels = new uint8_t[len];
	memcpy(_pixels, other.data(), len);
	_is_loaded = TRUE;
	_format = BMP;
	return;
}
int bitmap::load(const char* path) {
	this->free_memory();
	if (path == NULL && _input_file_path == NULL) {
		return this->panic("No input file path found...", TRUE);
	}
	if (_format != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	std::ifstream* file;
	if (path != NULL) {
		file = new std::ifstream(path, std::ios::in | std::ios::binary);
	}
	else {
		file = new std::ifstream(_input_file_path->c_str(), std::ios::in | std::ios::binary);
	}
	file->setf(std::ios_base::binary);
	if (!file->is_open()) {
		delete file;
		return this->panic("Unable to open file", TRUE);
	}
	int ret = this->load_from_stream(*file);
	file->close(); delete file;
	if (path != NULL) {
		_free_obj(_input_file_path);
		_input_file_path = new std::string(path);
	}
	return ret;
}
int bitmap::from_base64(const char* data){
	this->free_memory();
	_NEW_STR(docoded_data);
	int ret = sow_web_jsx::base64::to_decode_str(data, *docoded_data) == true ? TRUE : FALSE;
	if (is_error_code(ret) == TRUE) {
		docoded_data->clear(); delete docoded_data;
		return ret;
	}
	std::stringstream* stream = new std::stringstream(std::stringstream::in | std::ios::out | std::stringstream::binary);
	stream->write(docoded_data->c_str(), docoded_data->size()); 
	_free_obj(docoded_data);
	ret = this->load_from_stream(*stream);
	stream->clear(); delete stream;
	return ret;
}
int bitmap::save_to_vector(std::vector<char>& dest, image_format format) {
	dest.reserve(_header->fHeader.bfSize);
	insert_vct(dest, reinterpret_cast<char*>(&_header->fHeader), sizeof(_header->fHeader));
	insert_vct(dest, reinterpret_cast<char*>(&_header->iHeader), sizeof(_header->iHeader));
	uint8_t* temp = new uint8_t[_header->fHeader.bfSize - _header->fHeader.bfOffBits];
	uint8_t* out = temp;
	rgb32* in = reinterpret_cast<rgb32*>(_pixels);
	//int nPadding = ((_header->iHeader.biWidth / 4) + 1) * 4;
	int padding = (_header->iHeader.biSizeImage - _header->iHeader.biWidth * _header->iHeader.biHeight * 3) / _header->iHeader.biHeight;
	for (int i = 0; i < _header->iHeader.biHeight; ++i, out += padding) {
		for (int j = 0; j < _header->iHeader.biWidth; ++j) {
			*(out++) = in->b;
			*(out++) = in->g;
			*(out++) = in->r;
			++in;
		}
	}
	insert_vct(dest, reinterpret_cast<char*>(&temp[0]), _header->fHeader.bfSize - _header->fHeader.bfOffBits);
	delete[] temp;
	return TRUE;
}
int bitmap::to_base64(std::string& out) {
	std::vector<char>* dest = new std::vector<char>();
	int ret = FALSE;
	if (_input_file_path != NULL) {
		std::ifstream* file = new std::ifstream(_input_file_path->c_str(), std::ios::in | std::ios::binary);
		if (!file->is_open()) {
			delete file; _free_obj(dest);
			return this->panic("Unable to open file", TRUE);
		}
		ret = load_file_to_vct(*file, *dest);
		file->close();
	}
	else {
		if (_pixels == NULL) {
			_free_obj(dest);
			return FALSE;
		}
		ret = save_to_vector(*dest, image_format::BMP);
	}
	if (is_error_code(ret) == FALSE) {
		std::string* data = new std::string(dest->data(), dest->size());
		ret = sow_web_jsx::base64::to_encode_str(*data, out) == true ? TRUE : FALSE;
		_free_obj(data);
	}
	_free_obj(dest);
	return ret;
}
bitmap::~bitmap() {
	this->free_memory();
}
int bitmap::save(const char* path, image_format format) {
	if (_format != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (_is_loaded == FALSE) {
		return this->panic("Please Load an Image than try again.", TRUE);
	}
	std::ofstream* file = new std::ofstream(path, std::ios::out | std::ios::binary);
	if (!file->is_open()) {
		return this->panic("Unable to open file", TRUE);
	}
	save_to_stream(*file, FALSE, BMP);
	file->close(); delete file;
	return TRUE;
}
rgb32* bitmap::get_pixel(uint32_t x, uint32_t y) const {
	if (_format != BMP) {
		return NULL;
	}
	rgb32* temp = reinterpret_cast<rgb32*>(_pixels);
	return &temp[(_header->iHeader.biHeight - 1 - y) * _header->iHeader.biWidth + x];
}
int bitmap::set_pixel(rgb32* pixel, uint32_t x, uint32_t y) {
	if (_format != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	rgb32* temp = reinterpret_cast<rgb32*>(_pixels);
	memcpy(&temp[(_header->iHeader.biHeight - 1 - y) * _header->iHeader.biWidth + x], pixel, sizeof(rgb32));
	return TRUE;
}
void bitmap::dump_data() {
	if (_format != BMP)return;
	std::cout << "Bitmap data:" << std::endl;
	std::cout << "============" << std::endl;
	std::cout << "Width:  " << _header->iHeader.biWidth << std::endl;
	std::cout << "Height: " << _header->iHeader.biHeight << std::endl;
	std::cout << "Data:" << std::endl;
	for (int cy = 0; cy < _header->iHeader.biHeight; cy++) {
		for (int cx = 0; cx < _header->iHeader.biWidth; cx++) {
			int pixel = (cy * (_header->iHeader.biWidth * 3)) + (cx * 3);
			std::cout << "rgb(" << (int)_pixels[pixel] << "," << (int)_pixels[pixel + 1] << "," << (int)_pixels[pixel + 2] << ") ";
		}
		std::cout << std::endl;
	}
	std::cout << "_________________________________________________________" << std::endl;
}
int bitmap::resize(bitmap& dest) {
	if (_format != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (dest.get_format() != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (_is_loaded == FALSE)return FALSE;
	if (_pixels == NULL) return FALSE;
	uint32_t new_width = dest.get_width(), new_height = dest.get_height();
	double scale_width = static_cast<double>(new_width) / static_cast<double>(_header->iHeader.biWidth);
	double scale_height = static_cast<double>(new_height) / static_cast<double>(_header->iHeader.biHeight);
	for (uint32_t y = 0; y < new_height; ++y) {
		for (uint32_t x = 0; x < new_width; ++x) {
			rgb32* tmp_pixel = get_pixel(static_cast<uint32_t>(x / scale_width), static_cast<uint32_t>(y / scale_height));
			dest.set_pixel(tmp_pixel, x, y);
		}
	}
	return TRUE;
}
uint8_t* bitmap::data()const {
	return _pixels;
}
bitmap_header* bitmap::header()const {
	return _header;
}
int bitmap::resize(const uint32_t new_width, const uint32_t new_height){
	if (_format != BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	bitmap* bmp = new bitmap(BMP);
	int ret = bmp->create_canvas(new_width, new_height);
	if (is_error_code(ret) == TRUE)return ret;
	ret = this->resize(*bmp);
	if (is_error_code(ret) == TRUE)return ret;
	*this = *bmp;
	return ret;
}
int bitmap::create_canvas(const uint32_t width, const uint32_t height){
	this->free_memory();
	if (width % 4 != 0) {
		return panic("There is a windows-imposed requirement on BMP that the width be a multiple of 4", TRUE);
	}
	_header = new bitmap_header;
	_header->fHeader.bfType = 0x4d42;
	_header->iHeader.biSize = 40;//
	_header->fHeader.bfOffBits = 14 + _header->iHeader.biSize;
	_header->fHeader.bfSize = _header->fHeader.bfOffBits + (width * height * 3);
	_header->fHeader.bfReserved1 = 0;
	_header->fHeader.bfReserved2 = 0;
	_header->iHeader.biWidth = width;
	_header->iHeader.biHeight = height;
	_header->iHeader.biPlanes = 1;
	_header->iHeader.biBitCount = 24;
	_header->iHeader.biCompression = 0;
	_header->iHeader.biSizeImage = width * height * (uint32_t)(_header->iHeader.biBitCount / 8);
	_is_loaded = TRUE; _is_error = FALSE;
	this->reset_rgb();
	return TRUE;
}
//Draw white background
void bitmap::reset_rgb() {
	if (_is_loaded == FALSE)return;
	if (_format != BMP)return;
	rgb32* in = new rgb32();
	in->r = (uint8_t)(0xff);
	in->g = (uint8_t)(0xff);
	in->b = (uint8_t)(0xff);
	in->a = (uint8_t)(0xff);
	if (_pixels != NULL) {
		delete[]_pixels; _pixels = NULL;
	}
	_pixels = new uint8_t[_header->iHeader.biWidth * _header->iHeader.biHeight * sizeof(rgb32)];
	rgb32* out = reinterpret_cast<rgb32*>(_pixels);
	for (int32_t i = 0; i < _header->iHeader.biHeight; ++i) {
		for (int32_t j = 0; j < _header->iHeader.biWidth; ++j) {
			out->b = in->b;
			out->g = in->g;
			out->r = in->r;
			out->a = in->a;
			++out;
		}
	}
	delete in;
}
uint32_t bitmap::get_width() const {
	return _header->iHeader.biWidth;
}
uint32_t bitmap::get_height() const {
	return _header->iHeader.biHeight;
}
image_format bitmap::get_format() const{
	return _format;
}
int bitmap::has_error(){
	return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
}
int bitmap::is_loaded() const {
	return _is_loaded;
}
const char* bitmap::get_last_error() {
	if (has_error() == TRUE) {
		return const_cast<const char*>(_internal_error);
	}
	return "No reason found from context!!!";

}

void bitmap::free_memory(){
	_free_char(_internal_error);
	_free_obj(_input_file_path);
	if (_pixels != NULL) {
		delete[] _pixels; _pixels = NULL;
	}
	if (_header != NULL) {
		delete _header; _header = NULL;
	}
	_is_error = FALSE; _is_loaded = FALSE;
	return;
}
void bitmap::clear(){
	this->free_memory();
}
int bitmap::panic(const char* error, int error_code) {
	_free_char(_internal_error);
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	if (error_code >= 0)error_code = error_code * -1;
	_is_error = error_code;
	return _is_error;
}
using namespace sow_web_jsx;
void bitmap_export(v8::Isolate* isolate, v8::Handle<v8::Object> target) {
	v8::Local<v8::FunctionTemplate> bit_map_tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		bitmap* bmp = NULL;
		if (args[0]->IsString()) {
			native_string utf_str(isolate, args[0]);
			std::string* abs_path = new std::string(wj_env->get_root_dir());
			sow_web_jsx::get_server_map_path(utf_str.c_str(), *abs_path);

			bmp = new bitmap(abs_path->c_str(), image_format::BMP);
			utf_str.clear(); _free_obj(abs_path);
		}
		else {
			bmp = new bitmap(image_format::BMP);
		}
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, bmp));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<bitmap*>(&bmp, [](const v8::WeakCallbackInfo<bitmap*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	bit_map_tpl->SetClassName(v8_str(isolate, "BitMap"));
	bit_map_tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = bit_map_tpl->PrototypeTemplate();
	set_prototype(isolate, prototype, "release", [](js_method_args) {
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL)return;
		bmp->free_memory();
		delete bmp; bmp = NULL;
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
	});
	set_prototype(isolate, prototype, "lock_bits", [](js_method_args) {
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "unlock_bits", [](js_method_args) {
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "reset", [](js_method_args) {
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		bmp->reset_rgb();
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "release_mem", [](js_method_args) {
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL)return;
		bmp->clear();
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "dump_data", [](js_method_args) {
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		bmp->dump_data();
		args.GetReturnValue().Set(args.Holder());
	});
	set_prototype(isolate, prototype, "load", [](js_method_args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		std::string* abs_path = new std::string(wj_env->get_root_dir());
		::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int ret = bmp->load(abs_path->c_str());
		_free_obj(abs_path); utf_abs_path_str.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	});
	set_prototype(isolate, prototype, "to_base64", [](js_method_args) {
		bitmap* bmp = ::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		std::string* out = new std::string();
		int ret = bmp->to_base64(*out);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), "Unable to convert base64 image...");
		}
		else {
			args.GetReturnValue().Set(v8_str(args.GetIsolate(), out->c_str()));
		}
		_free_obj(out);
	});
	set_prototype(isolate, prototype, "load_from_base64", [](js_method_args) {
		if (!args[0]->IsString()) {
			throw_js_type_error(args.GetIsolate(), "base64 Data required....");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(isolate, "bitmap object disposed...");
			return;
		}
		native_string utf_base64(isolate, args[0]);
		int ret = bmp->from_base64(utf_base64.c_str());
		utf_base64.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	});
	set_prototype(isolate, prototype, "save", [](js_method_args) {
		if (!args[0]->IsString()) {
			throw_js_type_error(args.GetIsolate(), "File Path required....");
			return;
		}
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		native_string utf_abs_path_str(isolate, args[0]);
		wjsx_env* wj_env = ::unwrap_wjsx_env(isolate);
		std::string* abs_path = new std::string(wj_env->get_root_dir());
		sow_web_jsx::get_server_map_path(utf_abs_path_str.c_str(), *abs_path);
		int ret = bmp->save(abs_path->c_str(), image_format::BMP);
		_free_obj(abs_path); utf_abs_path_str.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	});
	set_prototype(isolate, prototype, "get_width", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		uint32_t width = bmp->get_width();
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<int>(width)));
	});
	set_prototype(isolate, prototype, "get_height", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		uint32_t height = bmp->get_height();
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<int>(height)));
	});
	set_prototype(isolate, prototype, "get_pixel", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), bmp->get_last_error());
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(args.GetIsolate(), "x y required...");
			return;
		}
		if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "x y should be number...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int x = args[0]->Int32Value(ctx).FromMaybe(0);
		int y = args[1]->Int32Value(ctx).FromMaybe(0);
		rgb32* pixel = bmp->get_pixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8_result->Set(ctx, v8_str(isolate, "r"), v8::Integer::New(isolate, static_cast<int>(pixel->r)));
		v8_result->Set(ctx, v8_str(isolate, "g"), v8::Integer::New(isolate, static_cast<int>(pixel->g)));
		v8_result->Set(ctx, v8_str(isolate, "b"), v8::Integer::New(isolate, static_cast<int>(pixel->b)));
		v8_result->Set(ctx, v8_str(isolate, "a"), v8::Integer::New(isolate, static_cast<int>(pixel->a)));
		args.GetReturnValue().Set(v8_result); v8_result.Clear();
	});
	set_prototype(isolate, prototype, "resize", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), bmp->get_last_error());
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(args.GetIsolate(), "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_type_error(args.GetIsolate(), "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_type_error(args.GetIsolate(), "Width required...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	});
	set_prototype(isolate, prototype, "create_canvas", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(args.GetIsolate(), "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_type_error(args.GetIsolate(), "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_type_error(args.GetIsolate(), "Width required...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->create_canvas(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	});
	set_prototype(isolate, prototype, "set_pixel", [](js_method_args) {
		bitmap* bmp = sow_web_jsx::unwrap<bitmap>(args);
		if (bmp == NULL) {
			throw_js_error(args.GetIsolate(), "bitmap object disposed...");
			return;
		}
		if (bmp->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), bmp->get_last_error());
			return;
		}
		if (args.Length() < 3) {
			throw_js_error(args.GetIsolate(), "rgba and x y required...");
			return;
		}
		if (!args[0]->IsObject()) {
			throw_js_type_error(args.GetIsolate(), "rgba should be object...");
			return;
		}
		if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
			throw_js_type_error(args.GetIsolate(), "x y should be number...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Object> rgba = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		rgb32* pixel = new rgb32();
		int val = v8_object_get_number(isolate, ctx, rgba, "r");
		if (val == -500) { delete pixel; return; }
		pixel->r = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "g");
		if (val == -500) { delete pixel; return; }
		pixel->g = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "b");
		if (val == -500) { delete pixel; return; }
		pixel->b = (uint8_t)val;
		val = v8_object_get_number(isolate, ctx, rgba, "a");
		if (val == -500) { delete pixel; return; }
		pixel->a = (uint8_t)val;
		int x = args[1]->Int32Value(ctx).FromMaybe(0);
		int y = args[2]->Int32Value(ctx).FromMaybe(0);
		int ret = bmp->set_pixel(pixel, static_cast<unsigned int>(x), static_cast<unsigned int>(y));
		//delete pixel;
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, bmp->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	});
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, "BitMap"), bit_map_tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}
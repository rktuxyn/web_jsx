/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "bitmap.h"
#	include "base64.h"
#if !defined(READ_CHUNK)
#	define READ_CHUNK		16384
#endif//!READ_CHUNK
//12:43 AM 1/11/2020
template<class _input>
inline int is_error_code(_input ret) {
	return (ret == FALSE || ret == std::string::npos || ret < 0) ? TRUE : FALSE;
}
void insert_vct(std::vector<char>& dest, char* data, size_t len) {
	dest.insert(dest.end(), data, len + data);
}
bitmap::bitmap(image_format format) {
	_internal_error = new char; _is_error = FALSE; _is_loaded = FALSE;
	_header = new bitmap_header; _pixels = NULL; _input_file_path = NULL;
	_format = format;
	if (_format != image_format::BMP) {
		this->panic("Unsupported Image format.", TRUE);
	}
}
bitmap::bitmap(const char* path, image_format format){
	_internal_error = new char; _is_error = FALSE; _is_loaded = FALSE;
	_header = new bitmap_header; _pixels = NULL;
	_input_file_path = new std::string(path);
	_format = format;
	if (_format != image_format::BMP) {
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
	_format = image_format::BMP;
	return;
}
int bitmap::load(const char* path) {
	this->free_memory();
	if (path == NULL && _input_file_path == NULL) {
		return this->panic("No input file path found...", TRUE);
	}
	if (_format != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	std::ifstream* file;
	if (path != NULL) {
		file = new std::ifstream(path, std::ios::in | std::ios::binary);
	}
	else {
		file = new std::ifstream(_input_file_path->c_str(), std::ios::in | std::ios::binary);
	}
	if (!file->is_open()) {
		delete file;
		return this->panic("Unable to open file", TRUE);
	}
	int ret = this->load_from_stream(*file);
	file->close(); delete file;
	if (path != NULL) {
		if (_input_file_path != NULL) {
			_input_file_path->clear(); delete _input_file_path; _input_file_path = NULL;
		}
		_input_file_path = new std::string(path);
	}
	return ret;
}
int bitmap::from_base64(const char* data){
	this->free_memory();
	std::string* docoded_data = new std::string();
	int ret = sow_web_jsx::base64::to_decode_str(data, *docoded_data) == true ? TRUE : FALSE;
	if (is_error_code(ret) == TRUE) {
		docoded_data->clear(); delete docoded_data;
		return ret;
	}
	std::stringstream* stream = new std::stringstream(std::stringstream::in | std::ios::out | std::stringstream::binary);
	stream->write(docoded_data->c_str(), docoded_data->size()); 
	docoded_data->clear(); delete docoded_data;
	ret = this->load_from_stream(*stream);
	stream->clear(); delete stream;
	return ret;
}
int load_file_to_vct(std::ifstream& file_stream, std::vector<char>& dest) {
	file_stream.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff totalSize = file_stream.tellg();
	size_t total_len = (size_t)totalSize;
	file_stream.seekg(0, std::ios::beg);//Back to begain of stream
	if (total_len == std::string::npos || total_len == 0)return FALSE;
	//size_t read_len = 0;
	dest.reserve(total_len);
	do {
		if (!file_stream.good())break;
		size_t read_len = totalSize > READ_CHUNK ? READ_CHUNK : totalSize;
		char* in = new char[read_len];
		file_stream.read(in, read_len);
		totalSize -= read_len;
		//stream.write(in, read_len);
		dest.insert(dest.end(), in, read_len + in);
		/* Free memory */
		delete[]in;
		if (totalSize <= 0) break;
	} while (true);
	return TRUE;
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
			delete file; delete dest;
			return this->panic("Unable to open file", TRUE);
		}
		ret = load_file_to_vct(*file, *dest);
		file->close(); delete file;
	}
	else {
		if (_pixels == NULL) {
			delete dest;
			return FALSE;
		}
		ret = save_to_vector(*dest, image_format::BMP);
	}
	//ret = save_to_vector(*dest, image_format::BMP);
	if (is_error_code(ret) == FALSE) {
		std::string* data = new std::string(dest->data(), dest->size());
		ret = sow_web_jsx::base64::to_encode_str(*data, out) == true ? TRUE : FALSE;
		data->clear(); delete data;
	}
	dest->clear(); delete dest;
	return ret;
}
bitmap::~bitmap() {
	this->free_memory();
}
int bitmap::save(const char* path, image_format format) {
	if (_format != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (_is_loaded == FALSE) {
		return this->panic("Please Load an Image than try again.", TRUE);
	}
	std::ofstream* file = new std::ofstream(path, std::ios::out | std::ios::binary);
	if (!file->is_open()) {
		return this->panic("Unable to open file", TRUE);
	}
	save_to_stream(*file, FALSE, image_format::BMP);
	file->close(); delete file;
	return TRUE;
}
rgb32* bitmap::get_pixel(uint32_t x, uint32_t y) const {
	if (_format != image_format::BMP) {
		return NULL;
	}
	rgb32* temp = reinterpret_cast<rgb32*>(_pixels);
	return &temp[(_header->iHeader.biHeight - 1 - y) * _header->iHeader.biWidth + x];
}
int bitmap::set_pixel(rgb32* pixel, uint32_t x, uint32_t y) {
	if (_format != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	rgb32* temp = reinterpret_cast<rgb32*>(_pixels);
	memcpy(&temp[(_header->iHeader.biHeight - 1 - y) * _header->iHeader.biWidth + x], pixel, sizeof(rgb32));
	return TRUE;
}
void bitmap::dump_data() {
	if (_format != image_format::BMP)return;
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
	if (_format != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (dest.get_format() != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	if (_is_loaded == FALSE)return FALSE;
	if (_pixels == NULL) return FALSE;
	uint32_t new_width = dest.get_width(), new_height = dest.get_height();
	//if (new_width == _header->iHeader.biWidth && new_height == _header->iHeader.biHeight)return TRUE;
	double scale_width = static_cast<double>(new_width) / static_cast<double>(_header->iHeader.biWidth);
	double scale_height = static_cast<double>(new_height) / static_cast<double>(_header->iHeader.biHeight);
	//int new_len = new_width * new_height * 3;
	//uint8_t* temp_data = new uint8_t[new_len];
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
	if (_format != image_format::BMP) {
		return this->panic("Unsupported Image format.", TRUE);
	}
	bitmap* bmp = new bitmap(image_format::BMP);
	int ret = bmp->create_canvas(new_width, new_height);
	if (is_error_code(ret) == TRUE)return ret;
	ret = this->resize(*bmp);
	if (is_error_code(ret) == TRUE)return ret;
	*this = *bmp;
	return ret;
}
//Assignment operator creates a deep copy of the source image.
//bitmap& bitmap::operator = (const bitmap& other){
//	if (other.is_loaded() == FALSE)return *this;
//	this->free_memory();
//	_header = new bitmap_header;
//	memcpy(_header, other.header(), sizeof(bitmap_header));
//	size_t len = _header->iHeader.biWidth * _header->iHeader.biHeight * sizeof(rgb32);
//	_pixels = new uint8_t[len];
//	memcpy(_pixels, other.data(), len);
//	_is_loaded = TRUE;
//	_format = image_format::BMP;
//	return *this;
//}
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
	if (_format != image_format::BMP)return;
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
	if (_internal_error != NULL) {
		delete[]_internal_error; _internal_error = NULL; _is_error = FALSE;
	}
	if (_pixels != NULL) {
		delete[] _pixels; _pixels = NULL;
	}
	if (_header != NULL) {
		delete _header; _header = NULL;
	}
	if (_input_file_path != NULL) {
		_input_file_path->clear(); delete _input_file_path; _input_file_path = NULL;
	}
	_is_error = FALSE; _is_loaded = FALSE;
	return;
}
void bitmap::clear(){
	this->free_memory();
}
int bitmap::panic(const char* error, int error_code) {
	if (_internal_error != NULL)
		delete[]_internal_error;
	size_t len = strlen(error);
	_internal_error = new char[len + 1];
	strcpy_s(_internal_error, len, error);
	/*_internal_error = new char[strlen(error) + 1];
	strcpy(_internal_error, error);*/
	if (error_code >= 0)error_code = error_code * -1;
	_is_error = error_code;
	return _is_error;
}
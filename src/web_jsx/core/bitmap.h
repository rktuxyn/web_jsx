/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//12:42 AM 1/11/2020
#if !defined(_bitmap_h)
#	define _bitmap_h
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
#	include <iostream>
#	include <fstream>
#	include <sstream>
#	include <string>
#	include <vector>
#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE
#if !defined(TRUE)
#	define TRUE                1
#endif//!TRUE
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
	int load_from_stream(_source_stream&stream);
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
#endif//!_bitmap_h
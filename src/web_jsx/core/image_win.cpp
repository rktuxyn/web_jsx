/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:57 PM 1/16/2020
#if (defined(_WIN32)||defined(_WIN64))
#	include "image_win.h"
#	include "v8_util.h"
#	include "base64.h"
#	include <iostream>
#	include <string>
#	include <algorithm>
#	include <stdio.h>
#	include <windows.h>
#	include <winbase.h>
#	include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
#if !defined(GMEM_MOVEABLE)
#	define GMEM_MOVEABLE       0x0002
#endif//!GMEM_MOVEABLE
#if !defined(PixelFormat32bppRGB)
#	define    PixelFormat32bppRGB        (9 | (32 << 8) | PixelFormatGDI)
#endif//!GMEM_MOVEABLE
#ifndef FALSE
#	define FALSE               0
#endif
#ifndef TRUE
#	define TRUE                1
#endif
#pragma warning(disable:4996)
enum image_format {
	BMP = 0,
	PNG = 1,
	JPEG = 2,
	JPG = 3,
	GIF = 4,
	TIFF = 5,
	TIF = 6,
	NO_EXT = -1,
	UNKNOWN = -2
};
typedef struct {
	uint8_t a, r, g, b;
} rgb32;
enum _mood {
	UNKNOWN_ = 0,
	READ = 1,
	WRITE = 2
};
void to_lower(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
	});
}
image_format get_image_format(const std::string& path_str) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos) return image_format::NO_EXT;
	std::string str_extension = path_str.substr(found + 1);
	to_lower(str_extension);
	if (str_extension == "jpg" ||
		str_extension == "jpeg" ||
		str_extension == "jfif" ||
		str_extension == "pjpeg" ||
		str_extension == "pjp"
		) return image_format::JPEG;
	if (str_extension == "bmp") return image_format::BMP;
	if (str_extension == "gif") return image_format::GIF;
	if (str_extension == "png") return image_format::PNG;
	if (str_extension == "tif") return image_format::TIF;
	if (str_extension == "tiff") return image_format::TIFF;
	return image_format::UNKNOWN;
}
const char* get_mime_type(image_format format) {
	if (format == image_format::BMP)return "image/bmp";
	if (format == image_format::PNG)return "image/png";
	if (format == image_format::JPEG)return "image/jpeg";
	if (format == image_format::JPG)return "image/jpeg";
	if (format == image_format::GIF)return "image/gif";
	if (format == image_format::TIFF)return "image/tiff";
	if (format == image_format::TIF)return "image/tiff";
	return NULL;
}
INT get_encoder_clsid(const WCHAR* format, CLSID* pClsid) {
	UINT num, size;
	Gdiplus::GetImageEncodersSize(&num, &size);
	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL) return -1;
	Gdiplus::Status st = Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	if (st == Gdiplus::Ok) {
		for (UINT ix = 0; ix < num; ++ix) {
			if (0 == _wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[ix].Clsid;
				free(pImageCodecInfo);
				return ix;
			}
		}
	}
	free(pImageCodecInfo);
	return -1;
}
const char* get_gdiplus_error_reason(Gdiplus::Status stat) {
	switch (stat)
	{
	case Gdiplus::Ok: return "Ok";
	case Gdiplus::GenericError:return "GenericError";
	case Gdiplus::InvalidParameter:return "InvalidParameter";
	case Gdiplus::OutOfMemory:return "OutOfMemory";
	case Gdiplus::ObjectBusy:return "ObjectBusy";
	case Gdiplus::InsufficientBuffer:return "InsufficientBuffer";
	case Gdiplus::NotImplemented:return "NotImplemented";
	case Gdiplus::Win32Error:return "Win32Error";
	case Gdiplus::WrongState:return "WrongState";
	case Gdiplus::Aborted:return "Aborted";
	case Gdiplus::FileNotFound:return "FileNotFound";
	case Gdiplus::ValueOverflow:return "ValueOverflow";
	case Gdiplus::AccessDenied:return "AccessDenied";
	case Gdiplus::UnknownImageFormat:return "UnknownImageFormat";
	case Gdiplus::FontFamilyNotFound:return "FontFamilyNotFound";
	case Gdiplus::FontStyleNotFound:return "FontStyleNotFound";
	case Gdiplus::NotTrueTypeFont:return "NotTrueTypeFont";
	case Gdiplus::UnsupportedGdiplusVersion:return "UnsupportedGdiplusVersion";
	case Gdiplus::GdiplusNotInitialized:return "GdiplusNotInitialized";
	case Gdiplus::PropertyNotFound:return "PropertyNotFound";
	case Gdiplus::PropertyNotSupported:return "PropertyNotSupported";
	default:return "Unknown";
	}
}
using namespace sow_web_jsx;
class image {
private:
	UINT _height;
	UINT _width;
	byte* _pixels;
	Gdiplus::BitmapData* _bitmap_data;
	Gdiplus::Bitmap* _bit_map;
	image_format _format;
	_mood _task_mood;
	int _gd_is_init;
	ULONG_PTR _gdiplus_token;
	int _is_loaded;
	int _errc;
	int _is_copy;
	char* _internal_error;
	__forceinline int panic(const char* error, int code) {
		if (_internal_error != NULL)
			delete[]_internal_error;
		_internal_error = new char[strlen(error) + 1];
		strcpy(_internal_error, error);
		_errc = code < 0 ? code : code * -1;
		return _errc;
	}
	__forceinline int gd_init() {
		this->gdiplus_shutdown();
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::Status  stat;
		stat = GdiplusStartup(&_gdiplus_token, &gdiplusStartupInput, NULL);
		if (stat == Gdiplus::Ok) {
			_gd_is_init = TRUE;
			return TRUE;
		}
		std::string err("Unable to init GDI+. Error reason:");
		err.append(get_gdiplus_error_reason(stat));
		this->panic(err.c_str(), TRUE);
		return _errc;
	}
public:
	__forceinline image() {
		_errc = FALSE; _is_loaded = FALSE;
		_internal_error = NULL; _pixels = NULL; _bit_map = NULL;
		_height = 0; _width = 0; _bitmap_data = NULL;
		_task_mood = _mood::UNKNOWN_; _gd_is_init = FALSE;
		_format = image_format::UNKNOWN; _gdiplus_token = NULL;
		_is_copy = FALSE;
	}
	__forceinline ~image() {
		this->release_all();
	}
	__forceinline void release_all() {
		this->free_memory();
		if (_is_copy == TRUE)return;
		this->gdiplus_shutdown();
	}
	__forceinline void gdiplus_shutdown() {
		if (_gdiplus_token != NULL) {
			Gdiplus::GdiplusShutdown(_gdiplus_token);
			_gdiplus_token = NULL;
			_gd_is_init = FALSE;
		}
		return;
	}
	__forceinline int load(const char* path) {
		this->release_all();
		if (__file_exists(path) == false) {
			return panic("ERROR: File not found...", TRUE);
		}
		_format = get_image_format(path);
		if (_format == image_format::UNKNOWN || _format == image_format::NO_EXT) {
			return panic("ERROR: Unsupported image format...", TRUE);
		}
		int ret = this->gd_init();
		if (is_error_code(ret) == TRUE)return ret;
		wchar_t* wpath = ccr2ws(path);
		_bit_map = new Gdiplus::Bitmap(const_cast<const wchar_t*>(wpath));
		_height = _bit_map->GetHeight();
		_width = _bit_map->GetWidth(); _is_loaded = TRUE;
		return TRUE;
	}
	//10:25 AM 1/19/2020
	__forceinline int load_from_buff(char* buffer, ULONG size, image_format format = image_format::BMP) {
		this->release_all();
		int ret = this->gd_init();
		if (is_error_code(ret) == TRUE)return ret;
		IStream* pStream = NULL;
		if (::CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pStream) == S_OK) {
			//ULONG ulBytesWrite;
			if (pStream->Write(buffer, size, NULL) == S_OK) {
				_bit_map = Gdiplus::Bitmap::FromStream(pStream);
				if (_bit_map != NULL) {
					Gdiplus::Status stat = _bit_map->GetLastStatus();
					if (stat == Gdiplus::Ok) {
						_height = _bit_map->GetHeight();
						_width = _bit_map->GetWidth();
						_is_loaded = TRUE; _format = format;
					}
					else {
						std::string err("Unable to create Bitmap from stream. Error reason:");
						err.append(get_gdiplus_error_reason(stat));
						ret = this->panic(err.c_str(), TRUE);
						delete _bit_map; err.clear(); std::string().swap(err);
						_bit_map = NULL;
					}
				}
			}
			else {
				ret = this->panic("Unable to write data to memmory stream.", TRUE);
			}
			pStream->Release();
		}
		else {
			ret = this->panic("Unable to create memmory stream.", TRUE);
		}
		return ret;
	}
	__forceinline int load_from_base64(const char* data, image_format format = image_format::BMP) {
		std::string* out = new std::string();
		if (sow_web_jsx::base64::to_decode_str(data, *out) == false) {
			delete out; out = NULL;
			return this->panic("Unablet to decode base64 data. Please try again.", TRUE);
		}
		int ret = this->load_from_buff(out->data(), static_cast<ULONG>(out->size()), format);
		out->clear(); delete out; out = NULL;
		return ret;
	}
	__forceinline int create_canvas(const INT width, const INT height) {
		this->release_all();
		int ret = this->gd_init();
		if (is_error_code(ret) == TRUE)return ret;
		_bit_map = new Gdiplus::Bitmap(width, height, PixelFormat32bppRGB);
		_is_loaded = TRUE; _height = height;
		_width = width; _format = image_format::BMP;
		this->reset_rgb();
		return ret;
	}
	__forceinline void reset_rgb() {
		if (_is_loaded == FALSE)return;
		_task_mood = _mood::WRITE;
		this->lock_bits(Gdiplus::ImageLockModeWrite);
		rgb32* in = new rgb32();
		in->r = (uint8_t)(0xff);
		in->g = (uint8_t)(0xff);
		in->b = (uint8_t)(0xff);
		in->a = (uint8_t)(0xff);
		rgb32* out = reinterpret_cast<rgb32*>(_pixels);
		for (UINT i = 0; i < _height; ++i) {
			for (UINT j = 0; j < _width; ++j) {
				out->b = in->b;
				out->g = in->g;
				out->r = in->r;
				out->a = in->a;
				++out;
			}
		}
		this->unlock_bits();
		_task_mood = _mood::UNKNOWN_;
	}
	__forceinline image_format get_format() const {
		return _format;
	}
	__forceinline void unlock_bits() {
		if (_bit_map == NULL)return;
		if (_bitmap_data == NULL)return;
		_bit_map->UnlockBits(_bitmap_data);
		::DeleteObject(_bitmap_data);
		_bitmap_data = NULL;
		if (_pixels != NULL) {
			::DeleteObject(_pixels);
			_pixels = NULL;
		}
		_task_mood = _mood::UNKNOWN_;
		return;
	}
	__forceinline void dump_data() {
		this->lock_bits(Gdiplus::ImageLockModeRead);
		rgb32* out = reinterpret_cast<rgb32*>(_pixels);
		for (UINT i = 0; i < _height; ++i) {
			for (UINT j = 0; j < _width; ++j) {
				std::cout << "rgb(" << (int)out->b << "," << (int)out->g << "," << (int)out->r << ") ";
				++out;
			}
		}
		this->unlock_bits();
	}
	__forceinline int lock_bits(Gdiplus::ImageLockMode lock_mode = Gdiplus::ImageLockModeRead) {
		if (_is_loaded == FALSE)return this->panic("Please Load an Image than try again.", TRUE);
		this->unlock_bits();
		_bitmap_data = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0, 0, _width, _height);
		// Lock a 5x3 rectangular portion of the bitmap for reading.
		_bit_map->LockBits(&rect, lock_mode, PixelFormat32bppARGB, _bitmap_data);
		if (_pixels != NULL) {
			::DeleteObject(_pixels);
			_pixels = NULL;
		}
		_pixels = reinterpret_cast<byte*>(_bitmap_data->Scan0);
		return TRUE;
	}
	__forceinline rgb32* get_pixel(INT x, INT y) const {
		if (_is_loaded == FALSE)return NULL;
		if (_pixels == NULL)return NULL;
		rgb32* pixel_color = reinterpret_cast<rgb32*>(_pixels);
		return &pixel_color[(_height - 1 - y) * _width + x];
	}
	__forceinline int set_pixel(rgb32* pixel_color, INT x, INT y) {
		if (_is_loaded == FALSE)return FALSE;
		if (_pixels == NULL)return FALSE;
		rgb32* temp = reinterpret_cast<rgb32*>(_pixels);
		memcpy(&temp[(_height - 1 - y) * _width + x], pixel_color, sizeof(rgb32));
		return TRUE;
	}
	__forceinline ULONG_PTR gdiplus_token()const {
		return _gdiplus_token;
	}
	__forceinline Gdiplus::Bitmap* get_data()const {
		Gdiplus::Rect rect(0, 0, _width, _height);
		return _bit_map->Clone(rect, PixelFormat32bppARGB);
	}
	/*Assignment operator creates a deep copy of the source image.*/
	__forceinline image& operator = (const image& other) {
		if (other.is_loaded() == FALSE)return *this;
		this->free_memory();
		this->_bit_map = other.get_data();
		_width = other.get_width(); _height = other.get_height();
		this->_gdiplus_token = other.gdiplus_token();
		_is_loaded = TRUE; _format = other.get_format();
		return *this;
	}
	__forceinline int resize(image& dest) {
		if (_is_loaded == FALSE)return this->panic("Please Load an Image than try again.", TRUE);
		this->lock_bits(Gdiplus::ImageLockModeRead);
		dest.lock_bits(Gdiplus::ImageLockModeWrite);
		INT new_width = dest.get_width(), new_height = dest.get_height();
		double scale_width = static_cast<double>(new_width) / static_cast<double>(_width);
		double scale_height = static_cast<double>(new_height) / static_cast<double>(_height);
		for (INT y = 0; y < new_height; ++y) {
			for (INT x = 0; x < new_width; ++x) {
				rgb32* pixel_color = get_pixel(static_cast<INT>(x / scale_width), static_cast<INT>(y / scale_height));
				dest.set_pixel(pixel_color, x, y);
			}
		}
		this->unlock_bits(); dest.unlock_bits();
		return TRUE;
	}
	__forceinline int resize(INT width, INT height) {
		if (_is_loaded == FALSE)return this->panic("Please Load an Image than try again.", TRUE);
		if (width % 4 != 0) {
			return panic("There is a windows-imposed requirement on BMP that the width be a multiple of 4", TRUE);
		}
		image* img = new image();
		int ret = img->create_canvas(width, height);
		if (is_error_code(ret) == TRUE) {
			panic(img->get_last_error(), ret);
			img->free_memory(); delete img;
			return ret;
		}
		ret = this->resize(*img);
		if (is_error_code(ret) != TRUE) {
			*this = *img;
		}
		img->free_memory(TRUE); delete img;
		return ret;
	}
	__forceinline int save(const char* path) {
		return this->save_as(path, _format);
	}
	__forceinline int save_as(const char* path, const image_format format) {
		if (_is_loaded == FALSE)return this->panic("Please Load an Image than try again.", TRUE);
		const char* cmime_type = get_mime_type(format);
		if (cmime_type == NULL) return this->panic("Unsupported Image format.\nSupported .bmp, .png, .jpeg, .jpg, .gif, .tiff, .tif", TRUE);
		CLSID encoder_clsid;
		wchar_t* mime_type = ccr2ws(cmime_type);
		int ret = get_encoder_clsid(const_cast<const wchar_t*>(mime_type), &encoder_clsid);
		delete[]mime_type;
		if (ret < 0)return this->panic("Unable to create mime type.", TRUE);
		wchar_t* wpath = ccr2ws(path);
		Gdiplus::Status stat;
		Gdiplus::EncoderParameters* encoder_parameters = NULL;
		if (format == image_format::JPEG || format == image_format::JPG) {
			encoder_parameters = new Gdiplus::EncoderParameters();
			encoder_parameters->Count = 1;
			encoder_parameters->Parameter[0].Value = 0;// Save the image as a JPEG with quality level 0.
			encoder_parameters->Parameter[0].Guid = Gdiplus::EncoderQuality;
			encoder_parameters->Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			encoder_parameters->Parameter[0].NumberOfValues = 1;
		}
		stat = _bit_map->Save(const_cast<const wchar_t*>(wpath), &encoder_clsid, encoder_parameters);
		delete[]wpath;
		if (encoder_parameters != NULL) {
			delete encoder_parameters; encoder_parameters = NULL;
		}
		if (stat == Gdiplus::Status::Ok)return TRUE;
		std::string err("Failed to save image. Error reason:");
		err.append(get_gdiplus_error_reason(stat));
		this->panic(err.c_str(), TRUE); err.clear(); std::string().swap(err);
		return _errc;
	}
	//10:44 AM 1/20/2020
	template<class _func>
	__forceinline int get_image_buff(_func cb, image_format format = image_format::BMP) {
		if (_is_loaded == FALSE) return this->panic("Please Load an Image than try again.", TRUE);
		if (_bit_map == NULL) return this->panic("Please Load an Image than try again.", TRUE);
		const char* cmime_type = get_mime_type(format);
		if (cmime_type == NULL) return this->panic("Unsupported Image format.\nSupported .bmp, .png, .jpeg, .jpg, .gif, .tiff, .tif", TRUE);
		IStream* oStream = NULL;
		if (::CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)& oStream) != S_OK)return this->panic("Unable to create memmory stream.", TRUE);
		this->unlock_bits();
		CLSID encoder_clsid;
		wchar_t* mime_type = ccr2ws(cmime_type);
		int ret = get_encoder_clsid(const_cast<const wchar_t*>(mime_type), &encoder_clsid);
		delete[]mime_type;
		if (ret < 0)return this->panic("Unable to create mime type.", TRUE);
		_bit_map->Save(oStream, &encoder_clsid);
		ULARGE_INTEGER ulnSize; LARGE_INTEGER lnOffset;
		lnOffset.QuadPart = 0;
		oStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize);
		oStream->Seek(lnOffset, STREAM_SEEK_SET, NULL);
		if (ulnSize.QuadPart <= 0) {
			oStream->Release();
			return this->panic("No data found in Image Stream.", TRUE);
		}
		int eof = FALSE; ULONG ulBytesRead; unsigned int total_len = (unsigned int)ulnSize.QuadPart;
		const int chunk = 1024 * 2; unsigned read_len = 0;
		do {
			char* pBuff;
			if (total_len > chunk) {
				read_len = chunk;
			}
			else {
				read_len = total_len;
			}
			pBuff = new char[read_len];
			oStream->Read(pBuff, (ULONG)read_len, &ulBytesRead);
			total_len -= ulBytesRead;
			if (total_len <= 0)eof = TRUE;
			cb(const_cast<const char*>(pBuff), static_cast<int>(ulBytesRead));
			//cb(pBuff, ulBytesRead);
			delete[]pBuff;
		} while (eof == FALSE);
		oStream->Release();
		return TRUE;
	}
	//04:13 PM 1/19/2020
	__forceinline int get_image_buff(std::string&out, image_format format = image_format::BMP) {
		if (_is_loaded == FALSE) return this->panic("Please Load an Image than try again.", TRUE);
		if (_bit_map == NULL) return this->panic("Please Load an Image than try again.", TRUE);
		const char* cmime_type = get_mime_type(format);
		if (cmime_type == NULL) return this->panic("Unsupported Image format.\nSupported .bmp, .png, .jpeg, .jpg, .gif, .tiff, .tif", TRUE);
		IStream* oStream = NULL;
		if (::CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)& oStream) != S_OK)return this->panic("Unable to create memmory stream.", TRUE);
		this->unlock_bits();
		CLSID encoder_clsid;
		wchar_t* mime_type = ccr2ws(cmime_type);
		int ret = get_encoder_clsid(const_cast<const wchar_t*>(mime_type), &encoder_clsid);
		delete[]mime_type;
		if (ret < 0)return this->panic("Unable to create mime type.", TRUE);
		_bit_map->Save(oStream, &encoder_clsid);
		ULARGE_INTEGER ulnSize; LARGE_INTEGER lnOffset;
		lnOffset.QuadPart = 0;
		oStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize);
		oStream->Seek(lnOffset, STREAM_SEEK_SET, NULL);
		if (ulnSize.QuadPart <= 0) {
			oStream->Release();
			return this->panic("No data found in Image Stream.", TRUE);
		}
		char* pBuff = new char[(unsigned int)ulnSize.QuadPart];
		ULONG ulBytesRead;
		oStream->Read(pBuff, (ULONG)ulnSize.QuadPart, &ulBytesRead);
		oStream->Release();
		out = std::string(pBuff, ulBytesRead);
		delete[]pBuff; pBuff = NULL;
		return TRUE;
	}
	__forceinline int to_base64(std::string& out, image_format format = image_format::BMP) {
		std::string* data = new std::string();
		int ret = this->get_image_buff(*data, format);
		if (is_error_code(ret) == FALSE) {
			ret = sow_web_jsx::base64::to_encode_str(*data, out) == true ? TRUE : FALSE;
		}
		data->clear(); delete data;
		return ret;
	}
	__forceinline UINT get_width() const {
		return _width;
	}
	__forceinline UINT get_height() const {
		return _height;
	}
	__forceinline const char* get_last_error() const {
		if (_errc >= 0 || _internal_error == NULL) return "No Error Found!!!";
		return const_cast<const char*>(_internal_error);
	}
	__forceinline int has_error() const {
		return _errc == TRUE || _errc < 0 ? TRUE : FALSE;
	}
	__forceinline int is_loaded() const {
		return _is_loaded;
	}
	__forceinline void free_memory(int is_copy = FALSE) {
		_is_copy = is_copy;
		if (_bitmap_data != NULL) {
			::DeleteObject(_bitmap_data); _bitmap_data = NULL;
		}
		if (_internal_error != NULL) {
			delete[]_internal_error; _internal_error = NULL;
		}
		if (_pixels != NULL) {
			::DeleteObject(_pixels); _pixels = NULL;
		}
		if (_bit_map != NULL) {
			delete _bit_map; _bit_map = NULL;
		}
		_height = 0; _width = 0;
		_errc = FALSE; _is_loaded = FALSE;
		_format = image_format::UNKNOWN;
		return;
	}
};
void image_export(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> ctx){
	v8::Local<v8::FunctionTemplate> image_tpl = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall()) {
			throw_js_error(isolate, "Cannot call constructor as function!!!");
			return;
		}
		image* img = new image();
		v8::Local<v8::Object> obj = args.This();
		obj->SetInternalField(0, v8::External::New(isolate, img));
		v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> pobj(isolate, obj);
		pobj.SetWeak<image*>(&img, [](const v8::WeakCallbackInfo<image*>& data) {
			delete[] data.GetParameter();
		}, v8::WeakCallbackType::kParameter);
	});
	image_tpl->SetClassName(v8_str(isolate, "Image"));
	image_tpl->InstanceTemplate()->SetInternalFieldCount(1);
	v8::Local<v8::ObjectTemplate> prototype = image_tpl->PrototypeTemplate();
	prototype->Set(v8::String::NewFromUtf8(isolate, "load", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "File Path Required required....");
			return;
		}
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(isolate, "Image object disposed...");
			return;
		}
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abspath = new std::string(utf_abs_path_str.c_str());
		format__path(*abspath);
		int ret = img->load(abspath->c_str());
		utf_abs_path_str.clear(); abspath->clear(); delete abspath;
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "release", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL)return;
		img->release_all();
		delete img; img = NULL;
		args.Holder()->SetAlignedPointerInInternalField(0, nullptr);
	}));
	prototype->Set(isolate, "reset", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		img->reset_rgb();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "release_mem", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		img->release_all();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "get_data", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		image_format format = image_format::BMP;//get_image_format(*abspath);
		if (args.Length() > 0) {
			if (args[0]->IsNumber()) {
				v8::Local<v8::Context>ctx = args.GetIsolate()->GetCurrentContext();
				format = (image_format)args[0]->Int32Value(ctx).FromMaybe(0);
			}
		}
		std::string* out = new std::string();
		int ret = img->get_image_buff(*out, format);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), img->get_last_error());
		}
		else {
			args.GetReturnValue().Set(v8_str(args.GetIsolate(), out->c_str()));
		}
		out->clear(); delete out;
	}));
	prototype->Set(isolate, "get_datac", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!args[0]->IsFunction()) {
			throw_js_error(args.GetIsolate(), "Callback required...");
			return;
		}
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		image_format format = image_format::BMP;//get_image_format(*abspath);
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
				format = (image_format)args[1]->Int32Value(ctx).FromMaybe(0);
			}
		}
		v8::Persistent<v8::Function> cb;
		cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
		v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
		int ret = img->get_image_buff([&isolate, &callback](const char* buff, int len) {
			//v8_str(isolate, buff);
			v8::Local<v8::Value> argv[2] = {
				v8::String::NewFromUtf8(isolate, buff,v8::NewStringType::kNormal, len).ToLocalChecked(),
				v8::Integer::New(isolate, len)
			};
			callback->Call(isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(), 2, argv);
			argv->Clear();
		}, format);
		callback.Clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), img->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "to_base64", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		image_format format = image_format::BMP;//get_image_format(*abspath);
		if (args.Length() > 0) {
			if (args[0]->IsNumber()) {
				v8::Local<v8::Context>ctx = args.GetIsolate()->GetCurrentContext();
				format = (image_format)args[0]->Int32Value(ctx).FromMaybe(0);
			}
		}
		std::string* out = new std::string();
		int ret = img->to_base64(*out, format);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), img->get_last_error());
		}
		else {
			args.GetReturnValue().Set(v8_str(args.GetIsolate(), out->c_str()));
		}
		
		out->clear(); delete out;
	}));
	prototype->Set(isolate, "load_from_data", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "base64 Data required....");
			return;
		}
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		image_format format = image_format::BMP;
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = args.GetIsolate()->GetCurrentContext();
				format = (image_format)args[1]->Int32Value(ctx).FromMaybe(0);
			}
		}
		native_string utf_base64(isolate, args[0]);
		std::string* img_data = new std::string(utf_base64.c_str(), utf_base64.size());
		int ret = img->load_from_buff(img_data->data(), static_cast<ULONG>(img_data->size()), format);
		utf_base64.clear(); img_data->clear(); delete img_data;
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "load_from_base64", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		if (!args[0]->IsString()) {
			throw_js_error(isolate, "base64 Data required....");
			return;
		}
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		image_format format = image_format::BMP;//get_image_format(*abspath);
		if (args.Length() > 1) {
			if (args[1]->IsNumber()) {
				v8::Local<v8::Context>ctx = args.GetIsolate()->GetCurrentContext();
				format = (image_format)args[1]->Int32Value(ctx).FromMaybe(0);
			}
		}
		native_string utf_base64(isolate, args[0]);
		int ret = img->load_from_base64(utf_base64.c_str(), format);
		utf_base64.clear();
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "save", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (!args[0]->IsString()) {
			throw_js_error(args.GetIsolate(), "File Path required....");
			return;
		}
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		native_string utf_abs_path_str(isolate, args[0]);
		std::string* abspath = new std::string(utf_abs_path_str.c_str());
		format__path(*abspath);
		image_format format = get_image_format(*abspath);
		if (format == image_format::UNKNOWN || format == image_format::NO_EXT) {
			utf_abs_path_str.clear(); abspath->clear(); delete abspath;
			throw_js_error(args.GetIsolate(), "Unsupported Image format defined...");
			return;
		}
		int ret = TRUE;
		ret = img->save_as(abspath->c_str(), format);
		utf_abs_path_str.clear(); abspath->clear(); delete abspath;
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
			return;
		}
		args.GetReturnValue().Set(v8::Integer::New(isolate, ret));
	}));
	prototype->Set(isolate, "get_width", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		UINT width = img->get_width();
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<int>(width)));
	}));
	prototype->Set(isolate, "get_height", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		UINT height = img->get_height();
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<int>(height)));
	}));
	prototype->Set(isolate, "resize", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(args.GetIsolate(), "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "Width required...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = img->resize(width, height);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "create_canvas", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (args.Length() < 2) {
			throw_js_error(args.GetIsolate(), "Height and Width required...");
			return;
		}
		if (!args[0]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "Height required...");
			return;
		}
		if (!args[1]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "Width required...");
			return;
		}
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		int width = args[0]->Int32Value(ctx).FromMaybe(0);
		int height = args[1]->Int32Value(ctx).FromMaybe(0);
		int ret = img->create_canvas(width, height);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, img->get_last_error());
		}
		else {
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "lock_bits", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		_mood mood = _mood::READ;
		if (args.Length() >= 1) {
			if (args[0]->IsNumber()) {
				v8::Local<v8::Context>ctx = args.GetIsolate()->GetCurrentContext();
				mood = (_mood)args[0]->ToInteger(ctx).ToLocalChecked()->Value();
			}
		}
		int ret = img->lock_bits(mood == _mood::READ ? Gdiplus::ImageLockModeRead : Gdiplus::ImageLockModeWrite);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(args.GetIsolate(), img->get_last_error());
			return;
		}
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "get_pixel", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
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
		rgb32* pixel = img->get_pixel(x, y);
		if (pixel == NULL) {
			throw_js_error(args.GetIsolate(), "Unable to read pixel from image...");
			return;
		}
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		v8_result->Set(ctx, v8_str(isolate, "r"), v8::Integer::New(isolate, static_cast<int>(pixel->r)));
		v8_result->Set(ctx, v8_str(isolate, "g"), v8::Integer::New(isolate, static_cast<int>(pixel->g)));
		v8_result->Set(ctx, v8_str(isolate, "b"), v8::Integer::New(isolate, static_cast<int>(pixel->b)));
		v8_result->Set(ctx, v8_str(isolate, "a"), v8::Integer::New(isolate, static_cast<int>(pixel->a)));
		args.GetReturnValue().Set(v8_result); v8_result.Clear();
	}));
	prototype->Set(isolate, "set_pixel", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		if (args.Length() < 3) {
			throw_js_error(args.GetIsolate(), "rgba and x y required...");
			return;
		}
		if (!args[0]->IsObject()) {
			throw_js_error(args.GetIsolate(), "rgba should be object...");
			return;
		}
		if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
			throw_js_error(args.GetIsolate(), "x y should be number...");
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
		int ret = img->set_pixel(pixel, x, y);
		if (is_error_code(ret) == TRUE) {
			throw_js_error(isolate, "Unable to set your request pixel...");
		}
		else {
			//std::cout << "rgb(" << (int)pixel->b << "," << (int)pixel->g << "," << (int)pixel->r << ") " << std::endl;
			args.GetReturnValue().Set(args.Holder());
		}
	}));
	prototype->Set(isolate, "unlock_bits", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		img->unlock_bits();
		args.GetReturnValue().Set(args.Holder());
	}));
	prototype->Set(isolate, "dump_data", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		image* img = sow_web_jsx::unwrap<image>(args);
		if (img == NULL) {
			throw_js_error(args.GetIsolate(), "Image object disposed...");
			return;
		}
		if (img->is_loaded() == FALSE) {
			throw_js_error(args.GetIsolate(), "Image does not loaded yet...");
			return;
		}
		img->dump_data();
		args.GetReturnValue().Set(args.Holder());
	}));
	{
		v8::Local<v8::ObjectTemplate>image_format_object = v8::ObjectTemplate::New(isolate);
		image_format_object->Set(isolate, "BMP", v8::Integer::New(isolate, (int)image_format::BMP));
		image_format_object->Set(isolate, "PNG", v8::Integer::New(isolate, (int)image_format::PNG));
		image_format_object->Set(isolate, "JPEG", v8::Integer::New(isolate, (int)image_format::JPEG));
		image_format_object->Set(isolate, "JPG", v8::Integer::New(isolate, (int)image_format::JPG));
		image_format_object->Set(isolate, "GIF", v8::Integer::New(isolate, (int)image_format::GIF));
		image_format_object->Set(isolate, "TIFF", v8::Integer::New(isolate, (int)image_format::TIFF));
		image_format_object->Set(isolate, "TIF", v8::Integer::New(isolate, (int)image_format::TIF));
		ctx->Set(isolate, "image_format", image_format_object);
		v8::Local<v8::ObjectTemplate>image_mood = v8::ObjectTemplate::New(isolate);
		image_mood->Set(isolate, "READ", v8::Integer::New(isolate, (int)_mood::READ));
		image_mood->Set(isolate, "WRITE", v8::Integer::New(isolate, (int)_mood::WRITE));
		ctx->Set(isolate, "image_mood", image_mood);
	}
	ctx->Set(isolate, "Image", image_tpl);
}

#endif//!_WIN32||_WIN64
//End 3:56 PM 1/18/2020
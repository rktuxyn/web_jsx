//2:17 AM 11/27/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_pdf_generator_h)
#	define _pdf_generator_h
#	include "web_jsx_global.h"
#	include <wkhtmltox/pdf.h>
#	include <vector>
#	include <map>
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)

#define WKHTMLTOPDF_SETTING_CTOR(s)    { sizeof(s)-1}
namespace pdf_ext {
	/*[function pointers]*/
	typedef void (*pdf_ext_func)(wkhtmltopdf_converter*, int);
	typedef void (*pdf_ext_change_func)(wkhtmltopdf_converter*);
	typedef void (*pdf_ext_ot_func)(wkhtmltopdf_converter*, const char*);
	/*[/function pointers]*/
	class pdf_generator {
	private:
		bool _disposed;
		char*_msg;
		int _status;
		wkhtmltopdf_global_settings* _wgs;
		wkhtmltopdf_object_settings* _wos;
		wkhtmltopdf_converter*_converter;
		virtual void set_status(int ret_val, const char* ret_msg);
		std::map<const char*, const char*>*_wgs_settings;
		int _prepared_wgs;
		std::map<const char*, const char*>*_wos_settings;
		int _prepared_wos;
		virtual void init_func();
		virtual int init_wgs();
		virtual int init_wos();
		virtual void prepare_default_settings();
		virtual int validate_settings(std::vector<const char*>&key, std::map<const char*, const char*>&settings);
		virtual int update_map_key(std::map<const char*, const char*>&header, const char*key, const char*values);
	public:
		pdf_ext_func finished;
		pdf_ext_func progress_changed;
		pdf_ext_change_func phase_changed;
		pdf_ext_ot_func error;
		pdf_ext_ot_func warning;
		const char* version;
		pdf_generator();
		virtual ~pdf_generator();
		virtual const char* get_status_msg();
		virtual int init(int use_graphics);
		virtual int init(int use_graphics, std::map<const char*, const char*>&wgs_settings, std::map<const char*, const char*>&wos_settings);
		virtual long generate(const char*html, std::string& str_output);
		virtual int generate_to_path(const char*html, const char* output_path);
		virtual int generate(const char* url, const char* output_path);
		virtual int generate(const std::string html, const char* output_path);
		virtual int generate_from_url(const char*url, std::string& str_output);
		virtual int generate_from_url(const char*url, const char* output_path);
		virtual void global_settings(std::map<const char*, const char*>&settings);
		virtual void object_settings(std::map<const char*, const char*>&settings);
		virtual void dispose();
	};
}
#endif//!_parameter_direction_h
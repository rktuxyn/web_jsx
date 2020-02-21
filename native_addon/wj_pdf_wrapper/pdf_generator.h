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
#	include <wkhtmltox/pdf.h>
#	include <vector>
#	include <map>
#	include <string>

#pragma warning (disable : 4231)
#pragma warning(disable : 4996)

namespace pdf_ext {
	/*[function pointers]*/
	typedef void (*pdf_ext_func)(wkhtmltopdf_converter*, int);
	typedef void (*pdf_ext_change_func)(wkhtmltopdf_converter*);
	typedef void (*pdf_ext_ot_func)(wkhtmltopdf_converter*, const char*);
	/*[/function pointers]*/
	class pdf_generator {
	private:
		int _disposed;
		char*_msg;
		int _status;
		wkhtmltopdf_global_settings* _wgs;
		wkhtmltopdf_object_settings* _wos;
		wkhtmltopdf_converter*_converter;
		virtual void set_status(int ret_val, const char* ret_msg);
		std::map<std::string, std::string>*_wgs_settings;
		int _prepared_wgs;
		std::map<std::string, std::string>*_wos_settings;
		int _prepared_wos;
		void init_func();
		int init_wgs();
		int init_wos();
		void prepare_default_settings();
		int update_map_key(
			std::map<std::string, std::string>&header,
			std::string key, std::string values
		);
	public:
		pdf_ext_func finished;
		pdf_ext_func progress_changed;
		pdf_ext_change_func phase_changed;
		pdf_ext_ot_func error;
		pdf_ext_ot_func warning;
		const char* version;
		pdf_generator();
		pdf_generator(const pdf_generator&) = delete;
		pdf_generator& operator=(const pdf_generator&) = delete;
		~pdf_generator();
		const char* get_status_msg();
		int init(int use_graphics);
		int init(
			int use_graphics, 
			std::map<std::string, std::string>&wgs_settings,
			std::map<std::string, std::string>&wos_settings
		);
		long generate(const char*html, std::string& str_output);
		int generate_to_path(const char*html, const char* output_path);
		int generate_from_url(const char*url, std::string& str_output);
		int generate_from_url(const char*url, const char* output_path);
		void dispose();
	};
}
#endif//!_parameter_direction_h
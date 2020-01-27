/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(FAST_CGI_APP)
#	include "web_jsx_fcgi.h"
#endif//FAST_CGI_APP
#	include "web_jsx_cgi.h"
//#if !defined(_bitmap_h)
//#include "core/bitmap.h"
//#endif//!_bitmap_h
//void resize_image() {
//	bitmap* bmp = new bitmap();
//	//bmp->load("C:\\Users\\Administrator\\Desktop\\ksl\\white.bmp");
//	bmp->create_canvas(500, 236);
//	//bmp->dump_data();
//	bmp->save("C:\\Users\\Administrator\\Desktop\\ksl\\test_thumb.bmp");
//	/*auto pixel = bmp->get_pixel(10, 10);
//	pixel->a;*/
//	bmp->free_memory();
//	delete bmp;
//}
int main(int argc, char *argv[], char*envp[]) {
	std::ios::sync_with_stdio(false);
	if (is_user_interactive() == TRUE) {
		//resize_image();
		if (argc > 1) {
			try {
				web_jsx_cgi::app_core::prepare_console_response(argc, argv, false);
			} catch (std::exception&e) {
				std::cout << "\r\n";
				std::cout << e.what() << "\r\n";
			}
			return EXIT_SUCCESS;
		}
		print_info();
		return EXIT_SUCCESS;
	}
	if (argc > 1) {
		const char*interactive_req = const_cast<const char*>(argv[1]);
		if (strcmp(interactive_req, "I_REQ") == 0) {
			web_jsx_cgi::app_core::prepare_console_response(argc, argv, true);
			return EXIT_SUCCESS;
		}
	}
	//12:28 AM 1/28/2019
#if defined(FAST_CGI_APP)
	return web_jsx_cgi::fcgi_request::request_handler(const_cast<const char*>(argv[0]));
#else
	return web_jsx_cgi::cgi_request::request_handler(const_cast<const char*>(argv[0]));
#endif//!FAST_CGI_APP
}
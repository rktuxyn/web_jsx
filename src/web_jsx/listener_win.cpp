#include "listener_win.h"
#	include "core/web_jsx_global.h"
#	include "web_jsx_fcgi.h"
#	include "fcreqh.h"
#	include "core/v8_engine.h"
#	include "core/glb_r.h"
#	include <fastcgi.h>
#	include <fcgi_stdio.h>
#	include <fcgio.h>
#	include "web_jsx_cgi.h"
#	include "core/native_wrapper.h"
#	include "core/wjsx_env.h"
#	include <thread>
#	include <mutex>
int create_nonblocking_threads(const char* execute_path, int is_fserver){
	return 0;
}

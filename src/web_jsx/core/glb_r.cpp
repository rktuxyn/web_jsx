/**
* MIT License
*
* Copyright (c) 2022 FSys Tech Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#	include "glb_r.h"

#if !defined(NULL)
#ifdef __cplusplus
#	define NULL 0
#else
#	define NULL ((void *)0)
#endif
#endif//!__cplusplus

typedef struct RESOURCE_CTX{
	RESOURCE_CTX* next;
	void* data;
}resource_ctx;

resource_ctx* _RESOURCE_CTX;

void fsys_web_jsx::free_resource() {
	if (_RESOURCE_CTX == NULL)return;
	while (_RESOURCE_CTX) {
		resource_ctx* ctx = _RESOURCE_CTX;
		_RESOURCE_CTX = _RESOURCE_CTX->next;
		((add_resource_func)ctx->data)();
		delete ctx;
	}
	if (_RESOURCE_CTX != NULL)
		delete _RESOURCE_CTX;
	_RESOURCE_CTX = NULL;
}
void fsys_web_jsx::register_resource(add_resource_func func) {
	if (func == NULL)return;
	resource_ctx* ctx = new resource_ctx();
	ctx->data = func;
	ctx->next = _RESOURCE_CTX;
	_RESOURCE_CTX = ctx;
}

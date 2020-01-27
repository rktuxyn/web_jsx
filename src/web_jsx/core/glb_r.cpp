/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "glb_r.h"
#	include <vector>
typedef struct RESOURCE_CTX{
	RESOURCE_CTX* next;
	void* data;
}resource_ctx;
//std::vector<void*> _resource;
resource_ctx* _RESOURCE_CTX;
void sow_web_jsx::free_resource() {
	/*if (_resource.empty() || _resource.size() <= 0)return;
	for (size_t i = 0; i < _resource.size(); ++i) {
		((add_resource_func)_resource[i])();
	}
	std::vector<void*>().swap(_resource);*/
	if (_RESOURCE_CTX == NULL)return;
	resource_ctx* ctx;
	while (_RESOURCE_CTX) {
		ctx = _RESOURCE_CTX;
		_RESOURCE_CTX = _RESOURCE_CTX->next;
		delete ctx;
	}
	if (_RESOURCE_CTX != NULL)
		delete _RESOURCE_CTX;
	_RESOURCE_CTX = NULL;
}
void sow_web_jsx::register_resource(add_resource_func func) {
	resource_ctx* ctx = new resource_ctx();
	ctx->data = func;
	ctx->next = _RESOURCE_CTX;
	_RESOURCE_CTX = ctx;
	//_resource.push_back(func);
}

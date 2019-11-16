#include "glb_r.h"
std::vector<void*> _resource;
jsx_export void sow_web_jsx::free_resource() {
	if (_resource.empty() || _resource.size() <= 0)return;
	for (size_t i = 0; i < _resource.size(); ++i) {
		((add_resource_func)_resource[i])();
	}
	std::vector<void*>().swap(_resource);
}
jsx_export void sow_web_jsx::register_resource(add_resource_func func) {
	_resource.push_back(func);
}

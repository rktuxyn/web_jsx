/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//7:06 PM 2/21/2020
#	include "wjsx_env.h"
#	include <iterator>

//throw command-line interface not allowed function
#define IF_CLI_THROW_ERROR			\
if (is_cli() == TRUE)				\
FATAL("CLI MODE NOT POSSIBLE...")	\

std::string* concat_str(const char* a, const char* b) {
	size_t length = strlen(a) + strlen(b);
	char* new_str = new char[length + sizeof(char)];
	new_str[length] = '\0';
	sprintf_s(new_str, length, "%s%s", a, b);
	std::string* out_str = new std::string(new_str, length);
	//const char* cnew_str = *&new_str;
	delete[] new_str;
	return out_str;
}
wjsx_env* unwrap_wjsx_env(v8::Isolate* isolate) {
	void* data = isolate->GetData(isolate->GetNumberOfDataSlots() - 1);
	if (data == NULL) {
		isolate->ThrowException(v8::Exception::Error(v8_str(isolate, "Unable to load native environment...")));
		return NULL;
	}
	return reinterpret_cast<wjsx_env*>(data);
}
void wrap_wjsx_env(v8::Isolate* isolate, wjsx_env* wj_env){
	isolate->SetData(isolate->GetNumberOfDataSlots() - 1, (void*)wj_env);
	return;
}
wjsx_env::wjsx_env(int interactive_mode, int is_thread_req){
	_is_interactive = interactive_mode;
	_is_flush = FALSE; _is_disposed = FALSE; _is_thread_req = is_thread_req;
	compiled_cached = TRUE; check_file_state = TRUE; _is_script = FALSE;
	_cin = NULL; _cout = NULL; _cerr = NULL; _is_runtime_error = FALSE;
	_body = NULL; _headers = NULL; _cookies = NULL;
	_http_status = NULL; _native_data = NULL; _is_cli = TRUE;
	_root_dir = NULL; _app_dir = NULL; _my_thread_id = _app_thread_id = 0;
	_on_async_complete = NULL; _user_data = NULL; _total_processed = 1;
}
void wjsx_env::set_native_data_structure(native_data_structure& nds){
	_native_data = &nds;
}
int wjsx_env::has_native_data_structure(){
	return _native_data == NULL ? FALSE : TRUE;
}
void wjsx_env::free_native_data_structure() {
	if (has_native_data_structure() == TRUE) {
		_free_obj(_native_data);
	}
}
native_data_structure* wjsx_env::get_native_data_structure(int create){
	if (create == TRUE && has_native_data_structure() == FALSE) {
		_native_data = new native_data_structure;
	}
	return _native_data;
}
std::shared_ptr<std::mutex> wjsx_env::get_mutex() {
	IF_CLI_THROW_ERROR;
	return _native_data->get_mutex();
}
void wjsx_env::set_user_data(void* data){
	IF_CLI_THROW_ERROR;
	_user_data = data;
}
void wjsx_env::set_on_async_complete(on_async_complete callback){
	IF_CLI_THROW_ERROR;
	_on_async_complete = callback;
}
void wjsx_env::on_task_complete(v8::Isolate* isolate) {
	IF_CLI_THROW_ERROR;
	if (_on_async_complete == NULL)return;
	_on_async_complete(this, isolate);
}

int wjsx_env::get_my_thread_id() const{
	IF_CLI_THROW_ERROR;
	return _my_thread_id == 0 ? _app_thread_id : _my_thread_id;
}
int wjsx_env::get_app_thread_id() const{
	return _app_thread_id == 0 ? _my_thread_id : _app_thread_id;
}
int wjsx_env::is_interactive() const{
	return _is_interactive;
}
int wjsx_env::is_thread_req() const{
	//IF_CLI_THROW_ERROR;
	return _is_thread_req;
}
void wjsx_env::set_total_handled_req(int processed){
	IF_CLI_THROW_ERROR;
	_total_processed = processed + 1;
}
int wjsx_env::get_total_handled_req() const{
	IF_CLI_THROW_ERROR;
	return _total_processed;
}
int wjsx_env::is_cli()const {
	return  _is_thread_req == TRUE ? FALSE : _is_cli;
}
std::ostream& wjsx_env::cout(){
	IF_CLI_THROW_ERROR;
	return *_cout;
}
int wjsx_env::is_available_out_stream() {
	IF_CLI_THROW_ERROR;
	if (_is_disposed == TRUE)return FALSE;
	if (_cout == NULL)return FALSE;
	return stream_is_available(*_cout);
}
int wjsx_env::is_available_in_stream() {
	IF_CLI_THROW_ERROR;
	if (_is_disposed == TRUE)return FALSE;
	if (_cin == NULL)return FALSE;
	return stream_is_available(*_cin);
}
void wjsx_env::set_runtime_error(int is_error){
	IF_CLI_THROW_ERROR;
	_is_runtime_error = is_error > 0 ? TRUE : FALSE;
}
int wjsx_env::is_runtime_error() const{
	IF_CLI_THROW_ERROR;
	return _is_runtime_error;
}
void wjsx_env::set_mood_script(){
	IF_CLI_THROW_ERROR;
	_is_script = TRUE;
}
int wjsx_env::is_script_mood() const{
	IF_CLI_THROW_ERROR;
	return _is_script;
}
void wjsx_env::set_root_dir(const char* rdir){
	_free_obj(_root_dir);
	_root_dir = new std::string(rdir);
}
const char* wjsx_env::get_root_dir(){
	return _root_dir == NULL ? NULL : _root_dir->c_str();
}
void wjsx_env::set_app_dir(const char* adir){
	_free_obj(_app_dir);
	_app_dir = new std::string(adir);
}
const char* wjsx_env::get_app_dir(){
	return _app_dir == NULL ? NULL : _app_dir->c_str();
}
std::istream& wjsx_env::cin(){
	IF_CLI_THROW_ERROR;
	return *_cin;
}
std::ostream& wjsx_env::cerr(){
	IF_CLI_THROW_ERROR;
	return *_cerr;
}
std::stringstream& wjsx_env::body(){
	IF_CLI_THROW_ERROR;
	return *_body;
}
size_t wjsx_env::write_b(const char* data){
	IF_CLI_THROW_ERROR;
	if (data == NULL)return FALSE;
	size_t len = strlen(data);
	if (len == 0 || len == std::string::npos)return FALSE;
	_body->write(data, len);
	return len;
}
void wjsx_env::clear_body(int create_new) {
	IF_CLI_THROW_ERROR;
	if (_body == NULL) return;
	if (::get_stream_length(*_body) == 0)return;
	_free_obj(_body);
	if (create_new == TRUE)
		_body = new std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary); _body = _body = new std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
}
int wjsx_env::flush(){
	IF_CLI_THROW_ERROR;
	if (_is_flush == TRUE)return FALSE;
	/*if (this->is_available_out_stream() == FALSE) {
		_body->clear(); 
		return FALSE;
	}*/
	std::copy(std::istreambuf_iterator<char>(this->body()),
		std::istreambuf_iterator<char>(),
		std::ostream_iterator<char>(this->cout())
	);
	_body->clear();
	_is_flush = TRUE;
	return TRUE;
}
int wjsx_env::is_flush()const{
	IF_CLI_THROW_ERROR;
	return _is_flush;
}
void wjsx_env::set_flush(){
	IF_CLI_THROW_ERROR;
	_is_flush = TRUE;
}
std::map<std::string, std::string>& wjsx_env::get_http_header(){
	IF_CLI_THROW_ERROR;
	return *_headers;
}
std::vector<std::string>& wjsx_env::get_http_cookies(){
	IF_CLI_THROW_ERROR;
	return *_cookies;
}
std::vector<std::string>& wjsx_env::get_http_status(){
	IF_CLI_THROW_ERROR;
	return *_http_status;
}
void wjsx_env::clear(){
	if (_is_disposed == TRUE)return;
	//if (_is_cli == TRUE)return;
	_is_disposed = TRUE; _is_flush = FALSE;
	if (_is_cli == FALSE) {
		_free_obj(_cout); _free_obj(_cin);
		_free_obj(_cerr);
	}
	_free_obj(_body); _free_obj(_headers);
	_free_obj(_cookies); _free_obj(_http_status);
	_free_obj(_root_dir); _free_obj(_app_dir);
	_cout = NULL; _cin = NULL; _cerr = NULL; _native_data = NULL;
}
wjsx_env::~wjsx_env(){
	this->clear();
}
//template<class Function, class... Args>
//void async_wrapper(Function&& f, Args&&... args, std::future<void>& future,
//	std::future<void>&& is_valid, std::promise<void>&& is_moved) {
//	is_valid.wait(); // Wait until the return value of std::async is written to "future"
//	auto our_future = std::move(future); // Move "future" to a local variable
//	is_moved.set_value(); // Only now we can leave void_async in the main thread
//
//	// This is also used by std::async so that member function pointers work transparently
//	auto functor = std::bind(f, std::forward<Args>(args)...);
//	functor();
//}
//template<class Function, class... Args> // This is what you call instead of std::async
//void void_async(Function&& f, Args&&... args) {
//	std::future<void> future; // This is for std::async return value
//	// This is for our synchronization of moving "future" between threads
//	std::promise<void> valid;
//	std::promise<void> is_moved;
//	auto valid_future = valid.get_future();
//	auto moved_future = is_moved.get_future();
//	//std::reference_wrapper<std::future<void>> zz = std::ref(future);
//	// Here we pass "future" as a reference, so that async_wrapper
//	// can later work with std::async's return value
//	future = std::async(
//		async_wrapper<Function, Args...>,
//		std::forward<Function>(f), std::forward<Args>(args)...,
//		std::ref(future), std::move(valid_future), std::move(is_moved)
//	);
//	valid.set_value(); // Unblock async_wrapper waiting for "future" to become valid
//	moved_future.wait(); // Wait for "future" to actually be moved
//}
#	include <filesystem>
namespace fs = std::filesystem;
int file_has_changed(
	const char* a, const char* b
) {
	fs::path* file_a = new fs::path(a);
	fs::path* file_b = new fs::path(b);
	int ret =  fs::last_write_time(*file_a) > fs::last_write_time(*file_b) ? TRUE : FALSE;
	_free_obj(file_a); _free_obj(file_b);
	return ret;
}
int load_file_to_vct(
	std::ifstream& file_stream,
	std::vector<char>& buffer
) {
	size_t total_len = get_stream_length(file_stream);
	if (total_len == std::string::npos || total_len == 0)return FALSE;
	if (file_stream.flags() != std::ios_base::binary)
		file_stream.setf(std::ios_base::binary);
	buffer.reserve(total_len);
	do {
		if (not file_stream.good())break;
		size_t read_len = total_len > READ_CHUNK ? READ_CHUNK : total_len;
		char* in = new char[read_len];
		file_stream.read(in, read_len);
		total_len -= read_len;
		buffer.insert(buffer.end(), in, read_len + in);
		/* Free memory */
		delete[]in;
		if (total_len == 0 || total_len == std::string::npos) break;
	} while (true);
	return TRUE;
}

int read_file(
	const char* path, std::stringstream&stream
) {
	_NEW_STR(error);
	std::ifstream* file = ::create_file_stream<std::ifstream>(path, *error);
	if (file == NULL) {
		stream << error->c_str();
		_free_obj(error);
		return FALSE;
	}
	_free_obj(error);
	std::vector<char>* dest = new std::vector<char>();
	int ret = ::load_file_to_vct(*file, *dest);
	file->close(); delete file;
	if (ret == TRUE) {
		stream.write(dest->data(), dest->size());
	}
	_free_obj(dest);
	return ret;
}
int read_file(
	const char* path, std::string& out_put
) {
	std::ifstream* file = ::create_file_stream<std::ifstream>(path, out_put);
	if (file == NULL) {
		return FALSE;
	}
	std::vector<char>* dest = new std::vector<char>();
	int ret = ::load_file_to_vct(*file, *dest);
	file->close(); delete file;
	if (ret == TRUE) {
		out_put = std::string(dest->data(), dest->size());
	}
	_free_obj(dest);
	return ret;
}
int write_file(
	const char* path, const char* data, size_t len
) {
	_NEW_STR(error);
	std::ofstream* file = ::create_file_stream<std::ofstream>(path, *error);
	_free_obj(error);
	if (file == NULL)return FALSE;
	file->write(data, len);
	file->flush(); file->close(); delete file;
	return TRUE;
}
//const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
//	v8::String::Utf8Value str(isolate, x);
//	return *str ? *str : "<string conversion failed>";
//}
void v8_object_extend_internal(
	v8::Isolate* isolate, v8::Local<v8::Context> ctx,
	const v8::Handle<v8::Object> source,
	v8::Local<v8::Object> dest
) {
	v8::Local<v8::Array> property_names = source->GetOwnPropertyNames(ctx).ToLocalChecked();
	uint32_t length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		v8::Local<v8::Value> key = property_names->Get(ctx, i).ToLocalChecked();
		if (!key->IsString())continue;
		//std::cout << to_char_str(isolate, key) << std::endl;
		v8::Local<v8::Value> value = source->Get(ctx, key).ToLocalChecked();
		if (value->IsNullOrUndefined())continue;
		if (value->IsObject() && !value->IsFunction()) {
			v8::Local<v8::Value> next = dest->Get(ctx, key).ToLocalChecked();
			if (!next->IsNullOrUndefined() && next->IsObject() && !next->IsFunction()) {
				//deep extend here...
				v8_object_extend_internal(isolate, ctx, v8::Local<v8::Object>::Cast(value), v8::Local<v8::Object>::Cast(next));
				continue;
			}
		}
		dest->Set(ctx, key, value);
	}
}

#if !defined(_store_module)
#	define _store_module(module_obj, wj_module)		\
if( module_obj == NULL ){							\
	module_obj = new wj_native_modules;				\
	module_obj->wj_module = wj_module;				\
	module_obj->next = NULL;						\
} else {											\
	wj_native_modules* anm = new wj_native_modules;	\
	anm->wj_module = wj_module;						\
	anm->next = module_obj;							\
	module_obj = anm;								\
}
#endif//!_store_module

native_data_structure::native_data_structure(){
	_lib_data			= new std::map<std::string, typeof_native_obj>();
	_request_data		= new std::map<std::string, typeof_native_obj>();
	_is_disposed		= FALSE;
	_working_modules	= NULL;
	_native_modules		= NULL;
	_mutex				= NULL;
}
void native_data_structure::set_mutex(std::shared_ptr<std::mutex> mutex){
	_mutex = mutex;
}
std::shared_ptr<std::mutex> native_data_structure::get_mutex(){
	if (this->_mutex == NULL)FATAL("std::mutex should not NULL...");
	return _mutex;
}
int native_data_structure::store_native_module(void* wj_module) {
	if (wj_module == NULL)return FALSE;
	_store_module(_native_modules, wj_module);
	return TRUE;
}
wj_native_modules* native_data_structure::get_native_module(){
	return _native_modules;
}
int native_data_structure::store_working_module(void* wj_module){
	if (wj_module == NULL)return FALSE;
	_store_module(_working_modules, wj_module);
	return TRUE;
}
wj_native_modules* native_data_structure::get_working_module(){
	return _working_modules;
}
std::map<std::string, typeof_native_obj>& native_data_structure::get_lib_obj() {
	return *this->_lib_data;
}
void native_data_structure::add_lib_obj(const char* key, typeof_native_obj& value) {
	(*_lib_data)[std::string(key)] = value;
}
std::map<std::string, typeof_native_obj>& native_data_structure::get_request_obj() {
	return *this->_request_data;
}
void native_data_structure::add_request_obj(const std::string key, typeof_native_obj value) {
	(*_request_data)[key.c_str()] = value;
}
void native_data_structure::clear() {
	if (_is_disposed == TRUE)return;
	_is_disposed = TRUE;
	_free_obj(_lib_data);
	_free_obj(_request_data);
	delete _native_modules; _native_modules = NULL;
}
native_data_structure::~native_data_structure() {
	this->clear();
}
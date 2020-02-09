#	include "wj_pdf.h"
#	include <web_jsx/web_jsx.h>
#	include <web_jsx/v8_util.h>
#	include "pdf_generator.h"
#	include <map>
using namespace sow_web_jsx;
void v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>& out_put) {
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Array> property_names = v8_obj->GetOwnPropertyNames(ctx).ToLocalChecked();
	uint32_t length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		v8::Local<v8::Value> key = property_names->Get(ctx, i).ToLocalChecked();
		v8::Local<v8::Value> value = v8_obj->Get(ctx, key).ToLocalChecked();
		if (value->IsNullOrUndefined())continue;
		if (key->IsString() && value->IsString()) {
			/*native_string native_key(isolate, key);
			native_string native_value(isolate, value);
			out_put[native_value.get_string().data()] = native_value.get_string().data();*/
			v8::String::Utf8Value utf8_key(isolate, key);
			v8::String::Utf8Value utf8_value(isolate, value);
			std::string* key_str = new std::string(*utf8_key);
			std::string* val_str = new std::string(*utf8_value);
			out_put[key_str->data()] = val_str->data();
			//free(key_str); free(val_str);
		}
	}
}
void generate_pdf(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//11:14 PM 12/4/2018
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8_str(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Value> v8_path_str = config->Get(ctx, v8_str(isolate, "path")).ToLocalChecked();
		if (v8_path_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8_str(isolate, "Output path required!!!")));
			return;
		}
		v8::Local<v8::Value> v8_url_str = config->Get(ctx, v8_str(isolate, "url")).ToLocalChecked();
		bool form_body = to_boolean(isolate, config->Get(ctx, v8_str(isolate, "from_body")).ToLocalChecked());
		//v8::Local<v8::Boolean> b = v8::Local<v8::Boolean>::Cast(config->Get(ctx, v8_str(isolate, "from_body")).ToLocalChecked());
		if (form_body == false) {
			if (!args[1]->IsString() || args[0]->IsNullOrUndefined()) {
				if (!v8_url_str->IsString() && v8_url_str->IsNullOrUndefined()) {
					config.Clear();
					isolate->ThrowException(v8::Exception::TypeError(
						v8_str(isolate, "Body string required!!!")));
					return;
				}
			}
		}
		native_string utf8_path_str(isolate, v8_path_str);
		
		std::string* abs_path = new std::string(sow_web_jsx::wrapper::get_root_dir());
		sow_web_jsx::get_server_map_path(utf8_path_str.c_str(), *abs_path);
		v8::Local<v8::Value> v8_global_settings_str = config->Get(ctx, v8_str(isolate, "global_settings")).ToLocalChecked();
		auto wgs_settings = new std::map<const char*, const char*>();
		if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
			v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
			v8_global_settings_str.Clear();
			v8_global_settings_object.Clear();
		}
		v8::Local<v8::Value> v8_object_settings_str = config->Get(ctx, v8_str(isolate, "object_settings")).ToLocalChecked();
		auto wos_settings = new std::map<const char*, const char*>();
		if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
			v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
			v8_object_settings_str.Clear();
			v8_object_settings_object.Clear();
		}
		config.Clear();
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		int rec = 0;
		rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
		_free_obj(wgs_settings); _free_obj(wos_settings);
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			utf8_path_str.clear();
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_msg"),
				v8_str(isolate, pdf_gen->get_status_msg())
			);
			args.GetReturnValue().Set(v8_result);
			v8_result.Clear();
			delete pdf_gen; _free_obj(abs_path);
			return;
		}

		if (!form_body) {
			if (v8_url_str->IsString() && !v8_url_str->IsNullOrUndefined()) {
				native_string utf8_url_str(isolate, v8_url_str);
				rec = pdf_gen->generate_from_url(utf8_url_str.c_str(), abs_path->c_str());
				utf8_url_str.clear();
			}
			else {
				native_string utf8_body_str(isolate, args[1]);
				rec = pdf_gen->generate_to_path(utf8_body_str.c_str(), abs_path->c_str());
				utf8_body_str.clear();
			}

		}
		else {
			auto str = new std::string(sow_web_jsx::wrapper::get_body_stream().str());
			rec = pdf_gen->generate_to_path(str->c_str(), abs_path->c_str());
			_free_obj(str);
		}
		_free_obj(abs_path); utf8_path_str.clear();
		if (rec > 0) {
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_msg"),
				v8_str(isolate, "Success")
			);
		}
		else {
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				ctx,
				v8_str(isolate, "ret_msg"),
				v8_str(isolate, pdf_gen->get_status_msg())
			);
		}
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		delete pdf_gen;
		return;
	}
	catch (std::exception & e) {
		args.GetReturnValue().Set(v8_str(isolate, e.what()));
	}
}
void generate_pdf_from_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (sow_web_jsx::wrapper::is_http_status_ok() == FALSE) {
		args.GetReturnValue().Set(v8::Number::New(isolate, -1));
		return;
	}
	pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
	int rec = 0;
	if (args[0]->IsObject() && !args[0]->IsNullOrUndefined()) {
		v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_path_str = config->Get(ctx, v8_str(isolate, "path")).ToLocalChecked();
		if (!v8_path_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8_str(isolate, "You should not set output path here!!!")));
			return;
		}
		v8::Local<v8::Value> v8_url_str = config->Get(ctx, v8_str(isolate, "url")).ToLocalChecked();
		if (!v8_url_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8_str(isolate, "You should not set reading url here!!!")));
			return;
		}
		v8::Local<v8::Value> v8_global_settings_str = config->Get(ctx, v8_str(isolate, "global_settings")).ToLocalChecked();
		auto wgs_settings = new std::map<const char*, const char*>();
		if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
			v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
			v8_global_settings_str.Clear();
			v8_global_settings_object.Clear();
		}
		v8::Local<v8::Value> v8_object_settings_str = config->Get(ctx, v8_str(isolate, "object_settings")).ToLocalChecked();
		auto wos_settings = new std::map<const char*, const char*>();
		if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
			v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
			v8_object_settings_str.Clear();
			v8_object_settings_object.Clear();
		}
		config.Clear();
		rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
		_free_obj(wgs_settings); _free_obj(wos_settings);
	}
	else {
		rec = pdf_gen->init(true);
	}
	std::stringstream& _body_stream = sow_web_jsx::wrapper::get_body_stream();
	if (rec < 0) {
		_body_stream.clear(); std::stringstream().swap(_body_stream);
		_body_stream << pdf_gen->get_status_msg();
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		pdf_gen->dispose();
		delete pdf_gen;
		return;
	}

	std::string* str = new std::string(_body_stream.str());
	_body_stream.clear(); std::stringstream().swap(_body_stream);
	std::string* str_output = new std::string();
	rec = pdf_gen->generate(str->c_str(), *str_output);
	_free_obj(str);
	if (rec < 0) {
		_free_obj(str_output);
		_body_stream << pdf_gen->get_status_msg();
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		pdf_gen->dispose();
		delete pdf_gen;
		return;
	}
	_body_stream.write(str_output->c_str(), str_output->size());
	_free_obj(str_output);
	sow_web_jsx::wrapper::add_header("wkhtmltopdf_version", pdf_gen->version);
	pdf_gen->dispose();
	delete pdf_gen;
	sow_web_jsx::wrapper::add_header("Accept-Ranges", "bytes");
	sow_web_jsx::wrapper::add_header("Content-Type", "application/pdf");
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
	return;
}
void pdf_export(v8::Isolate* isolate, v8::Handle<v8::Object> target){
	v8::Handle<v8::Object> pdf_object = v8::Object::New(isolate);
	wjsx_set_method(isolate, pdf_object, "generate", generate_pdf);
	wjsx_set_method(isolate, pdf_object, "generate_from_body", generate_pdf_from_body);
	wjsx_set_object(isolate, target, "pdf", pdf_object);
}

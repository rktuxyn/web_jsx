/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//1:57 PM 1/16/2020
#if !defined(_crypto_wrapper_h)
#	define _crypto_wrapper_h
#	include <v8.h>
void cypto_export(v8::Isolate* isolate, v8::Handle<v8::Object> target);
#endif//!_crypto_wrapper_h
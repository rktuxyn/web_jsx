/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_zgzip_h)
#	define _zgzip_h
//3:45 PM 11/24/2018
#	include	<iostream>
#	include <sstream>
namespace gzip {
	/*template<class _out_stream>
	void compress_gzip (std::stringstream&source_stream, _out_stream&out_stream) {
		write_magic_header(out_stream);
		deflate_stream(source_stream, out_stream);
	}*/
	void compress_gzip(std::stringstream& source_stream, std::ostream& out_stream);
}; // namespace gzip
#endif//_zgzip_h
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
//7:08 PM 12/25/2018
#	include "curl_util.h"
CURLcode fsys_web_jsx::ssl_ctx_callback(
	CURL* curl, 
	void* ssl_ctx, 
	void* userptr
) {
	return CURLE_OK;
}
void curl_dump(
	const char* text,
	FILE* stream, unsigned char* ptr,
	size_t size
){
	size_t i;
	size_t c;
	unsigned int width = 0x10;

	fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
		text, (long)size, (long)size);

	for (i = 0; i < size; i += width) {
		fprintf(stream, "%4.4lx: ", (long)i);

		/* show hex to the left */
		for (c = 0; c < width; c++) {
			if (i + c < size)
				fprintf(stream, "%02x ", ptr[i + c]);
			else
				fputs("   ", stream);
		}

		/* show data on the right */
		for (c = 0; (c < width) && (i + c < size); c++) {
			char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
			fputc(x, stream);
		}

		fputc('\n', stream); /* newline */
	}
}
int fsys_web_jsx::debug_log(
	CURL* handle, 
	curl_infotype type,
	char* data, size_t size,
	void* userp
){
	const char* text;
	(void)handle; /* prevent compiler warning */
	(void)userp;

	switch (type) {
	case CURLINFO_TEXT:
		fprintf(stderr, "== Info: %s", data);
	default: /* in case a new one is introduced to shock us */
		return 0;

	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		break;
	case CURLINFO_SSL_DATA_OUT:
		text = "=> Send SSL data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		break;
	case CURLINFO_SSL_DATA_IN:
		text = "<= Recv SSL data";
		break;
	}
	curl_dump(text, stderr, (unsigned char*)data, size);
	return 0;
}

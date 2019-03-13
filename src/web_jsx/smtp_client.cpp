#include "smtp_client.h"
void date_time_now(std::string&ret) {
	time_t t;
	struct tm *tm;
	ret.resize(32);//RFC5322_TIME_LEN
	time(&t);
	tm = localtime(&t);
	strftime(&ret[0], 32, "%a, %d %b %Y %H:%M:%S %z", tm);
	return;
};
class stringdata {
public:
	std::string msg;
	size_t bytesleft;
	stringdata(std::string &&m)
		: msg{ m }, bytesleft{ msg.size() }
	{}
	stringdata(std::string &m) = delete;
};
void generate_message_id(std::string&ret) {
	//const int MESSAGE_ID_LEN = 37;
	time_t t;
	struct tm tm;
	std::string ret;
	ret.resize(15);
	time(&t);
	gmtime_s(&tm, &t);
	strftime(const_cast<char *>(ret.c_str()), 37, "%Y%m%d%H%M%S.", &tm);
	ret.reserve(37);
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	while (ret.size() < 37) {
		ret += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return;
};
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
	stringdata *text = reinterpret_cast<stringdata*>(userp);
	if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1) || (text->bytesleft == 0)) {
		return 0;
	}
	if ((nmemb * size) >= text->msg.size()) {
		text->bytesleft = 0;
		return text->msg.copy(reinterpret_cast<char *>(ptr), text->msg.size());
	}

	return 0;
};
#define FROM_ADDR    "<sender@example.org>"
#define TO_ADDR      "<addressee@example.net>"
#define CC_ADDR      "<info@example.org>"

#define FROM_MAIL "Sender Person " FROM_ADDR
#define TO_MAIL   "A Receiver " TO_ADDR
#define CC_MAIL   "John CC Smith " CC_ADDR

struct upload_status { int lines_read; };
void setPayloadText(const std::string &to,
	const std::string &from,
	const std::string &cc,
	const std::string &nameFrom,
	const std::string &subject,
	const std::string &body,
	char *payload_text) {
	static const char *payload_text[] = {
		"Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n",
		"To: " TO_MAIL "\r\n",
		"From: " FROM_MAIL "\r\n",
		"Cc: " CC_MAIL "\r\n",
		"Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
		"rfcpedant.example.org>\r\n",
		"Subject: SMTP example message\r\n",
		"\r\n", /* empty line to divide headers from body, see RFC5322 */
		"The body of the message starts here.\r\n",
		"\r\n",
		"It could be a lot of lines, could be MIME encoded, whatever.\r\n",
		"Check RFC5322.\r\n",
		NULL
	};
	std::string cur_date;
	date_time_now(cur_date);
	std::string msg_id;
	generate_message_id(msg_id);
	sprintf(payload_text, "Date:%\r\nTo: c%\r\nFrom: c%\r\nc%\r\nc%\r\nc%\r\nc%\r\nc%\r\nc%\r\nc%c\r\n");
}
CURLcode sendEmail(const std::string &to,
	const std::string &from,
	const std::string &cc,
	const std::string &nameFrom,
	const std::string &subject,
	const std::string &body,
	const std::string &url,
	const std::string &password) {
	CURLcode ret = CURLE_OK;

	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;

	CURL *curl = curl_easy_init();

	//setPayloadText(to, from, cc, nameFrom, subject, body);

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_USERNAME, from.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		//curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + from + ">").c_str());
		recipients = curl_slist_append(recipients, ("<" + to + ">").c_str());
		recipients = curl_slist_append(recipients, ("<" + cc + ">").c_str());

		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		ret = curl_easy_perform(curl);

		if (ret != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
		}

		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
	}

	return ret;
}

void smtp_client::smtp_request::set_error(const char * error) {

};

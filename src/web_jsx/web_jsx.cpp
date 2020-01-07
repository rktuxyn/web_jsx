/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(FAST_CGI_APP)
#if !defined(_web_jsx_fcgi_h)
#include "web_jsx_fcgi.h"
#endif//!_web_jsx_cgi_h
#endif//FAST_CGI_APP
#if !defined(_web_jsx_cgi_h)
#include "web_jsx_cgi.h"
#endif//!_web_jsx_cgi_h
//#if !defined(_my_sql_h)
//#include <my_sql.h>
//#endif//!_my_sql_h
//void test_pgsql() {
//	npgsql* pgsql = new npgsql();
//	int rec = pgsql->connect("Server=localhost; Port=5432; UserId=postgres;Password=pg1234;Database=sow_trading_cse; keepalive=10; CommandTimeout=100000;");
//	if (rec >= 0) {
//		std::map<std::string, char*>* result = new std::map<std::string, char*>();
//		rec = pgsql->execute_io("datafeeds.__get__market_depth", "{}", "{}", *result);
//		if (rec < 0) {
//			std::cout << pgsql->get_last_error();
//		}
//		else {
//			auto search = result->find("_ret_data_table");
//			if (search != result->end()) {
//				try {
//					std::cout << search->second;
//				}
//				catch (std::exception& e) {
//					std::cout << e.what() << '\n';
//				}
//			}
//			else {
//				std::cout << "Not found\n";
//			}
//		}
//	}
//	else {
//		std::cout << pgsql->get_last_error();
//	}
//	pgsql->close(); delete pgsql;
//}
//void test_mysql() {
//	my_sql* sql = new my_sql();
//	mysqlw::connection_details* con = new mysqlw::connection_details();
//	con->database = new std::string("web_jsx_db");
//	con->host = new std::string("localhost");
//	con->user = new std::string("root");
//	con->password = new std::string("mysql123");
//	con->unix_socket = NULL;
//	con->port = 0;
//	con->clientflag = 0;
//	if (sql->connect(con) == connection_state::CLOSED) {
//		std::cout << sql->get_last_error();
//	}
//	else {
//		const char* rs = sql->execute("INSERT INTO Persons (`PersonID`, `LastName`, `FirstName`, `Address`, `City`)VALUES (11, 'Rajib', 'Chy', 'Panchlaish, katalgong', 'Chittagong');");
//		if (sql->has_error()) {
//			std::cout << sql->get_last_error();
//		}
//		else {
//			std::cout << rs;
//		}
//	}
//	sql->exit_all();
//	delete sql;
//}
//void encrypt_decrypt() {
//	std::string key, iv, error;
//	if (crypto::generate_key_iv(key, iv, error) == FALSE) {
//		std::cout << error.c_str();
//		return;
//	}
//	std::cout << "Key:" << key.c_str() << std::endl;
//	std::cout << "Iv:" << iv.c_str() << std::endl;
//	std::string plain_text("");
//	const char* str = "No man can live without speed.";
//	for (int i = 0; i < 20; i++) {
//		plain_text.append(str);
//	}
//	std::stringstream dest(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
//	int rec = crypto::encrypt(plain_text.c_str(), key.c_str(), iv.c_str(), dest);
//	if (rec == FALSE) {
//		std::cout << "Encrypted Error:\r\n" << dest.str() << "\r\n";
//		return;
//	}
//	std::cout << "Encrypted:\r\n" << dest.str() << "\r\n";
//	std::string encrypted_text(dest.str().c_str(), rec); dest.clear(); std::stringstream().swap(dest);
//	rec = crypto::decrypt(encrypted_text.c_str(), key.c_str(), iv.c_str(), dest);
//	encrypted_text.clear();
//	if (rec == FALSE) {
//		std::cout << "Decrypted Error:\r\n" << dest.str() << "\r\n";
//		return;
//	}
//	std::cout << "Decrypted:\r\n" << dest.str() << "\r\n";
//}
int main(int argc, char *argv[], char*envp[]) {
	std::ios::sync_with_stdio(false);
	if (is_user_interactive() == TRUE) {
		//encrypt_decrypt();
		if (argc > 1) {
			try {
				web_jsx_cgi::app_core::prepare_console_response(argc, argv, false);
			} catch (std::exception&e) {
				std::cout << "\r\n";
				std::cout << e.what() << "\r\n";
			}
			return EXIT_SUCCESS;
		}
		print_info();
		return EXIT_SUCCESS;
	}
	if (argc > 1) {
		const char*interactive_req = const_cast<const char*>(argv[1]);
		if (strcmp(interactive_req, "I_REQ") == 0) {
			web_jsx_cgi::app_core::prepare_console_response(argc, argv, true);
			return EXIT_SUCCESS;
		}
	}
	//12:28 AM 1/28/2019
#if defined(FAST_CGI_APP)
	return web_jsx_cgi::fcgi_request::request_handler(const_cast<const char*>(argv[0]));
#else
	return web_jsx_cgi::cgi_request::request_handler(const_cast<const char*>(argv[0]));
#endif//!FAST_CGI_APP
}
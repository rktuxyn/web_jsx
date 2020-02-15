#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_my_sql_h)
#	define _my_sql_h
#	include <winsock2.h>
#	include <mysql.h>
#if !defined(mysqlw_export)
#	define mysqlw_export __declspec(dllexport)
#endif//!mysqlw_export
#	include "connection_state.h"
#pragma warning (disable : 4996)
namespace mysqlw {
	typedef struct conn_pool {
		struct conn_pool* next;    /* pointer to next member*/
		MYSQL* conn;                  /* MySQL connection handle*/
		connection_state conn_state;
		int busy;                   /* connection busy flag*/
		int error_code;
		const char* error_msg;
	}connection_pool;
	typedef struct {
		std::string* host;
		std::string* user;
		std::string* password;
		std::string* database;
		unsigned int port;
		const char* unix_socket;
		unsigned long clientflag;
	}connection_details;
	class mysqlw_export mysqlw_connection {
	public:
		connection_state conn_state;
	public:
		mysqlw_connection();
		mysqlw_connection(const connection_details* connection_info);
		~mysqlw_connection();
		int connect(const connection_details* connection_info);
		int connect();
		connection_pool* create_connection_pool();
		void free_connection_pool(connection_pool* cpool);
		void exit_nicely(connection_pool* cpool);
		void exit_all();
		void close_all_connection();
		int errcode();
		int switch_database(const connection_details* connection_info);
		const char* get_last_error();
		int errc;
	protected:
		const connection_details* _cinfo;
		int validate_cinfo(const connection_details* cinfo);
		connection_pool* _active_pools;
		char* _internal_error;
		void panic(const char* error, int code);
	};
	class mysqlw_export mysqlw_query {
	public:
		mysqlw_query(connection_pool* cpool);
		~mysqlw_query();
		int try_execute(const char* sql);
		const char* get_mysql_eror();
		MYSQL_RES* _execute(const char* sql);
		int execute_query(const char* sql);
		const char* get_first_col_val();
		void free_result();
		void free_connection();
		__int64 insert_id();
		MYSQL_FIELD* fetch_field();
		char* fetch_fieldname();
		int ping();
		void exit_nicely();
	protected:
		connection_pool* _cpool;
		MYSQL_RES* _res;
		MYSQL_ROW _row;
	};
}
class mysqlw_export my_sql {
public:
	my_sql();
	~my_sql();
	connection_state connect(mysqlw::connection_details* connection_info);
	connection_state connect();
	connection_state state();
	const char* get_last_error();
	template<class _func>
	int execute(const char* sql, _func func);
	const char* execute(const char* sql);
	bool has_error();
	void exit_all();
	void close_all_connection();
	int switch_database(const char* database_name);
private:
	char* _internal_error;
	int _errc;
	void panic(const char* error, int code);
	mysqlw::mysqlw_connection* _con;
	mysqlw::connection_details* _connection_inf;
};
template<class _func>
int my_sql::execute(const char* sql, _func func) {
	if (this->state() == connection_state::CLOSED) {
		this->panic("Connection not initilized yet...", -1);
		return _errc;
	}
	mysqlw::connection_pool* cpool = this->_con->create_connection_pool();
	if (cpool->error_code < 0) {
		this->panic(cpool->error_msg, cpool->error_code);
		this->_con->exit_nicely(cpool);
		return _errc;
	}
	mysqlw::mysqlw_query* query = new mysqlw::mysqlw_query(cpool);
	MYSQL_RES* res = query->_execute(sql);
	if (res == NULL) {
		this->panic(query->get_mysql_eror(), -1);
		delete query;
		return _errc;
	}
	int count = 0;
	if (mysql_num_rows( res ) != 0) {
		int n_fields = mysql_num_fields(res);
		if (n_fields != 0) {
			size_t len = 0;
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res)) != NULL) {
				std::vector<char*>* rows = new std::vector<char*>();
				for (int i = 0; i < n_fields; i++) {
					char* c = row[i];
					len = strlen(c);
					char* copy = new char[len + 1];
					strcpy(copy, c);
					rows->push_back(copy);
				}
				func(count, *rows); count++;
				rows->clear(); delete rows;
				row = NULL;
			}
		}
	}
	query->free_result(); query->free_connection();
	delete query; res = NULL;
	return count;
}
#endif//!_my_sql_h
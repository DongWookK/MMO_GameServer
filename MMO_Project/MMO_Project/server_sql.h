#pragma once
#include "pch.h"
#include "thread_sql.h"

class server_sql : public thread_sql
{
public:
	server_sql(SQLHDBC hdbc) : thread_sql(hdbc) {};

public:
	auto prepare() -> fw::error override
	{
		fw::error error_code{};

		error_code = prepare_server_info_select();
		ASSERT_RETURN_VALUE(!error_code, error_code);
		//error_code = prepare_load_server_dsn();
		//ASSERT_RETURN_VALUE(!error_code, error_code);

		return error_code;
	}

private:
	auto prepare_server_info_select() -> fw::error
	{
		ASSERT_RETURN_VALUE(stmt_ != SQL_NULL_HSTMT, error::code::sql_stmt_invalid);

		SQLWCHAR* query = (SQLWCHAR*)L"{CALL usp_server_info_select(?,?)}";
		auto ret_ = SQLPrepareW(stmt_, query, SQL_NTS);
		if (!SQL_SUCCEEDED(ret_))
		{
			log_error(stmt_, SQL_HANDLE_STMT, "db_manager::prepare");
			return error::code::sql_fail;
		}

		sql_param_50 = SQL_NTS;
		sql_param_int = 0;

		BIND_PARAM(stmt_, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, &ip_, 0, &sql_param_50);
		BIND_PARAM(stmt_, 2, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &server_no_, 0, &sql_param_int);

		return error::code::ok;
	}

public:
	auto server_info_select(std::wstring_view ip) -> fw::error
	{
		ASSERT_RETURN_VALUE(stmt_ != SQL_NULL_HSTMT, error::code::sql_stmt_invalid);

		wcsncpy_s(ip_, ip.data(), _TRUNCATE);
		sql_param_50 = SQL_NTS;

		auto  ret_ = SQLExecute(stmt_);
		if (!SQL_SUCCEEDED(ret_))
		{
			SQLCloseCursor(stmt_);
			return error::code::sql_fail;
		}

		SQLCloseCursor(stmt_);

		return error::code::ok;
	}

public:
	auto get_server_no() const -> int16_t
	{
		return server_no_;
	}

private:
	// bind parameter
	int16_t server_no_ = 0;
	SQLWCHAR ip_[50] = { 0 };

	// 데이터의 길이를 알려줄 지시자
	SQLLEN   sql_param_int = 0;
	SQLLEN   sql_param_50 = SQL_NTS;
};
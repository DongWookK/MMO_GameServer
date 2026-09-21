#pragma once
#include "pch.h"
#include "sql.h"

class server_sql : public sql
{
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
		SQLWCHAR* query = (SQLWCHAR*)L"{CALL usp_server_info_select(?,?)}";
		ret_ = SQLPrepareW(stmt_, query, SQL_NTS);
		if (!SQL_SUCCEEDED(ret_))
		{
			// log ret
			return error::code::ObjectAcquireFail;
		}

		sql_param_50 = SQL_NTS;
		sql_param_int = 0;

		BIND_PARAM(stmt_, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, &ip_, 0, &sql_param_50);
		BIND_PARAM(stmt_, 2, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &server_no_, 0, &sql_param_int);

		return error::code::none;
	}

public:
	auto server_info_select(std::wstring_view ip) -> fw::error
	{
		ASSERT_RETURN_VALUE(stmt_ != SQL_NULL_HSTMT, error::code::UserLoginFail);

		wcsncpy_s(ip_, ip.data(), _TRUNCATE);
		sql_param_50 = SQL_NTS;

		ret_ = SQLExecute(stmt_);
		if (!SQL_SUCCEEDED(ret_))
		{
			SQLCloseCursor(stmt_);
			return error::code::UserLoginFail;
		}

		SQLCloseCursor(stmt_);
	}

public:
	auto get_server_no() const -> int32_t
	{
		return server_no_;
	}

private:
	SQLHSTMT stmt_;

	// bind parameter
	int32_t server_no_ = 0;
	SQLWCHAR ip_[50] = { 0 };

	// 데이터의 길이를 알려줄 지시자
	SQLLEN   sql_param_int = 0;
	SQLLEN   sql_param_50 = SQL_NTS;
};
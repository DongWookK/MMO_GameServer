#pragma once
#include "pch.h"
#include "sql.h"

class user_sql : public thread_sql
{
public:
	auto prepare() -> fw::error override
	{
		fw::error error_code{};
		
		error_code = prepare_user_login();
		

		return error_code;
	}

private:
	auto prepare_user_login() -> fw::error
	{
		SQLWCHAR* query = (SQLWCHAR*)L"{CALL usp_user_login(?,?)}";
		ret_ = SQLPrepareW(m_hStmt, query, SQL_NTS);
		if (!SQL_SUCCEEDED(ret_)) 
		{
			// log ret
			return error::code::sql_fail;
		}

		m_cbParam1 = 0;
		m_cbParam2 = SQL_NTS;

		BIND_PARAM(m_hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_bindUserId, &m_cbParam1);
		BIND_PARAM(m_hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, m_bindUserName, 0, &m_cbParam2);

		return error::code::ok;
	}

public:
	auto exec_user_login(std::wstring_view ip) -> fw::error
	{

	}

private:
	SQLHSTMT m_hStmt;

	// bind parameter
	int      m_bindUserId = 0;
	SQLWCHAR m_bindUserName[50] = { 0 };

	// 데이터의 길이를 알려줄 지시자
	SQLLEN   m_cbParam1 = 0; 
	SQLLEN   m_cbParam2 = SQL_NTS;
};
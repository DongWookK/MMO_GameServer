#pragma once
#include "pch.h"
#include "sql.h"

class user_sql : public sql
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

	}

public:
	auto exec_user_login() -> fw::error
	{

	}
};
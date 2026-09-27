#pragma once
#include "pch.h"
#include "thread_sql.h"

class user_sql : public thread_sql
{
public:
	explicit user_sql(nanodbc::connection conn) : thread_sql(std::move(conn)) {};

public:
	auto prepare() -> fw::error override
	{
		fw::error error_code{};

		error_code = try_sql("user_sql::prepare_user_login", [&] {
			user_login_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_user_login(?,?)}"));
			});
		ASSERT_RETURN_VALUE(!error_code, error_code);

		return error_code;
	}

public:
	auto exec_user_login(std::wstring_view user_name, int32_t& out_user_no) -> fw::error
	{
		const std::wstring user_name_str{ user_name };
		int32_t user_no = 0;

		auto error_code = try_sql("user_sql::exec_user_login", [&] {
			user_login_stmt_.bind(0, user_name_str.c_str());
			user_login_stmt_.bind(1, &user_no, nanodbc::statement::PARAM_OUT);

			auto result = user_login_stmt_.execute();
			// OUTPUT 파라미터는 모든 결과셋을 소비한 뒤에 채워진다
			while (result.next_result()) {}
			});
		ASSERT_RETURN_VALUE(!error_code, error_code);

		out_user_no = user_no;
		return error_code;
	}

private:
	nanodbc::statement user_login_stmt_{};
};

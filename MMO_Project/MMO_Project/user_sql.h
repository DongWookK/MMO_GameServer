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

        error_code = prepare_user_login();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        return error_code;
    }

private:
    auto prepare_user_login() -> fw::error
    {
        return try_sql("user_sql::prepare_user_login", [&]
            {
                user_login_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_user_login(?,?)}"));

                user_login_stmt_.bind_strings(0, user_name_, std::size(user_name_), 1);
                user_login_stmt_.bind(1, &user_no_, nanodbc::statement::PARAM_OUT);
            });
    }

public:
    auto user_login(std::wstring_view user_name, int32_t& out_user_no) -> fw::error
    {
        wcsncpy_s(user_name_, user_name.data(), (std::min)(user_name.size(), std::size(user_name_) - 1));

        auto error_code = try_sql("user_sql::user_login", [&]
            {
                auto result = user_login_stmt_.execute();

                // OUTPUT 파라미터는 모든 결과셋을 소비한 뒤에 채워진다
                while (result.next_result()) {}
            });
        ASSERT_RETURN_VALUE(!error_code, error_code);

        out_user_no = user_no_;
        return error_code;
    }

private:
    nanodbc::statement user_login_stmt_{};

    wchar_t user_name_[100] = { 0 };
    int32_t user_no_{};
};
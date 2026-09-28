#pragma once
#include "pch.h"
#include "thread_sql.h"

class server_sql : public thread_sql
{
public:
    explicit server_sql(nanodbc::connection conn) : thread_sql(std::move(conn)) {};

public:
    auto prepare() -> fw::error override
    {
        fw::error error_code{};

        error_code = prepare_server_info_select();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        error_code = prepare_server_dsn_select();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        return error_code;
    }

private:
    auto prepare_server_info_select() -> fw::error
    {
        return try_sql("server_sql::prepare_server_info_select", [&]
            {
                server_info_select_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_server_info_select(?)}"));

                server_info_select_stmt_.bind_strings(0, ip_, std::size(ip_), 1);
            });
    }

public:
    auto server_info_select(std::wstring_view ip, nanodbc::result& out_result) -> fw::error
    {
        wcsncpy_s(ip_, ip.data(), (std::min)(ip.size(), std::size(ip_) - 1));

        return try_sql("server_sql::server_info_select", [&]
            {
                out_result = server_info_select_stmt_.execute();
            });
    }

private:
    auto prepare_server_dsn_select() -> fw::error
    {
        return try_sql("server_sql::prepare_server_dsn_select", [&]
            {
                server_dsn_select_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_server_dsn_select(?)}"));

                server_dsn_select_stmt_.bind(0, &server_no_, nanodbc::statement::PARAM_IN);
            });
    }

public:
    auto server_dsn_select(uint16_t server_no, nanodbc::result& out_result) -> fw::error
    {
        server_no_ = server_no;

        return try_sql("server_sql::server_dsn_select", [&]
            {
                out_result = server_dsn_select_stmt_.execute();
            });
    }

private:
    nanodbc::statement server_info_select_stmt_{};
    nanodbc::statement server_dsn_select_stmt_{};


    wchar_t ip_[50] = { 0 };
    uint16_t server_no_{};
};
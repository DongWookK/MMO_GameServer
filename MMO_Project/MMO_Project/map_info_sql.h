#pragma once
#include "pch.h"
#include "thread_sql.h"

class map_info_sql : public thread_sql
{
public:
    explicit map_info_sql(nanodbc::connection conn) : thread_sql(std::move(conn)) {};

public:
    auto prepare() -> fw::error override
    {
        fw::error error_code{};

        error_code = prepare_map_info_select();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        return error_code;
    }

private:
    auto prepare_map_info_select() -> fw::error
    {
        return try_sql("map_info_sql::prepare_map_info_select", [&]
            {
                map_info_select_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_map_info_select}"));
            });
    }

public:
    auto map_info_select(nanodbc::result& out_result) -> fw::error
    {
        return try_sql("map_info_sql::map_info_select", [&]
            {
                out_result = map_info_select_stmt_.execute();
            });
    }

private:
    nanodbc::statement map_info_select_stmt_{};
};

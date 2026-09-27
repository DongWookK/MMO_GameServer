#pragma once
#include "pch.h"

class thread_sql {
public:
    explicit thread_sql(nanodbc::connection conn) : conn_(std::move(conn)) {}

    virtual ~thread_sql() = default;

    virtual auto prepare() -> fw::error = 0;

protected:
    template<typename Fn>
    auto try_sql(const std::string_view context_msg, Fn&& fn) -> fw::error
    {
        try
        {
            std::forward<Fn>(fn)();
        }
        catch (const nanodbc::database_error& e)
        {
            FLOG_ERROR("[{}] ODBC Error -> [State: {}, Native: {}, Msg: {}]", context_msg, e.state(), e.native(), e.what());
            return error::code::sql_fail;
        }
        catch (const std::exception& e)
        {
            FLOG_ERROR("[{}] nanodbc Error -> {}", context_msg, e.what());
            return error::code::sql_fail;
        }

        return error::code::ok;
    }

protected:
    nanodbc::connection conn_;
};

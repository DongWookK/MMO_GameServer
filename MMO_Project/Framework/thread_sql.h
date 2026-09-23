#pragma once
#include "pch.h"
#include "db_manager.h"

class thread_sql {
public:
    thread_sql(SQLHDBC hdbc) : hdbc_(hdbc), stmt_(SQL_NULL_HSTMT) {
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &stmt_);
    }

    virtual ~thread_sql() {
        if (stmt_ != SQL_NULL_HSTMT) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
        }
    }

    virtual auto prepare() -> fw::error = 0;

    auto log_error(SQLHANDLE handle, SQLSMALLINT handle_type, const std::string_view context_msg) -> bool
    {
        std::string all_error_msgs = "";
        SQLSMALLINT i = 1;
        SQLWCHAR SqlState[6] = { 0, };
        SQLINTEGER NativeError = 0;
        SQLWCHAR Msg[SQL_MAX_MESSAGE_LENGTH] = { 0, };
        SQLSMALLINT MsgLen = 0;
        SQLRETURN rc;

        while (true)
        {
            rc = SQLGetDiagRecW(
                handle_type,
                handle,
                i,
                SqlState,
                &NativeError,
                Msg,
                sizeof(Msg) / sizeof(SQLWCHAR),
                &MsgLen
            );

            if (rc == SQL_NO_DATA) break;
            if (!SQL_SUCCEEDED(rc)) break;

            auto wstr_to_str = [](const wchar_t* wstr, int max_len) -> std::string {
                if (!wstr || !*wstr) return "";
                int len = (int)wcsnlen(wstr, max_len);
                if (len <= 0) return "";
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, len, NULL, 0, NULL, NULL);
                if (size_needed <= 0) return "";
                std::string str(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, wstr, len, &str[0], size_needed, NULL, NULL);
                return str;
                };

            std::string state_str = wstr_to_str(SqlState, 5);
            std::string msg_str = wstr_to_str(Msg, SQL_MAX_MESSAGE_LENGTH);

            all_error_msgs += fmt::format("[State: {}, Native: {}, Msg: {}] ", state_str, (long)NativeError, msg_str);
            i++;
        }

        if (all_error_msgs.empty())
        {
            all_error_msgs = "Failed to retrieve ODBC error diagnostic record (handle might be invalid or freed).";
        }

        FLOG_ERROR("[{}] ODBC Error -> {}", context_msg, all_error_msgs);
        return false;
    }

protected:
    SQLHDBC   hdbc_;
    SQLHSTMT  stmt_;
};
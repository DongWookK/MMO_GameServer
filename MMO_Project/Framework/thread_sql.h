#pragma once
#include "pch.h"
#include "db_manager.h"

class thread_sql {
public:
    thread_sql(SQLHDBC hdbc) : hdbc_(hdbc), hstmt_(SQL_NULL_HSTMT) {
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
    }

    virtual ~thread_sql() {
        if (hstmt_ != SQL_NULL_HSTMT) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
        }
    }

    virtual auto prepare() -> fw::error = 0;

protected:
    SQLHDBC   hdbc_;
    SQLHSTMT  hstmt_;
};
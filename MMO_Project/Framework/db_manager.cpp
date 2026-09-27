#pragma once
#include "pch.h"
#include "db_manager.h"

db_manager::~db_manager() {
    disconnect();
}

bool db_manager::connect(const std::wstring& connectionString) {
    try {
        conn_.connect(connectionString);
    }
    catch (const nanodbc::database_error& e) {
        FLOG_CRITICAL("[DB Error] Connection Failed: [State: {}, Native: {}] {}", e.state(), e.native(), e.what());
        return false;
    }

    FLOG_INFO("[DB Success] Connected to MS-SQL successfully!");
    return true;
}

void db_manager::disconnect() {
    sqls_.clear();

    try {
        if (conn_.connected()) {
            conn_.disconnect();
        }
    }
    catch (const nanodbc::database_error& e) {
        FLOG_ERROR("[DB Error] Disconnect Failed: {}", e.what());
    }
}

auto db_manager::prepare() -> fw::error
{
    fw::error error_code{};
    for (auto& thread_sql : sqls_)
    {
        error_code = thread_sql->prepare();
        ASSERT_RETURN_VALUE(!error_code, error_code);
    }

    return error_code;
}

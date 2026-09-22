#pragma once
#pragma comment(lib, "odbc32.lib")
#include "pch.h"
#include "thread_sql.h"

class db_manager {
public:
    using sqls_t = std::vector<std::unique_ptr<thread_sql>>;

public:
    db_manager();
    ~db_manager();

    auto connect(const std::wstring& connectionString) -> bool;
    auto disconnect() -> void;
    
    template<typename T, typename... Args>
    auto register_sql(Args&&... args) -> T* 
    {
        auto sql_ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw_ptr = sql_ptr.get();
        sqls_.push_back(std::move(sql_ptr));
        return raw_ptr;
    }

    template<typename T>
    auto get_sql() -> T* 
    {
        for (auto& sql_ptr : sqls_) {
            if (auto derived = dynamic_cast<T*>(sql_ptr.get())) {
                return derived;
            }
        }
        return nullptr;
    }

    auto prepare() -> fw::error;

    bool execute_login_proc(int userId, const std::wstring& userName);

private:
    void print_error(SQLSMALLINT handleType, SQLHANDLE handle);

private:
    SQLHENV m_env;   // 환경 핸들
    SQLHDBC m_dbc;   // 연결 핸들
    bool    m_isConnected;
    sqls_t  sqls_{};
};
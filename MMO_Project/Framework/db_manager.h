#pragma once
#include "pch.h"
#include "thread_sql.h"

class db_manager {
public:
    using sqls_t = std::vector<std::unique_ptr<thread_sql>>;

public:
    db_manager() = default;
    ~db_manager();

    auto connect(const std::wstring& connectionString) -> bool;
    auto disconnect() -> void;

    template<typename T, typename... Args>
    auto register_sql(Args&&... args) -> T*
    {
        auto sql_ptr = std::make_unique<T>(conn_, std::forward<Args>(args)...);
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

    auto get_sqls() -> sqls_t&
    {
        return sqls_;
    }

    auto prepare() -> fw::error;
    auto get_connection() -> nanodbc::connection& { return conn_; }

private:
    nanodbc::connection conn_{};    // 워커 스레드당 1개 (nanodbc::connection 은 스레드 간 공유 X)
    sqls_t  sqls_{};
};

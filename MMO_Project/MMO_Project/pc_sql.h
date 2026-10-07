#pragma once
#include "pch.h"
#include "thread_sql.h"

class pc_sql : public thread_sql
{
public:
    // usp_pc_create_insert 반환 코드
    enum class create_result : int32_t
    {
        ok = 0,
        name_duplicate = 1,
    };

    // usp_pc_save_update 반환 코드
    enum class save_result : int32_t
    {
        ok = 0,
        not_exist = 1,
    };

    struct save_param
    {
        int32_t user_no{};
        int64_t pc_no{};
        int32_t level{};
        int64_t exp{};
        int32_t hp{};
        int32_t mp{};
        int32_t map_no{};
        double  location_x{};
        double  location_y{};
        double  location_z{};
    };

public:
    explicit pc_sql(nanodbc::connection conn) : thread_sql(std::move(conn)) {};

public:
    auto prepare() -> fw::error override
    {
        fw::error error_code{};

        error_code = prepare_pc_create_insert();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        error_code = prepare_pc_login_select();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        error_code = prepare_pc_list_select();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        error_code = prepare_pc_save_update();
        ASSERT_RETURN_VALUE(!error_code, error_code);

        return error_code;
    }

private:
    auto prepare_pc_create_insert() -> fw::error
    {
        return try_sql("pc_sql::prepare_pc_create_insert", [&]
            {
                pc_create_insert_stmt_.prepare(conn_, NANODBC_TEXT("{? = CALL usp_pc_create_insert(?,?,?,?)}"));

                pc_create_insert_stmt_.bind(0, &create_return_, nanodbc::statement::PARAM_RETURN);
                pc_create_insert_stmt_.bind(1, &user_no_);
                pc_create_insert_stmt_.bind_strings(2, pc_name_, std::size(pc_name_), 1);
                pc_create_insert_stmt_.bind(3, &pc_type_);
                pc_create_insert_stmt_.bind(4, &pc_no_, nanodbc::statement::PARAM_OUT);
            });
    }

public:
    auto pc_create_insert(int32_t user_no, std::wstring_view pc_name, uint8_t pc_type
                          , create_result& out_result, int64_t& out_pc_no) -> fw::error
    {
        user_no_ = user_no;
        wcsncpy_s(pc_name_, pc_name.data(), (std::min)(pc_name.size(), std::size(pc_name_) - 1));
        pc_type_ = pc_type;
        pc_no_ = 0;
        create_return_ = 0;

        auto error_code = try_sql("pc_sql::pc_create_insert", [&]
            {
                auto result = pc_create_insert_stmt_.execute();

                while (result.next_result()) {}
            });
        ASSERT_RETURN_VALUE(!error_code, error_code);

        out_result = static_cast<create_result>(create_return_);
        out_pc_no = pc_no_;
        return error_code;
    }

private:
    auto prepare_pc_login_select() -> fw::error
    {
        return try_sql("pc_sql::prepare_pc_login_select", [&]
            {
                pc_login_select_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_pc_login_select(?,?)}"));

                pc_login_select_stmt_.bind(0, &select_user_no_);
                pc_login_select_stmt_.bind(1, &select_pc_no_);
            });
    }

public:
    auto pc_login_select(int32_t user_no, int64_t pc_no, nanodbc::result& out_result) -> fw::error
    {
        select_user_no_ = user_no;
        select_pc_no_ = pc_no;

        return try_sql("pc_sql::pc_login_select", [&]
            {
                out_result = pc_login_select_stmt_.execute();
            });
    }

private:
    auto prepare_pc_list_select() -> fw::error
    {
        return try_sql("pc_sql::prepare_pc_list_select", [&]
            {
                pc_list_select_stmt_.prepare(conn_, NANODBC_TEXT("{CALL usp_pc_list_select(?)}"));

                pc_list_select_stmt_.bind(0, &list_user_no_);
            });
    }

public:
    auto pc_list_select(int32_t user_no, nanodbc::result& out_result) -> fw::error
    {
        list_user_no_ = user_no;

        return try_sql("pc_sql::pc_list_select", [&]
            {
                out_result = pc_list_select_stmt_.execute();
            });
    }

private:
    auto prepare_pc_save_update() -> fw::error
    {
        return try_sql("pc_sql::prepare_pc_save_update", [&]
            {
                pc_save_update_stmt_.prepare(conn_, NANODBC_TEXT("{? = CALL usp_pc_save_update(?,?,?,?,?,?,?,?,?,?)}"));

                pc_save_update_stmt_.bind(0, &save_return_, nanodbc::statement::PARAM_RETURN);
                pc_save_update_stmt_.bind(1, &save_.user_no);
                pc_save_update_stmt_.bind(2, &save_.pc_no);
                pc_save_update_stmt_.bind(3, &save_.level);
                pc_save_update_stmt_.bind(4, &save_.exp);
                pc_save_update_stmt_.bind(5, &save_.hp);
                pc_save_update_stmt_.bind(6, &save_.mp);
                pc_save_update_stmt_.bind(7, &save_.map_no);
                pc_save_update_stmt_.bind(8, &save_.location_x);
                pc_save_update_stmt_.bind(9, &save_.location_y);
                pc_save_update_stmt_.bind(10, &save_.location_z);
            });
    }

public:
    auto pc_save_update(const save_param& param, save_result& out_result) -> fw::error
    {
        save_ = param;      // 바인딩된 멤버 버퍼에 값만 복사 (주소는 그대로)
        save_return_ = 0;

        auto error_code = try_sql("pc_sql::pc_save_update", [&]
            {
                auto result = pc_save_update_stmt_.execute();
                while (result.next_result()) {}
            });
        ASSERT_RETURN_VALUE(!error_code, error_code);

        out_result = static_cast<save_result>(save_return_);
        return error_code;
    }

private:
    nanodbc::statement pc_create_insert_stmt_{};
    nanodbc::statement pc_login_select_stmt_{};
    nanodbc::statement pc_list_select_stmt_{};
    nanodbc::statement pc_save_update_stmt_{};

    int32_t create_return_{};
    int32_t user_no_{};
    wchar_t pc_name_[32] = { 0 };
    uint8_t pc_type_{};
    int64_t pc_no_{};

    int32_t select_user_no_{};
    int64_t select_pc_no_{};
    int32_t list_user_no_{};

    int32_t    save_return_{};
    save_param save_{};
};

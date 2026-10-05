#pragma once
#include "pch.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include "main_server.h"
#include "thread_manager.h"
#include "thread_local.h"
#include "network_manager.h"
#include "user_manager.h"
#include "troc_user.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "server_sql.h"
#include "user_sql.h"
#include "pc_sql.h"

main_server::~main_server() = default;

auto main_server::start_service() -> fw::error
{
    fw::error error_code{};
    
    error_code = core_setup();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = core_start();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    load_feature();

    error_code = feature_setup();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = feature_start();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    on_service();

    FLOG_INFO("main server::start_service complete!");

    return error_code;
}

auto main_server::stop_service() -> fw::error
{
    fw::error error_code{};

    error_code = feature_stop();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = feature_teardown();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = core_stop();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = core_teardown();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    return error_code;
}

auto main_server::get_io_context() -> boost::asio::io_context*
{
    return io_context_.get();
}

auto main_server::core_setup() -> fw::error
{
    fw::error error_code{};

    error_code = load_config_from_file();
    ASSERT_RETURN_VALUE(!error_code, error_code);

    error_code = set_thread_config();
    ASSERT_RETURN_VALUE(!error_code, error_code);

    error_code = set_network_config();
    ASSERT_RETURN_VALUE(!error_code, error_code);

    return error_code;
}

auto main_server::core_start() -> fw::error
{
    fw::error error_code{};

    error_code = primary_thread_start();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = thread_manager_start();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = fw::network_manager::instance()->start();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    return error_code;
}

auto main_server::core_stop() -> fw::error
{
    fw::error error_code{};

    work_guard_.reset(); // thread run 탈출하게 허용

    error_code = thread_manager_->stop();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    return error_code;
}

auto main_server::core_teardown() -> fw::error
{
    return fw::error();
}

auto main_server::load_feature() -> void
{
    feature_list_.push_back(user_manager::instance());
    feature_list_.push_back(std::make_shared<troc_user>());

    return;
}

auto main_server::feature_setup() -> fw::error
{
    fw::error error{};
    for (const auto feature : feature_list_)
    {
        error = feature->setup();
        ASSERT_RETURN_VALUE(!error, error);
    }

    return error;
}

auto main_server::feature_start() -> fw::error
{
    fw::error error{};
    for (const auto feature : feature_list_)
    {
        error = feature->start();
        ASSERT_RETURN_VALUE(!error, error);
    }

    return error;
}

auto main_server::feature_stop() -> fw::error
{
    fw::error error{};
    for (const auto feature : feature_list_)
    {
        error = feature->stop();
        ASSERT_RETURN_VALUE(!error, error);
    }

    return error;
}

auto main_server::feature_teardown() -> fw::error
{
    fw::error error{};
    for (const auto feature : feature_list_)
    {
        error = feature->teardown();
        ASSERT_RETURN_VALUE(!error, error);
    }

    return error;
}

auto main_server::on_service() -> fw::error
{
    return fw::error();
}

auto main_server::load_config_from_file() -> fw::error
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exe_path(buffer);
    std::filesystem::path file_path = exe_path.parent_path() / std::wstring{L"parameter.json"};

    std::ifstream file(file_path);
    if (!file.is_open()) {
        FLOG_CRITICAL("[Error] 설정 파일을 찾을 수 없습니다: {}", file_path.string());
        return error::code::file_open_fail;
    }

    try {
        nlohmann::json j;
        file >> j;

        std::string utf8_conn = j["database"]["connection_string"].get<std::string>();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_conn.c_str(), (int)utf8_conn.size(), NULL, 0);
        db_conn_str_list_[fw::to_underlying(common::sql_type::info)].resize(size_needed);
        MultiByteToWideChar(CP_UTF8, 0, utf8_conn.c_str(), (int)utf8_conn.size(), &db_conn_str_list_[fw::to_underlying(common::sql_type::info)][0], size_needed);

    }
    catch (const std::exception& e) {
        FLOG_CRITICAL("[Error] JSON 파싱 실패:  {}", e.what());
        return error::code::file_open_fail;
    }

    FLOG_INFO("connection info : db_connection({})", wstring_to_string(db_conn_str_list_[fw::to_underlying(common::sql_type::info)]));

    return error::code::ok;
}

auto main_server::set_thread_config() -> fw::error
{
    fw::error error_code{};
    io_context_ = std::make_unique<worker_context_t>();
    work_guard_ = std::make_unique<work_guard_t>(io_context_->get_executor());

    const auto thread_count = std::thread::hardware_concurrency();
    thread_manager_ = std::make_unique<fw::thread_manager>(get_io_context(), thread_count);

    packet_dispatcher::instance()->set_strands(io_context_.get(), thread_count);
    error_code = thread_manager_->setup();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    return error_code;
}

auto main_server::set_network_config() -> fw::error
{
    fw::error error_code{};

    error_code = fw::network_manager::instance()->setup(io_context_.get());
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    fw::network_manager::instance()->set_accept_handler([](fw::network_manager::session_ptr_t session) {

        spdlog::info("New player connected and assigned to user_manager!");
        });

    fw::network_manager::instance()->set_disconnect_handler([](fw::network_manager::session_ptr_t session)
        {
            spdlog::info("user disconnected session({})", session->get_index());

            if (user_manager::instance()->find_user(session) == nullptr)
            {
                return fw::error{};
            }

            auto error = user_manager::instance()->user_logout(session);
            ASSERT_RETURN_VALUE(!error, error);

            return error;
        });

    return error_code;
}

auto main_server::primary_thread_start() -> fw::error
{
    fw::error error_code{};

    fw::tls::db_list = &dbms_;

#pragma region info
    constexpr auto info_db = fw::to_underlying(common::sql_type::info);
    sql_reigsters_[info_db] = [](db_manager& info_db) -> void
        {
            info_db.register_sql<server_sql>();
            // ...
        };

    if (!dbms_[info_db].connect(db_conn_str_list_[info_db])) {
        FLOG_CRITICAL("PRIMARY Thread :: DB Connection Failed");
        ASSERT_RETURN_VALUE(false, error::code::sql_stmt_invalid);
    }

    sql_reigsters_[info_db](dbms_[info_db]);

    error_code = dbms_[info_db].prepare();
    ASSERT_RETURN_VALUE(!error_code, error_code);

    load_server_info();

#pragma endregion

#pragma region game
    constexpr auto game_db = fw::to_underlying(common::sql_type::game);
    sql_reigsters_[game_db] = [](db_manager& game_db) -> void 
        {
            game_db.register_sql<user_sql>();
            game_db.register_sql<pc_sql>();
            // ...
        };


    if (!dbms_[game_db].connect(db_conn_str_list_[game_db])) {
        FLOG_CRITICAL("PRIMARY Thread :: DB Connection Failed");
        ASSERT_RETURN_VALUE(false, error::code::sql_stmt_invalid);
    }

    sql_reigsters_[game_db](dbms_[game_db]);
    error_code = dbms_[game_db].prepare();
    ASSERT_RETURN_VALUE(!error_code, error_code);
#pragma endregion

    return error_code;
}

auto main_server::load_server_info() -> fw::error
{
    auto error_code = fw::error{};

    auto info_sql = main_server::instance()->get_sql<server_sql>(common::sql_type::info);
    ASSERT_RETURN_VALUE(info_sql != nullptr, error::code::sql_fail);

    auto end_point = fw::network_manager::instance()->get_end_point();
    auto string_ip = end_point.address().to_string();
    auto wstring_ip = fw::string_to_wstring(string_ip);

    nanodbc::result result;
    error_code = info_sql->server_info_select(wstring_ip, result);
    ASSERT_RETURN_VALUE(!error_code, error_code);

    while (result.next())
    {
        server_no_ = static_cast<uint16_t>(result.get<int32_t>(0));
        server_type_ = result.get<uint8_t>(1);
    }

    FLOG_INFO("server_info :: server({}:{})", server_no_, server_type_);

    error_code = info_sql->server_dsn_select(server_no_, result);
    ASSERT_RETURN_VALUE(!error_code, error_code);

    while (result.next())
    {
        auto type = result.get<uint8_t>(0);
        db_conn_str_list_[type] = result.get<std::wstring>(1);
    }
    
    return error_code;
}

auto main_server::thread_manager_start() -> fw::error
{
    auto error_code = fw::error{};
    
    error_code = thread_manager_->start(db_conn_str_list_, sql_reigsters_);
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    return error_code;
}

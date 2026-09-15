#pragma once
#include "pch.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include "main_server.h"
#include "thread_manager.h"
#include "network_manager.h"
#include "user_manager.h"
#include "troc_user.h"
#include <nlohmann/json.hpp>
#include <fstream>

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

    error_code = thread_manager_->start(db_connection_str_);
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
        return error::code::ObjectAcquireFail;
    }

    try {
        nlohmann::json j;
        file >> j;

        std::string utf8_conn = j["database"]["connection_string"].get<std::string>();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_conn.c_str(), (int)utf8_conn.size(), NULL, 0);
        db_connection_str_.resize(size_needed);
        MultiByteToWideChar(CP_UTF8, 0, utf8_conn.c_str(), (int)utf8_conn.size(), &db_connection_str_[0], size_needed);

    }
    catch (const std::exception& e) {
        FLOG_CRITICAL("[Error] JSON 파싱 실패:  {}", e.what());
        return error::code::ObjectAcquireFail;
    }

    FLOG_INFO("connection info : db_connection({})", wstring_to_string(db_connection_str_));

    return error::code::none;
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

            auto error = user_manager::instance()->user_logout(session);
            ASSERT_RETURN_VALUE(!error, error);

            return error;
        });

    return error_code;
}

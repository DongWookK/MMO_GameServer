#pragma once
#include "pch.h"
#include "main_server.h"
#include "thread_manager.h"
#include "network_manager.h"
#include "user_manager.h"
#include "troc_user.h"

auto main_server::start_service() -> fw::error
{
    fw::error error_code{};
    
    error_code = core_setup();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    // network manager는 제일 마지막에 start해야하지않나?
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

    io_context_ = std::make_unique<worker_context_t>();
    work_guard_ = std::make_unique<work_guard_t>(io_context_->get_executor());

    const auto thread_count = std::thread::hardware_concurrency();
    thread_manager_ = std::make_unique<fw::thread_manager>(get_io_context(), thread_count);

    packet_dispatcher::instance()->set_strands(io_context_.get(), thread_count);
    error_code = thread_manager_->setup();
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    error_code = fw::network_manager::instance()->setup(io_context_.get());
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    fw::network_manager::instance()->set_accept_handler([](fw::network_manager::session_ptr_t session) {
        
        spdlog::info("New player connected and assigned to user_manager!");
        });

    return error_code;
}

auto main_server::core_start() -> fw::error
{
    fw::error error_code{};

    error_code = thread_manager_->start();
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

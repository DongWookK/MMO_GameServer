#pragma once
#include "pch.h"
#include "main_server.h"
#include "thread_manager.h"
#include "network_manager.h"

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
    
    for (uint32_t i = 0; i < thread_count; ++i)
    {
        strands_.push_back(std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(io_context_->get_executor()));
    }

    error_code = fw::network_manager::instance()->setup(io_context_.get());
    ASSERT_RETURN_VALUE(!(error_code), error_code);

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
    // todo - feature make_unique + feature_list_t;
    // todo - replace load feature by sql
    return;
}

auto main_server::feature_setup() -> fw::error
{
    return fw::error();
}

auto main_server::feature_start() -> fw::error
{
    return fw::error();
}

auto main_server::feature_stop() -> fw::error
{
    return fw::error();
}

auto main_server::feature_teardown() -> fw::error
{
    return fw::error();
}

auto main_server::on_service() -> fw::error
{

    return fw::error();
}

#pragma once
#include "pch.h"
#include "main_server.h"
#include "thread_manager.h"

auto main_server::start_service() -> fw::error
{
    fw::error error_code{};
    
    error_code = core_setup();
    RETURN_VALUE(error_code);

    error_code = core_start();
    RETURN_VALUE(error_code);

    load_feature();

    error_code = feature_setup();
    RETURN_VALUE(error_code);

    error_code = feature_start();
    RETURN_VALUE(error_code);

    return error_code;
}

auto main_server::stop_service() -> fw::error
{
    fw::error error_code{};

    error_code = feature_stop();
    RETURN_VALUE(error_code);

    error_code = feature_teardown();
    RETURN_VALUE(error_code);

    error_code = core_stop();
    RETURN_VALUE(error_code);

    error_code = core_teardown();
    RETURN_VALUE(error_code);

    return error_code;
}

auto main_server::core_setup() -> fw::error
{
    fw::error error_code{};

    fw::thread_manager::instance()->setup();
    RETURN_VALUE(error_code);

    return error_code;
}

auto main_server::core_start() -> fw::error
{
    fw::error error_code{};

    fw::thread_manager::instance()->start();
    RETURN_VALUE(error_code);

    return error_code;
}

auto main_server::core_stop() -> fw::error
{
    fw::error error_code{};

    fw::thread_manager::instance()->stop();
    RETURN_VALUE(error_code);
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

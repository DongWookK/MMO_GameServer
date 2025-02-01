#include "pch.h"
#include "main_server.h"
#include "thread_manager.h"

auto main_server::start_service() -> fw::error
{
    load_feature();
    
    fw::error error_code{};
    
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

    return error_code;
}

auto main_server::load_feature() -> void
{
    // todo thread_manager make_unique + feature_list_t;
    return;
}

auto main_server::feature_setup() -> fw::error
{

    return fw::error();
}

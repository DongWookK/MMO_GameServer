#pragma once
#include "pch.h"
#include "main_server.h"
#include "logger.h"

int main()
{
	int32_t error_code{};

	error_code = initialize_logger();
	ASSERT_RETURN_VALUE(!(error_code), error_code);

	FLOG_INFO("main server::start_service");

    spdlog::shutdown();

	main_server::instance()->start_service();
	main_server::instance()->stop_service();

	FLOG_INFO("main server::graceful shutdown");

    spdlog::shutdown();

	return 0;
}

#pragma once
#include "pch.h"
#include "main_server.h"

int initialize_logger()
{
    // 콘솔 출력(Output) 인코딩을 UTF-8로 설정 (한글 옵션)
    SetConsoleOutputCP(CP_UTF8);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink-log.txt", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // 복합 로거(Multi-sink Logger) 생성
    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto combined_logger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());
    combined_logger->set_level(spdlog::level::trace);

    spdlog::set_default_logger(combined_logger);
    spdlog::debug("디버그 메시지");
	
	return 0;
}

int main()
{
	int32_t error_code{};

	error_code = initialize_logger();
	ASSERT_RETURN_VALUE(!(error_code), error_code);

    spdlog::info("main server start_service()");
	main_server::instance()->start_service();
	main_server::instance()->stop_service();

	return 0;
}

#pragma once
#include "pch.h"

// 현재 날짜와 시간을 문자열로 반환하는 함수 (예: 20260627_232600)
std::string get_current_datetime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm buf;
    // Windows 환경 안전한 localtime_s 사용 (Linux인 경우 localtime_r 사용)
    localtime_s(&buf, &in_time_t);

    std::stringstream ss;
    ss << std::put_time(&buf, "%Y%m%d_%H%M%S");
    return ss.str();
}

int initialize_logger()
{
    // 콘솔 출력(Output) 인코딩을 UTF-8로 설정 (한글 옵션)
    SetConsoleOutputCP(CP_UTF8);
    std::string filename = "logs/log_" + get_current_datetime() + ".txt";

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // 복합 로거(Multi-sink Logger) 생성
    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto combined_logger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());
    combined_logger->set_level(spdlog::level::trace);

    spdlog::set_default_logger(combined_logger);
    spdlog::debug("create default logger");

    fw::SetLogger(spdlog::default_logger());

    return 0;
}
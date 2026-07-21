#pragma once
#include "stdint.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

/*---------------------------------------------
Crash Define
---------------------------------------------*/
#define CRASH(cause)						\
{											\
	unsigned int* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))						\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

/*---------------------------------------------
For Code
---------------------------------------------*/
#ifdef NDEBUG
#define ASSERT_RETURN_VALUE(expr, error_code)            \
{                                                       \
    if (!(expr)) {                                      \
        spdlog::log(              \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
            spdlog::level::err,                         \
            "ASSERT Failed: ({})", #expr                \
        );                                              \
        return error_code;                              \
    }                                                   \
}
#else
#define ASSERT_RETURN_VALUE(expr, error_code)            \
{                                                       \
    if (!(expr)) {                                      \
        spdlog::log(              \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
            spdlog::level::err,                         \
            "ASSERT Failed: ({})", #expr                \
        );                                              \
        ASSERT_CRASH(expr);                             \
        return error_code;                              \
    }                                                   \
}
#endif

/*---------------------------------------------
For Log
---------------------------------------------*/
#define __FILE _CRT_WIDE(__FILE__) 
#define FILELINE __FILE, __LINE__

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#include <spdlog/spdlog.h>

#define FLOG_INFO(...) \
    spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
                spdlog::level::info, \
                __VA_ARGS__)

#define FLOG_WARN(...) \
    spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
                spdlog::level::warn, \
                __VA_ARGS__)

#define FLOG_ERROR(...) \
    spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
                spdlog::level::err, \
                __VA_ARGS__)

#define FLOG_CRITICAL(...) \
    spdlog::log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
                spdlog::level::critical, \
                __VA_ARGS__)


namespace fw {
    // Server에서 생성한 기본 로거를 FW의 spdlog 레지스트리에 공유 등록하는 함수
    inline void SetLogger(std::shared_ptr<spdlog::logger> logger) {
        if (logger) {
            spdlog::set_default_logger(logger);
        }
    }
}

namespace fw
{
	using error = int32_t;
}
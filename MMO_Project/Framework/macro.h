#pragma once
#include "stdint.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <flatbuffers/flatbuffers.h>

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

#if defined(_MSC_VER)
#define DEBUG_BREAK()                   \
    do {                                \
        if (::IsDebuggerPresent()) {    \
            __debugbreak();             \
        }                               \
    } while (0)
#endif

#define ASSERT_DEBUG_BREAK(expr)			\
{											\
	if (!(expr))							\
	{										\
		DEBUG_BREAK();						\
		__analysis_assume(expr);			\
	}										\
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
        ASSERT_DEBUG_BREAK(expr);                             \
        return error_code;                              \
    }                                                   \
}
#endif

#ifdef NDEBUG
#define ASSERT_RETURN(expr)            \
{                                                       \
    if (!(expr)) {                                      \
        spdlog::log(              \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
            spdlog::level::err,                         \
            "ASSERT Failed: ({})", #expr                \
        );                                              \
        return;                                         \
    }                                                   \
}
#else
#define ASSERT_RETURN(expr)            \
{                                                       \
    if (!(expr)) {                                      \
        spdlog::log(              \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
            spdlog::level::err,                         \
            "ASSERT Failed: ({})", #expr                \
        );                                              \
        ASSERT_DEBUG_BREAK(expr);                             \
        return;                                         \
    }                                                   \
}
#endif

#ifdef NDEBUG
#define ASSERT_RETURN_ERROR(expr, error_var, error_code)             \
{                                                                   \
    if (!(expr)) {                                                  \
        spdlog::log(                                                \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},\
            spdlog::level::err,                                     \
            "ASSERT Failed: ({})", #expr                            \
        );                                                          \
        (error_var) = (error_code);                                 \
        return (error_var);                                         \
    }                                                               \
}
#else
#define ASSERT_RETURN_ERROR(expr, error_var, error_code)             \
{                                                                   \
    if (!(expr)) {                                                  \
        spdlog::log(                                                \
            spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},\
            spdlog::level::err,                                     \
            "ASSERT Failed: ({})", #expr                            \
        );                                                          \
        DEBUG_BREAK();                                              \
        (error_var) = (error_code);                                 \
        return (error_var);                                         \
    }                                                               \
}
#endif

/*---------------------------------------------
Defer
---------------------------------------------*/
namespace fw {
    template <typename Fn>
    class scope_exit {
    public:
        explicit scope_exit(Fn&& fn) noexcept : fn_(std::move(fn)) {}
        ~scope_exit() noexcept
        {
            if (!active_) return;

            try { fn_(); }
            catch (const std::exception& e) { spdlog::error("defer failed : {}", e.what()); }
            catch (...) { spdlog::error("defer failed : unknown exception"); }
        }

        scope_exit(const scope_exit&) = delete;
        scope_exit& operator=(const scope_exit&) = delete;

        auto release() noexcept -> void { active_ = false; }

    private:
        Fn   fn_;
        bool active_ = true;
    };
}

#define FW_CONCAT_IMPL(a, b) a##b
#define FW_CONCAT(a, b) FW_CONCAT_IMPL(a, b)
#define DEFER(...) \
    auto FW_CONCAT(defer_, __LINE__) = fw::scope_exit([&]() { __VA_ARGS__; })

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
    inline void SetLogger(std::shared_ptr<spdlog::logger> logger) {
        if (logger) {
            spdlog::set_default_logger(logger);
        }
    }
}

namespace fw {
    template <typename Enum>
    constexpr auto to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }


    inline auto wstring_to_string(const std::wstring& wstr) -> std::string
    {
        if (wstr.empty()) return {};

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, nullptr, nullptr);

        return str;
    }

    inline auto string_to_wstring(const std::string& str) -> std::wstring
    {
        if (str.empty()) return {};

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);

        return wstr;
    }

    inline auto to_wstring(const flatbuffers::String* s) -> std::wstring
    {
        if (!s) return {};
        return fw::string_to_wstring(s->str());
    }

    inline auto create_string(flatbuffers::FlatBufferBuilder& builder, std::wstring_view ws)
        -> flatbuffers::Offset<flatbuffers::String>
    {
        return builder.CreateString(fw::wstring_to_string(std::wstring{ ws }));
    }
}
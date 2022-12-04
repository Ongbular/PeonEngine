#pragma once

#pragma warning( disable : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )
#include "spdlog/spdlog.h"
#pragma warning( default : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )

enum class eLogLevel : uint8_t
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
};

/**
 * @brief 런타임에 발생한 로그를 기록합니다. 디버그 모드에서는 콘솔창에도 로그를 기록합니다.
 */
class Logger final
{
public:
    /**
     * @brief 로깅할 파일을 생성하고 로거를 초기화합니다.
     * @details 디버그 모드에서는 콘솔창을 추가합니다.
     */
    static HRESULT InitializeLogger();

    static void PrintEngineLogo();

    static void PrintCallStack();

    /**
     * @brief 콘솔창이 열려있다면 해제합니다.
     */
    static void ReleaseConsole();

    template <typename... Args>
    static void Log(const eLogLevel level, spdlog::format_string_t<Args...> fmt, Args &&... args);

    template <typename... Args>
    static void Debug(spdlog::format_string_t<Args...> fmt, Args &&... args);

    template <typename... Args>
    static void Info(spdlog::format_string_t<Args...> fmt, Args &&... args);

    template <typename... Args>
    static void Warn(spdlog::format_string_t<Args...> fmt, Args &&... args);

    template <typename... Args>
    static void Error(spdlog::format_string_t<Args...> fmt, Args &&... args);

private:
    Logger() = default;
    ~Logger() = default;

    static constexpr std::string_view sDefaultLogPath{ EDITOR_LOG_PATH };
    static constexpr std::string_view sDefaultLogPrefix{ "Log_" };
    static constexpr std::string_view sDefaultLogExtension{ ".log" };

    static FILE* DummyFp;
};

template <typename ... Args>
void Logger::Log(const eLogLevel level, spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    switch (level)
    {
    case eLogLevel::Info:
        spdlog::info(fmt, args...);
        break;
    case eLogLevel::Debug:
        spdlog::debug(fmt, args...);
        break;
    case eLogLevel::Trace:
        spdlog::trace(fmt, args...);
        break;
    case eLogLevel::Warning:
        spdlog::warn(fmt, args...);
        break;
    case eLogLevel::Error:
        // Intentional fall through
    case eLogLevel::Critical:
        spdlog::error(fmt, args...);
        break;
    }
}

template <typename ... Args>
void Logger::Debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Logger::Log(eLogLevel::Debug, fmt, args...);
}

template <typename ... Args>
void Logger::Info(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Logger::Log(eLogLevel::Info, fmt, args...);
}

template <typename ... Args>
void Logger::Warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Logger::Log(eLogLevel::Warning, fmt, args...);
}

template <typename ... Args>
void Logger::Error(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
    Logger::Log(eLogLevel::Error, fmt, args...);
}

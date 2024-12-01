#pragma once

#include <chrono>
#include <filesystem>
#include <string>

namespace TimeUtils {

    enum struct TimeUnit { MICRO, MILLI, SECOND, MINUTE, HOUR, DAY, WEEK, YEAR };

    //%Y-%m-%d %H:%M:%S
    std::string DateTimeLocalToString(const std::chrono::system_clock::time_point& dateTime, const char* format = nullptr);
    //%Y-%m-%d %H:%M:%S
    std::string DateTimeLocalToString(const std::filesystem::file_time_type::clock::time_point& dateTime, const char* format = nullptr);
    //%Y-%m-%d %H:%M:%S
    std::string DateTimeUtcToString(const std::chrono::system_clock::time_point& dateTime, const char* format = nullptr);
    //%Y-%m-%d %H:%M:%S
    std::string DateTimeUtcToString(const std::filesystem::file_time_type::clock::time_point& dateTime, const char* format = nullptr);

    //%Y-%m-%d %H:%M:%S
    std::string TodayNowLocalToString(const char* format = nullptr);
    //%Y-%m-%d %H:%M:%S
    std::string TodayNowUtcToString(const char* format = nullptr);

    //%Y-%m-%d
    std::string DateLocalToString(const std::chrono::system_clock::time_point& date, const char* format = nullptr);
    //%Y-%m-%d
    std::string DateLocalToString(const std::filesystem::file_time_type::clock::time_point& date, const char* format = nullptr);
    //%Y-%m-%d
    std::string DateUtcToString(const std::chrono::system_clock::time_point& date, const char* format = nullptr);
    //%Y-%m-%d
    std::string DateUtcToString(const std::filesystem::file_time_type::clock::time_point& date, const char* format = nullptr);

    //%Y-%m-%d
    std::string TodayLocalToString(const char* format = nullptr);
    //%Y-%m-%d
    std::string TodayUtcToString(const char* format = nullptr);

    //%H:%M:%S
    std::string TimeLocalToString(const std::chrono::system_clock::time_point& time, const char* format = nullptr);
    //%H:%M:%S
    std::string TimeLocalToString(const std::filesystem::file_time_type::clock::time_point& time, const char* format = nullptr);
    //%H:%M:%S
    std::string TimeUtcToString(const std::chrono::system_clock::time_point& time, const char* format = nullptr);
    //%H:%M:%S
    std::string TimeUtcToString(const std::filesystem::file_time_type::clock::time_point& time, const char* format = nullptr);

    //%H:%M:%S
    std::string NowLocalToString(const char* format = nullptr);
    //%H:%M:%S
    std::string NowUtcToString(const char* format = nullptr);


    std::chrono::system_clock::time_point StringToTimePoint(const std::string& timeString, const std::string& format);

    std::chrono::system_clock::time_point FileTimeToSystemTime(const std::filesystem::file_time_type& fileTime);
    std::filesystem::file_time_type SystemTimeToFileTime(const std::chrono::system_clock::time_point& sysTime);

    //

    std::string DurationToString(const std::chrono::microseconds& duration, TimeUnit minTimeUnit = TimeUnit::MICRO);

} // namespace TimeUtils

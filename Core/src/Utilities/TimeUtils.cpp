#include "Core/Utilities/TimeUtils.h"

#include "Core/Utilities/Format.h"
#include "Core/Utilities/StringUtils.h"

#include <functional>
#include <iomanip>
#include <sstream>
#include <format>
#include <filesystem>

using c = std::chrono::system_clock;
using tp = c::time_point;
using ft = std::filesystem::file_time_type;

namespace {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr char DefaultDateFormat[]{"%Y-%m-%d"};
    constexpr char DefaultDateTimeFormat[]{"%Y-%m-%d %H:%M:%S"};
    constexpr char DefaultTimeFormat[]{"%H:%M:%S"};

    std::string TimePointToString(const tp& time, const char* format, std::function<errno_t(tm*, const time_t*)> convertFunc) {
        std::stringstream ss;

        auto ts = c::to_time_t(time);
        tm theTime;
        convertFunc(&theTime, &ts);

        ss << std::put_time(&theTime, format);
        return ss.str();
    }

    std::string TimePointLocalToString(const tp& time, const char* format) {
        return TimePointToString(time, format, _localtime64_s);
    }
    std::string TimePointLocalToString(const ft& time, const char* format) {
        return TimePointLocalToString(TimeUtils::FileTimeToSystemTime(time), format);
    }

    std::string TimePointUtcToString(const tp& time, const char* format) {
        return TimePointToString(time, format, _gmtime64_s);
    }
    std::string TimePointUtcToString(const ft& time, const char* format) {
        return TimePointUtcToString(TimeUtils::FileTimeToSystemTime(time), format);
    }

    std::string DurationToStringImpl(bool negative, f64 elapsed, const char* unit) {
        if(negative) {
            elapsed *= -1.0;
        }
        return StrUtil::Trim(StrUtil::Format("%.2f %s", elapsed, unit));
    }
} // namespace

namespace TimeUtils {
    std::string DateTimeLocalToString(const tp& dateTime, const char* format) {
		return TimePointLocalToString(dateTime, format ? format : DefaultDateTimeFormat);
    }
	std::string DateTimeLocalToString(const ft& dateTime, const char* format) {
		return TimePointLocalToString(dateTime, format ? format : DefaultDateTimeFormat);
	}
	std::string DateTimeUtcToString(const tp& dateTime, const char* format) {
		return TimePointUtcToString(dateTime, format ? format : DefaultDateTimeFormat);
	}
	std::string DateTimeUtcToString(const ft& dateTime, const char* format) {
		return TimePointUtcToString(dateTime, format ? format : DefaultDateTimeFormat);
    }

	std::string TodayNowLocalToString(const char* format) {
		return TimePointLocalToString(c::now(), format ? format : DefaultDateTimeFormat);
	}
	std::string TodayNowUtcToString(const char* format) {
		return TimePointUtcToString(c::now(), format ? format : DefaultDateTimeFormat);
    }

	std::string DateLocalToString(const tp& date, const char* format) {
		return TimePointLocalToString(date, format ? format : DefaultDateFormat);
	}
	std::string DateLocalToString(const ft& date, const char* format) {
		return TimePointLocalToString(date, format ? format : DefaultDateFormat);
    }
	std::string DateUtcToString(const tp& date, const char* format) {
		return TimePointUtcToString(date, format ? format : DefaultDateFormat);
    }
	std::string DateUtcToString(const ft& date, const char* format) {
		return TimePointUtcToString(date, format ? format : DefaultDateFormat);
	}

	std::string TodayLocalToString(const char* format) {
		return TimePointLocalToString(c::now(), format ? format : DefaultDateFormat);
	}
	std::string TodayUtcToString(const char* format) {
		return TimePointUtcToString(c::now(), format ? format : DefaultDateFormat);
    }

	std::string TimeLocalToString(const tp& time, const char* format) {
		return TimePointLocalToString(time, format ? format : DefaultTimeFormat);
	}
	std::string TimeLocalToString(const ft& time, const char* format) {
		return TimePointLocalToString(time, format ? format : DefaultTimeFormat);
    }
	std::string TimeUtcToString(const tp& time, const char* format) {
		return TimePointUtcToString(time, format ? format : DefaultTimeFormat);
    }
	std::string TimeUtcToString(const ft& time, const char* format) {
		return TimePointUtcToString(time, format ? format : DefaultTimeFormat);
    }

	std::string NowLocalToString(const char* format) {
		return TimePointLocalToString(c::now(), format ? format : DefaultTimeFormat);
	}
	std::string NowUtcToString(const char* format) {
		return TimePointUtcToString(c::now(), format ? format : DefaultTimeFormat);
    }

    std::string DurationToString(const std::chrono::microseconds& duration, TimeUnit minUnit) {
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        bool negative = microseconds < 0;
        microseconds = std::abs(microseconds);

        TimeUnit timeUnit = minUnit;

        if(duration < 1ms) timeUnit = std::max(timeUnit, TimeUnit::MICRO);
        else if(duration < 1s) timeUnit = std::max(timeUnit, TimeUnit::MILLI);
        else if(duration < 1min) timeUnit = std::max(timeUnit, TimeUnit::SECOND);
		else if (duration < 1h) timeUnit = std::max(timeUnit, TimeUnit::MINUTE);
		else if (duration < 24h) timeUnit = std::max(timeUnit, TimeUnit::HOUR);
		else if (duration < 24h * 7) timeUnit = std::max(timeUnit, TimeUnit::DAY);
		else if (duration < 24h * 365) timeUnit = std::max(timeUnit, TimeUnit::WEEK);
		else timeUnit = TimeUnit::YEAR;
        /*
        if(microseconds < MinMillis) {
            timeUnit = std::max(timeUnit, TimeUnit::MICRO);
        } else if(microseconds < MinSeconds) {
            timeUnit = std::max(timeUnit, TimeUnit::MILLI);
        } else if(microseconds < MinMinutes) {
            timeUnit = std::max(timeUnit, TimeUnit::SECOND);
        } else if(microseconds < MinHours) {
            timeUnit = std::max(timeUnit, TimeUnit::MINUTE);
        } else if(microseconds < MinDays) {
            timeUnit = std::max(timeUnit, TimeUnit::HOUR);
        } else if(microseconds < MinWeeks) {
            timeUnit = std::max(timeUnit, TimeUnit::DAY);
        } else if(microseconds < MinYears) {
            timeUnit = std::max(timeUnit, TimeUnit::WEEK);
        } else {
            timeUnit = TimeUnit::YEAR;
        }
        */

        switch(timeUnit) {
            using enum TimeUnit;
        case MICRO: return DurationToStringImpl(negative, static_cast<f64>(duration.count()), "us");
        case MILLI: return DurationToStringImpl(negative, duration / 1.0ms, "ms");
		case SECOND: return DurationToStringImpl(negative, duration / 1.0s, "s");
		case MINUTE: return DurationToStringImpl(negative, duration / 1.0min, "m");
		case HOUR: return DurationToStringImpl(negative, duration / 1.0h, "h");
		case DAY: return DurationToStringImpl(negative, duration / 24.0h, "days");
		case WEEK: return DurationToStringImpl(negative, duration / 168.0h, "weeks");
        case YEAR: return DurationToStringImpl(negative, duration / 24.0h / 365.0, "years");
        default: return "Invalid Time Unit";
        }
        /*
		DurationToStringImpl(negative, duration_cast<milliseconds>(duration).count() / 100.0, "milliseconds");
        switch(timeUnit) {
        case TimeUnit::MICRO: return DurationToStringImpl(negative, microseconds * 1.0, "microseconds");
        case TimeUnit::MILLI: return DurationToStringImpl(negative, microseconds * MicroToMilli, "milliseconds");
        case TimeUnit::SECOND: return DurationToStringImpl(negative, microseconds * MicroToSecond, "seconds");
        case TimeUnit::MINUTE: return DurationToStringImpl(negative, microseconds * MicroToMinute, "minutes");
        case TimeUnit::HOUR: return DurationToStringImpl(negative, microseconds * MicroToHour, "hours");
        case TimeUnit::DAY: return DurationToStringImpl(negative, microseconds * MicroToDay, "days");
        case TimeUnit::WEEK: return DurationToStringImpl(negative, microseconds * MicroToWeek, "weeks");
        case TimeUnit::YEAR: return DurationToStringImpl(negative, microseconds * MicroToYear, "years");
        default: return "Invalid time unit.";
        */
        //};
    }

    tp StringToTimePoint(const std::string& timeString, const std::string& format) {
        std::tm tm = {};
        std::stringstream ss(timeString);
        ss >> std::get_time(&tm, format.c_str());
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    tp FileTimeToSystemTime(const ft& fileTime) {
        auto fileNow = ft::clock::now();
        auto sysNow = c::now();

        return sysNow - (fileNow - fileTime);
    }

    ft SystemTimeToFileTime(const tp& sysTime) {
        auto fileNow = ft::clock::now();
        auto sysNow = c::now();

        return fileNow - (sysNow - sysTime);
    }

} // namespace TimeUtils
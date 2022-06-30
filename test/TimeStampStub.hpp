#ifndef AV_SPEECH_IN_NOISE_TESTS_TIMESTAMPSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_TIMESTAMPSTUB_HPP_

#include "LogString.hpp"

#include <av-speech-in-noise/core/FileSystemPath.hpp>

#include <sstream>

namespace av_speech_in_noise {
class TimeStampStub : public TimeStamp {
  public:
    void capture() override { insert(log_, "capture "); }

    auto log() const -> const std::stringstream & { return log_; }

    void setYear(int y) { year_ = y; }

    void setMonth(int y) { month_ = y; }

    void setDayOfMonth(int y) { dayOfMonth_ = y; }

    void setHour(int y) { hour_ = y; }

    void setMinute(int y) { minute_ = y; }

    void setSecond(int y) { second_ = y; }

    auto year() -> int override {
        insert(log_, "year ");
        return year_;
    }

    auto month() -> int override {
        insert(log_, "month ");
        return month_;
    }

    auto dayOfMonth() -> int override {
        insert(log_, "dayOfMonth ");
        return dayOfMonth_;
    }

    auto hour() -> int override {
        insert(log_, "hour ");
        return hour_;
    }

    auto minute() -> int override {
        insert(log_, "minute ");
        return minute_;
    }

    auto second() -> int override {
        insert(log_, "second ");
        return second_;
    }

  private:
    std::stringstream log_{};
    int year_{};
    int month_{};
    int dayOfMonth_{};
    int hour_{};
    int minute_{};
    int second_{};
};
}

#endif

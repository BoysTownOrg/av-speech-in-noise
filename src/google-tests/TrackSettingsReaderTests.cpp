#include <recognition-test/AdaptiveMethod.hpp>

namespace av_speech_in_noise {
    class TextFileReader {
    public:
        virtual ~TextFileReader() = default;
        virtual std::string read(std::string) = 0;
    };

    class TrackSettingsReader : public ITrackSettingsReader {
        TextFileReader *reader;
    public:
        explicit TrackSettingsReader(TextFileReader *reader) :
            reader{reader} {}
        
        const TrackingRule *read(std::string filePath) override {
            reader->read(std::move(filePath));
            return {};
        }
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class TextFileReaderStub : public TextFileReader {
        std::string filePath_{};
    public:
        auto filePath() const {
            return filePath_;
        }
        
        std::string read(std::string s) override {
            filePath_ = std::move(s);
            return {};
        }
    };
    
    class TrackSettingsReaderTests : public ::testing::Test {
    
    };
    
    TEST_F(TrackSettingsReaderTests, tbd) {
        TextFileReaderStub fileReader;
        TrackSettingsReader reader{&fileReader};
        reader.read("a");
        assertEqual("a", fileReader.filePath());
    }
}

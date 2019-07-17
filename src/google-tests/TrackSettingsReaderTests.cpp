#include "assert-utility.h"
#include <recognition-test/TrackSettingsReader.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class TextFileReaderStub : public TextFileReader {
        std::string filePath_{};
        std::string contents_{};
    public:
        auto filePath() const {
            return filePath_;
        }
        
        std::string read(std::string s) override {
            filePath_ = std::move(s);
            return contents_;
        }
        
        void setContents(std::string s) {
            contents_ = std::move(s);
        }
    };
    
    class TrackSettingsInterpreterStub : public ITrackSettingsInterpreter {
        std::string content_{};
        const TrackingRule *trackingRule_{};
    public:
        const TrackingRule *trackingRule(std::string s) override {
            content_ = std::move(s);
            return trackingRule_;
        }
        
        auto content() const {
            return content_;
        }
        
        void setTrackingRule(const TrackingRule *r) {
            trackingRule_ = r;
        }
    };
    
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        TextFileReaderStub fileReader;
        TrackSettingsInterpreterStub interpreter;
        TrackSettingsReader reader{&fileReader, &interpreter};
        
        auto read(std::string s = {}) {
            return reader.read(std::move(s));
        }
    };
    
    TEST_F(TrackSettingsReaderTests, readPassesFilePathToFileReader) {
        read("a");
        assertEqual("a", fileReader.filePath());
    }
    
    TEST_F(TrackSettingsReaderTests, readPassesContentsToInterpreter) {
        fileReader.setContents("a");
        read();
        assertEqual("a", interpreter.content());
    }
    
    TEST_F(TrackSettingsReaderTests, readReturnsInterpretedResult) {
        TrackingRule trackingRule;
        interpreter.setTrackingRule(&trackingRule);
        assertEqual(&std::as_const(trackingRule), read());
    }
}

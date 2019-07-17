#include <recognition-test/AdaptiveMethod.hpp>

namespace av_speech_in_noise {
    class TextFileReader {
    public:
        virtual ~TextFileReader() = default;
        virtual std::string read(std::string) = 0;
    };
    
    class ITrackSettingsInterpreter {
    public:
        virtual ~ITrackSettingsInterpreter() = default;
        virtual const TrackingRule *trackingRule(std::string) = 0;
    };

    class TrackSettingsReader : public ITrackSettingsReader {
        TextFileReader *reader;
        ITrackSettingsInterpreter *interpreter;
    public:
        TrackSettingsReader(
            TextFileReader *reader,
            ITrackSettingsInterpreter *interpreter
        ) :
            reader{reader},
            interpreter{interpreter} {}
        
        const TrackingRule *read(std::string filePath) override {
            return interpreter->trackingRule(reader->read(std::move(filePath)));
        }
    };
}

#include "assert-utility.h"
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

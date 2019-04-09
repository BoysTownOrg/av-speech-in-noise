#include "assert-utility.h"
#include <stimulus-players/AudioReader.hpp>
#include <gtest/gtest.h>

namespace {
    class AudioFileReaderStub : public stimulus_players::AudioFileReader {
        std::string file_{};
        bool failOnLoad_{};
        bool failed_{};
    public:
        void failOnLoad() {
            failOnLoad_ = true;
        }
        
        auto file() const {
            return file_;
        }
        
        void loadFile(std::string s) override {
            file_ = std::move(s);
            if (failOnLoad_)
                failed_ = true;
        }
        
        bool failed() override {
            return failed_;
        }
    };

    class AudioReaderTests : public ::testing::Test {
    protected:
        AudioFileReaderStub fileReader{};
        stimulus_players::AudioReader reader{&fileReader};
    };
    
    TEST_F(AudioReaderTests, loadsFile) {
        reader.loadFile("a");
        assertEqual("a", fileReader.file());
    }
    
    TEST_F(AudioReaderTests, throwsInvalidFileOnFailure) {
        fileReader.failOnLoad();
        try {
            reader.loadFile({});
            FAIL() << "Expected stimulus_players::AudioReader::InvalidFile";
        } catch (const stimulus_players::AudioReader::InvalidFile &) {
        }
    }
}

#include "assert-utility.h"
#include <stimulus-players/AudioReader.hpp>
#include <gtest/gtest.h>

namespace {
    class AudioFileReaderStub : public stimulus_players::AudioFileReader {
        std::string file_{};
    public:
        auto file() const {
            return file_;
        }
        
        void loadFile(std::string s) override {
            file_ = std::move(s);
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
}

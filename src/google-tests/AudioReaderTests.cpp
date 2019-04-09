#include "assert-utility.h"
#include <stimulus-players/AudioReader.hpp>
#include <gtest/gtest.h>

namespace {
    class AudioFileReaderStub : public stimulus_players::AudioFileReader {
        std::vector<std::vector<int>> buffers{};
        std::string file_{};
        int minimumPossibleSample_{};
        bool failOnLoad_{};
        bool failed_{};
    public:
        void failOnLoad() {
            failOnLoad_ = true;
        }
        
        auto file() const {
            return file_;
        }
        
        void setBuffers(std::vector<std::vector<int>> b) {
            buffers = std::move(b);
        }
        
        std::vector<int> readNextBuffer() override {
            if (buffers.size() == 0)
                return {};
            
            auto buffer = buffers.front();
            buffers.erase(buffers.begin());
            return buffer;
        }
        
        void loadFile(std::string s) override {
            file_ = std::move(s);
            if (failOnLoad_)
                failed_ = true;
        }
        
        bool failed() override {
            return failed_;
        }
        
        int minimumPossibleSample() override {
            return minimumPossibleSample_;
        }
        
        void setMinimumPossibleSample(int x) {
            minimumPossibleSample_ = x;
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
    
    TEST_F(AudioReaderTests, concatenatesNormalizedBuffers) {
        fileReader.setBuffers({
            { 0, 1, 2 },
            { -1, -2, -3 }
        });
        fileReader.setMinimumPossibleSample(-3);
        assertEqual({ 0.f/-3, 1.f/-3, 2.f/-3, -1.f/-3, -2.f/-3, -3.f/-3 }, reader.read());
    }
}

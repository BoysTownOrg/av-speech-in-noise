#include "assert-utility.h"
#include <stimulus-players/AudioReaderImpl.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace {
    class AudioBufferStub : public stimulus_players::AudioBuffer {
        std::vector<std::vector<int>> audio_{};
    public:
        void setAudio(std::vector<std::vector<int>> x) {
            audio_ = std::move(x);
        }
        
        int channels() override {
            return gsl::narrow<int>(audio_.size());
        }
        
        std::vector<int> channel(int i) override {
            return audio_.at(i);
        }
        
        bool empty() override {
            return audio_.empty();
        }
    };
    
    class BufferedAudioReaderStub : public stimulus_players::BufferedAudioReader {
        std::vector<std::vector<std::vector<int>>> buffersPartTwo_{};
        AudioBufferStub buffer{};
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
        
        stimulus_players::AudioBuffer *readNextBuffer() override {
            std::vector<std::vector<int>> next{};
            if (buffersPartTwo_.size()) {
                next = buffersPartTwo_.front();
                buffersPartTwo_.erase(buffersPartTwo_.begin());
            }
            buffer.setAudio(next);
            return &buffer;
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
        
        void setBuffers(std::vector<std::vector<std::vector<int>>> x) {
            buffersPartTwo_ = std::move(x);
        }
    };

    class AudioReaderTests : public ::testing::Test {
    protected:
        BufferedAudioReaderStub bufferedReader{};
        stimulus_players::AudioReaderImpl reader{&bufferedReader};
    };
    
    TEST_F(AudioReaderTests, loadsFile) {
        reader.read("a");
        assertEqual("a", bufferedReader.file());
    }
    
    TEST_F(AudioReaderTests, readThrowsInvalidFileOnFailure) {
        bufferedReader.failOnLoad();
        try {
            reader.read({});
            FAIL() << "Expected stimulus_players::AudioReader::InvalidFile";
        } catch (const stimulus_players::AudioReaderImpl::InvalidFile &) {
        }
    }
    
    TEST_F(AudioReaderTests, readConcatenatesNormalizedBuffersPartTwo) {
        bufferedReader.setBuffers({{
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        }, {
            { 10, 11, 12 },
            { 13, 14, 15 },
            { 16, 17, 18 }
        }, {
            { 19, 20, 21 },
            { 22, 23, 24 },
            { 25, 26, 27 }
        }});
        bufferedReader.setMinimumPossibleSample(-3);
        std::vector<std::vector<float>> x = {
            { 1.f/-3, 2.f/-3, 3.f/-3, 10.f/-3, 11.f/-3, 12.f/-3, 19.f/-3, 20.f/-3, 21.f/-3 },
            { 4.f/-3, 5.f/-3, 6.f/-3, 13.f/-3, 14.f/-3, 15.f/-3, 22.f/-3, 23.f/-3, 24.f/-3 },
            { 7.f/-3, 8.f/-3, 9.f/-3, 16.f/-3, 17.f/-3, 18.f/-3, 25.f/-3, 26.f/-3, 27.f/-3 }
        };
        assertEqual(x,
            reader.read({})
        );
    }
}

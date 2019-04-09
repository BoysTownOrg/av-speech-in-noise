#include <string>

class AudioFileReader {
public:
    virtual ~AudioFileReader() = default;
    virtual void loadFile(std::string) = 0;
};

class AudioReader {
    AudioFileReader *reader;
public:
    AudioReader(AudioFileReader *reader) : reader{reader} {}
    
    void loadFile(std::string filePath) {
        reader->loadFile(std::move(filePath));
    }
};

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class AudioFileReaderStub : public AudioFileReader {
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
        AudioReader reader{&fileReader};
    };
    
    TEST_F(AudioReaderTests, loadsFile) {
        reader.loadFile("a");
        assertEqual("a", fileReader.file());
    }
}

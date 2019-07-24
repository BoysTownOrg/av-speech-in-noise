#ifndef AudioReaderStub_h
#define AudioReaderStub_h

#include <stimulus-players/AudioReader.h>
#include <string>

namespace stimulus_players::tests {
    class AudioReaderStub : public AudioReader {
        std::vector<std::vector<float>> toRead_{};
        std::string filePath_{};
        bool throwOnRead_{};
    public:
        void set(std::vector<std::vector<float>> x) {
            toRead_ = std::move(x);
        }
        std::vector<std::vector<float> > read(std::string filePath) override {
            filePath_ = std::move(filePath);
            if (throwOnRead_)
                throw InvalidFile{};
            return toRead_;
        }
        auto filePath() const {
            return filePath_;
        }
        void throwOnRead() {
            throwOnRead_ = true;
        }
    };
}
#endif

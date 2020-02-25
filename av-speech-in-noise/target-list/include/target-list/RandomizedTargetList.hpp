#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_

#include "SubdirectoryTargetListReader.hpp"
#include <recognition-test/TargetList.hpp>
#include <vector>
#include <string>
#include <memory>

namespace target_list {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    using shuffle_iterator = std::vector<std::string>::iterator;
    virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
    using int_shuffle_iterator = std::vector<int>::iterator;
    virtual void shuffle(
        int_shuffle_iterator begin, int_shuffle_iterator end) = 0;
};

class RandomizedTargetListWithReplacement
    : public av_speech_in_noise::TargetList {
    std::vector<std::string> files{};
    std::string directory_{};
    std::string currentFile_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
    bool noFilesGotten{};

  public:
    RandomizedTargetListWithReplacement(DirectoryReader *, Randomizer *);
    void loadFromDirectory(std::string) override;
    auto next() -> std::string override;
    auto current() -> std::string override;
    auto empty() -> bool override;
    void reinsertCurrent() override;

  private:
    auto fullPath(std::string file) -> std::string;
    auto empty_() -> bool;
    void replaceLastFile();
};

class RandomizedTargetListWithReplacementFactory : public TargetListFactory {
    DirectoryReader *reader;
    Randomizer *randomizer;

  public:
    RandomizedTargetListWithReplacementFactory(
        DirectoryReader *reader, Randomizer *randomizer)
        : reader{reader}, randomizer{randomizer} {}

    auto make() -> std::shared_ptr<av_speech_in_noise::TargetList> override {
        return std::make_shared<RandomizedTargetListWithReplacement>(
            reader, randomizer);
    }
};

class RandomizedTargetListWithoutReplacement
    : public av_speech_in_noise::TargetList {
    std::vector<std::string> files{};
    std::string directory_{};
    std::string currentFile_{};
    DirectoryReader *reader;
    Randomizer *randomizer;

  public:
    RandomizedTargetListWithoutReplacement(DirectoryReader *, Randomizer *);
    auto empty() -> bool override;
    void loadFromDirectory(std::string directory) override;
    auto next() -> std::string override;
    auto current() -> std::string override;
    void reinsertCurrent() override;

  private:
    auto empty_() -> bool;
    auto fullPath(std::string file) -> std::string;
};

class CyclicRandomizedTargetListWithoutReplacement
    : public av_speech_in_noise::TargetList {
  public:
    CyclicRandomizedTargetListWithoutReplacement(
        DirectoryReader *, Randomizer *);
    auto empty() -> bool override { return {}; }
    void loadFromDirectory(std::string directory) override;
    auto next() -> std::string override;
    auto current() -> std::string override;
    void reinsertCurrent() override {}

  private:
    std::vector<std::string> files{};
    std::string directory_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
};
}

#endif

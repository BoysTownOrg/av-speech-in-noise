#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_

#include "SubdirectoryTargetListReader.hpp"
#include <recognition-test/TargetList.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>
#include <memory>

namespace target_list {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual void shuffle(gsl::span<std::string>) = 0;
    virtual void shuffle(gsl::span<int>) = 0;
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

  private:
    auto fullPath(std::string file) -> std::string;
    auto empty_() -> bool;
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
    : public av_speech_in_noise::FiniteTargetList {
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

class CyclicRandomizedTargetList : public av_speech_in_noise::TargetList {
  public:
    CyclicRandomizedTargetList(DirectoryReader *, Randomizer *);
    void loadFromDirectory(std::string directory) override;
    auto next() -> std::string override;
    auto current() -> std::string override;

  private:
    std::vector<std::string> files{};
    std::string directory_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
};
}

#endif

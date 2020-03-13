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
  public:
    class Factory : public TargetListFactory {
      public:
        Factory(DirectoryReader *reader, Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make()
            -> std::shared_ptr<av_speech_in_noise::TargetList> override {
            return std::make_shared<RandomizedTargetListWithReplacement>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        Randomizer *randomizer;
    };

    RandomizedTargetListWithReplacement(DirectoryReader *, Randomizer *);
    void loadFromDirectory(std::string) override;
    auto next() -> std::string override;
    auto current() -> std::string override;

  private:
    std::vector<std::string> files{};
    std::string directory{};
    std::string currentFile{};
    DirectoryReader *reader;
    Randomizer *randomizer;
};

class RandomizedTargetListWithoutReplacement
    : public av_speech_in_noise::FiniteTargetList {
  public:
    RandomizedTargetListWithoutReplacement(DirectoryReader *, Randomizer *);
    auto empty() -> bool override;
    void loadFromDirectory(std::string directory) override;
    auto next() -> std::string override;
    auto current() -> std::string override;
    void reinsertCurrent() override;

  private:
    std::vector<std::string> files{};
    std::string directory{};
    std::string currentFile{};
    DirectoryReader *reader;
    Randomizer *randomizer;
};

class CyclicRandomizedTargetList : public av_speech_in_noise::TargetList {
  public:
    class Factory : public TargetListFactory {
      public:
        Factory(DirectoryReader *reader, Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make()
            -> std::shared_ptr<av_speech_in_noise::TargetList> override {
            return std::make_shared<CyclicRandomizedTargetList>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        Randomizer *randomizer;
    };

    CyclicRandomizedTargetList(DirectoryReader *, Randomizer *);
    void loadFromDirectory(std::string directory) override;
    auto next() -> std::string override;
    auto current() -> std::string override;

  private:
    std::vector<std::string> files{};
    std::string directory{};
    DirectoryReader *reader;
    Randomizer *randomizer;
};
}

#endif

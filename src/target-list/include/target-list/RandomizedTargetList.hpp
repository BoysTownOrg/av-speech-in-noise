#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_RANDOMIZEDTARGETLIST_HPP_

#include "SubdirectoryTargetListReader.hpp"
#include <recognition-test/TargetList.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>
#include <memory>

namespace av_speech_in_noise {
namespace target_list {
class Randomizer {
  public:
    virtual ~Randomizer() = default;
    virtual void shuffle(gsl::span<av_speech_in_noise::LocalUrl>) = 0;
    virtual void shuffle(gsl::span<int>) = 0;
};
}

class RandomizedTargetListWithReplacement
    : public av_speech_in_noise::TargetList {
  public:
    class Factory : public TargetListFactory {
      public:
        Factory(DirectoryReader *reader, target_list::Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make()
            -> std::shared_ptr<av_speech_in_noise::TargetList> override {
            return std::make_shared<RandomizedTargetListWithReplacement>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        target_list::Randomizer *randomizer;
    };

    RandomizedTargetListWithReplacement(
        DirectoryReader *, target_list::Randomizer *);
    void loadFromDirectory(const av_speech_in_noise::LocalUrl &) override;
    auto next() -> av_speech_in_noise::LocalUrl override;
    auto current() -> av_speech_in_noise::LocalUrl override;
    auto directory() -> av_speech_in_noise::LocalUrl override;

  private:
    LocalUrls files{};
    av_speech_in_noise::LocalUrl directory_{};
    av_speech_in_noise::LocalUrl currentFile{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};

class RandomizedTargetListWithoutReplacement
    : public av_speech_in_noise::FiniteTargetList {
  public:
    RandomizedTargetListWithoutReplacement(
        DirectoryReader *, target_list::Randomizer *);
    auto empty() -> bool override;
    void loadFromDirectory(
        const av_speech_in_noise::LocalUrl &directory) override;
    auto next() -> av_speech_in_noise::LocalUrl override;
    auto current() -> av_speech_in_noise::LocalUrl override;
    void reinsertCurrent() override;
    auto directory() -> av_speech_in_noise::LocalUrl override;

  private:
    LocalUrls files{};
    av_speech_in_noise::LocalUrl directory_{};
    av_speech_in_noise::LocalUrl currentFile{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};

class CyclicRandomizedTargetList : public av_speech_in_noise::TargetList {
  public:
    class Factory : public TargetListFactory {
      public:
        Factory(DirectoryReader *reader, target_list::Randomizer *randomizer)
            : reader{reader}, randomizer{randomizer} {}

        auto make()
            -> std::shared_ptr<av_speech_in_noise::TargetList> override {
            return std::make_shared<CyclicRandomizedTargetList>(
                reader, randomizer);
        }

      private:
        DirectoryReader *reader;
        target_list::Randomizer *randomizer;
    };

    CyclicRandomizedTargetList(DirectoryReader *, target_list::Randomizer *);
    void loadFromDirectory(
        const av_speech_in_noise::LocalUrl &directory) override;
    auto next() -> av_speech_in_noise::LocalUrl override;
    auto current() -> av_speech_in_noise::LocalUrl override;
    auto directory() -> av_speech_in_noise::LocalUrl override;

  private:
    LocalUrls files{};
    av_speech_in_noise::LocalUrl directory_{};
    DirectoryReader *reader;
    target_list::Randomizer *randomizer;
};
}

#endif

#include "MersenneTwisterRandomizer.h"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include <presentation/Presenter.h>
#include <recognition-test/RecognitionTestModel.hpp>
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <stimulus-list/RandomizedStimulusList.hpp>
#include <stimulus-list/FileFilterDecorator.hpp>

class MacOsDirectoryReader : public stimulus_list::DirectoryReader {
    std::vector<std::string> filesIn(std::string directory) override {
        std::vector<std::string> files{};
        const auto path = [NSString stringWithCString:
            directory.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        const auto contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:
            path
            error: nil
        ];
        for (id thing in contents)
            files.push_back([thing UTF8String]);
        return files;
    }
};

int main() {
    MacOsDirectoryReader reader;
    stimulus_list::FileFilterDecorator filter{&reader, ".mov"};
    MersenneTwisterRandomizer randomizer;
    stimulus_list::RandomizedStimulusList list{&filter, &randomizer};
    AvFoundationStimulusPlayer stimulusPlayer;
    AvFoundationAudioPlayer audioPlayer;
    masker_player::RandomizedMaskerPlayer maskerPlayer{&audioPlayer};
    maskerPlayer.setFadeInOutSeconds(0.5);
    recognition_test::RecognitionTestModel model{&maskerPlayer, &list, &stimulusPlayer};
    CocoaView view;
    presentation::Presenter presenter{&model, &view};
    presenter.run();
}

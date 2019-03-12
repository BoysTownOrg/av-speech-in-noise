#include "MersenneTwisterRandomizer.h"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include <presentation/Presenter.h>
#include <recognition-test/Model.hpp>
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
    AvFoundationMaskerPlayer maskerPlayer;
    MacOsDirectoryReader reader;
    stimulus_list::FileFilterDecorator filter{&reader, ".mov"};
    MersenneTwisterRandomizer randomizer;
    stimulus_list::RandomizedStimulusList list{&filter, &randomizer};
    CocoaSubjectView subjectView{};
    AvFoundationStimulusPlayer player{};
    recognition_test::Model model{&maskerPlayer, &list, &player};
    CocoaView view;
    presentation::Presenter presenter{&model, &view};
    presenter.run();
}

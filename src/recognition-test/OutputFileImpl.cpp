#include "OutputFileImpl.hpp"
#include <sstream>

namespace recognition_test {
    namespace {
        class TrialStream {
            std::stringstream stream;
        public:
            template<typename T>
            void insert(T item) {
                stream << item;
            }
            
            void insertCommaAndSpace() {
                stream << ", ";
            }
            
            void insertNewLine() {
                stream << "\n";
            }
            
            auto str() const {
                return stream.str();
            }
        };
    }
    
    OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path) :
        writer{writer},
        path{path} {}

    void OutputFileImpl::writeTrial(
        const av_coordinated_response_measure::Trial &trial
    ) {
        TrialStream stream;
        stream.insert(trial.SNR_dB);
        stream.insertCommaAndSpace();
        stream.insert(trial.correctNumber);
        stream.insertCommaAndSpace();
        stream.insert(trial.subjectNumber);
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.correctColor));
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(trial.reversals);
        stream.insertNewLine();
        writer->write(stream.str());
    }

    std::string OutputFileImpl::colorName(av_coordinated_response_measure::Color c) {
        switch (c) {
        case av_coordinated_response_measure::Color::green:
            return "green";
        case av_coordinated_response_measure::Color::red:
            return "red";
        }
        return "unknown";
    }
    
    void OutputFileImpl::writeTrialHeading() {
        TrialStream stream;
        stream.insert("SNR (dB)");
        stream.insertCommaAndSpace();
        stream.insert("correct number");
        stream.insertCommaAndSpace();
        stream.insert("subject number");
        stream.insertCommaAndSpace();
        stream.insert("correct color");
        stream.insertCommaAndSpace();
        stream.insert("subject color");
        stream.insertCommaAndSpace();
        stream.insert("reversals");
        stream.insertNewLine();
        writer->write(stream.str());
    }
    
    void OutputFileImpl::open(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        auto fileName = path->generateFileName({});
        auto homeDirectory = path->homeDirectory();
        writer->open(homeDirectory + "/" + fileName + ".txt");
        if (writer->failed())
            throw OpenFailure{};
    }
}

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
        write(formatTrial(trial));
    }
    
    void OutputFileImpl::write(std::string s) {
        writer->write(std::move(s));
    }

    std::string OutputFileImpl::formatTrial(
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
        return stream.str();
    }

    std::string OutputFileImpl::colorName(av_coordinated_response_measure::Color c) {
        switch (c) {
        case av_coordinated_response_measure::Color::green:
            return "green";
        case av_coordinated_response_measure::Color::red:
            return "red";
        case av_coordinated_response_measure::Color::blue:
            return "blue";
        case av_coordinated_response_measure::Color::gray:
            return "gray";
        }
        return "unknown";
    }
    
    void OutputFileImpl::writeTrialHeading() {
        write(formatTrialHeading());
    }
    
    std::string OutputFileImpl::formatTrialHeading() {
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
        return stream.str();
    }
    
    void OutputFileImpl::openNewFile(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        writer->open(generateNewFilePath(test));
        if (writer->failed())
            throw OpenFailure{};
    }
    
    std::string OutputFileImpl::generateNewFilePath(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        return
            path->outputDirectory() + "/" +
            path->generateFileName(test) + ".txt";
    }
    
    namespace {
        class TestStream {
            std::stringstream stream{};
        public:
            template<typename T>
            void writeLabeledLine(std::string label, T thing) {
                stream << label;
                stream << ": ";
                stream << thing;
                stream << '\n';
            }
            
            auto str() const {
                return stream.str();
            }
            
            void insertNewLine() {
                stream << "\n";
            }
        };
    }
    
    void OutputFileImpl::writeTest(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        write(formatTest(test));
    }
    
    std::string OutputFileImpl::formatTest(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        TestStream stream;
        stream.writeLabeledLine("subject", test.subjectId);
        stream.writeLabeledLine("tester", test.testerId);
        stream.writeLabeledLine("session", test.session);
        stream.writeLabeledLine("masker", test.maskerFilePath);
        stream.writeLabeledLine("targets", test.targetListDirectory);
        stream.writeLabeledLine("signal level (dB SPL)", test.signalLevel_dB_SPL);
        stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
        stream.writeLabeledLine("condition", "audio-visual");
        stream.insertNewLine();
        return stream.str();
    }
}

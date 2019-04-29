#include "OutputFileImpl.hpp"
#include <sstream>

namespace av_coordinate_response_measure {
    namespace {
        class FormattedStream {
            std::stringstream stream;
        public:
            template<typename T>
            void writeLabeledLine(std::string label, T thing) {
                stream << std::move(label);
                stream << ": ";
                stream << thing;
                stream << '\n';
            }
            
            template<typename T>
            void insert(T item) {
                stream << item;
            }
            
            void insertCommaAndSpace() {
                stream << ", ";
            }
            
            void insertNewLine() {
                stream << '\n';
            }
            
            auto str() const {
                return stream.str();
            }
        };
    }
    
    OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path) :
        writer{writer},
        path{path} {}
    
    void OutputFileImpl::writeTrial(const Trial &trial) {
        write(formatTrial(trial));
    }
    
    void OutputFileImpl::write(std::string s) {
        writer->write(std::move(s));
    }

    std::string OutputFileImpl::formatTrial(const Trial &trial) {
        FormattedStream stream;
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
        stream.insert(evaluation(trial));
        stream.insertCommaAndSpace();
        stream.insert(trial.reversals);
        stream.insertNewLine();
        return stream.str();
    }
    
    std::string OutputFileImpl::evaluation(const Trial &trial) {
        return trial.correct ? "correct" : "incorrect";
    }

    std::string OutputFileImpl::colorName(Color c) {
        switch (c) {
        case Color::green:
            return "green";
        case Color::red:
            return "red";
        case Color::blue:
            return "blue";
        case Color::white:
            return "white";
        case Color::notAColor:
            return "not a color";
        }
        return "unknown";
    }
    
    void OutputFileImpl::writeTrialHeading() {
        write(formatTrialHeading());
    }
    
    std::string OutputFileImpl::formatTrialHeading() {
        FormattedStream stream;
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
        stream.insert("evaluation");
        stream.insertCommaAndSpace();
        stream.insert("reversals");
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::openNewFile(const Test &test) {
        writer->open(generateNewFilePath(test));
        if (writer->failed())
            throw OpenFailure{};
    }
    
    std::string OutputFileImpl::generateNewFilePath(const Test &test) {
        return
            path->outputDirectory() + "/" +
            path->generateFileName(test) + ".txt";
    }
    
    void OutputFileImpl::writeTest(const Test &test) {
        write(formatTest(test));
    }
    
    std::string OutputFileImpl::formatTest(const Test &test) {
        FormattedStream stream;
        stream.writeLabeledLine("subject", test.subjectId);
        stream.writeLabeledLine("tester", test.testerId);
        stream.writeLabeledLine("session", test.session);
        stream.writeLabeledLine("masker", test.maskerFilePath);
        stream.writeLabeledLine("targets", test.targetListDirectory);
        stream.writeLabeledLine("masker level (dB SPL)", test.maskerLevel_dB_SPL);
        stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
        stream.writeLabeledLine("condition", conditionName(test.condition));
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::close() { 
        writer->close();
    }
}

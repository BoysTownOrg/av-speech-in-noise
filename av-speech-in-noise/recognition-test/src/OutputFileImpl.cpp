#include "OutputFileImpl.hpp"
#include <sstream>

namespace av_speech_in_noise {
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
            
            void writeSubjectId(const TestInformation &p) {
                writeLabeledLine("subject", p.subjectId);
            }
            
            void writeTester(const TestInformation &p) {
                writeLabeledLine("tester", p.testerId);
            }
            
            void writeSession(const TestInformation &p) {
                writeLabeledLine("session", p.session);
            }
            
            void writeMasker(const CommonTest &p) {
                writeLabeledLine("masker", p.maskerFilePath);
            }
            
            void writeTargetList(const CommonTest &p) {
                writeLabeledLine("targets", p.targetListDirectory);
            }
            
            void writeMaskerLevel(const CommonTest &p) {
                writeLabeledLine("masker level (dB SPL)", p.maskerLevel_dB_SPL);
            }
            
            void writeCondition(const CommonTest &p) {
                writeLabeledLine("condition", conditionName(p.condition));
            }
        };
    }
    
    OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path) :
        writer{writer},
        path{path} {}
    
    void OutputFileImpl::write(std::string s) {
        writer->write(std::move(s));
    }
    
    void OutputFileImpl::writeTrial(const coordinate_response_measure::AdaptiveTrial &trial) {
        if (!justWroteAdaptiveCoordinateResponseTrial)
            writeAdaptiveCoordinateResponseTrialHeading();
        FormattedStream stream;
        stream.insert(trial.SNR_dB);
        stream.insertCommaAndSpace();
        stream.insert(trial.trial.correctNumber);
        stream.insertCommaAndSpace();
        stream.insert(trial.trial.subjectNumber);
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.trial.correctColor));
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.trial.subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(evaluation(trial.trial));
        stream.insertCommaAndSpace();
        stream.insert(trial.reversals);
        stream.insertNewLine();
        write(stream.str());
        justWroteAdaptiveCoordinateResponseTrial = true;
    }
    
    void OutputFileImpl::writeAdaptiveCoordinateResponseTrialHeading() {
        FormattedStream stream;
        stream.insert(headingItemName(HeadingItem::snr_dB));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::correctNumber));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::subjectNumber));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::correctColor));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::evaluation));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::reversals));
        stream.insertNewLine();
        write(stream.str());
    }
    
    void OutputFileImpl::writeTrial(const coordinate_response_measure::FixedLevelTrial &trial) {
        if (!justWroteFixedLevelCoordinateResponseTrial)
            writeFixedLevelCoordinateResponseTrialHeading();
        FormattedStream stream;
        stream.insert(trial.trial.correctNumber);
        stream.insertCommaAndSpace();
        stream.insert(trial.trial.subjectNumber);
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.trial.correctColor));
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.trial.subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(evaluation(trial.trial));
        stream.insertCommaAndSpace();
        stream.insert(trial.trial.stimulus);
        stream.insertNewLine();
        write(stream.str());
        justWroteFixedLevelCoordinateResponseTrial = true;
    }
    
    void OutputFileImpl::writeFixedLevelCoordinateResponseTrialHeading() {
        FormattedStream stream;
        stream.insert(headingItemName(HeadingItem::correctNumber));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::subjectNumber));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::correctColor));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::evaluation));
        stream.insertCommaAndSpace();
        stream.insert(headingItemName(HeadingItem::stimulus));
        stream.insertNewLine();
        write(stream.str());
    }
    
    std::string OutputFileImpl::evaluation(const coordinate_response_measure::Trial &trial) {
        return trial.correct ? "correct" : "incorrect";
    }
    
    void OutputFileImpl::writeFreeResponseTrialHeading() {
        write(formatOpenSetTrialHeading());
    }
    
    std::string OutputFileImpl::formatOpenSetTrialHeading() {
        FormattedStream stream;
        stream.insert("target");
        stream.insertCommaAndSpace();
        stream.insert("response");
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::openNewFile(const TestInformation &test) {
        writer->open(generateNewFilePath(test));
        if (writer->failed())
            throw OpenFailure{};
        justWroteAdaptiveCoordinateResponseTrial = false;
        justWroteFixedLevelCoordinateResponseTrial = false;
    }
    
    std::string OutputFileImpl::generateNewFilePath(const TestInformation &test) {
        return
            path->outputDirectory() + "/" +
            path->generateFileName(test) + ".txt";
    }
    
    void OutputFileImpl::writeTest(const AdaptiveTest &test) {
        write(formatTest(test));
    }
    
    void OutputFileImpl::writeTest(const FixedLevelTest &test) {
        write(formatTest(test));
    }
    
    std::string OutputFileImpl::formatTest(const AdaptiveTest &test) {
        FormattedStream stream;
        auto information = test.information;
        stream.writeSubjectId(information);
        stream.writeTester(information);
        stream.writeSession(information);
        auto common = test.common;
        stream.writeMasker(common);
        stream.writeTargetList(common);
        stream.writeMaskerLevel(common);
        stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
        stream.writeCondition(common);
        stream.insertNewLine();
        return stream.str();
    }
    
    std::string OutputFileImpl::formatTest(const FixedLevelTest &test) {
        FormattedStream stream;
        auto information = test.information;
        stream.writeSubjectId(information);
        stream.writeTester(information);
        stream.writeSession(information);
        auto common = test.common;
        stream.writeMasker(common);
        stream.writeTargetList(common);
        stream.writeMaskerLevel(common);
        stream.writeLabeledLine("SNR (dB)", test.snr_dB);
        stream.writeCondition(common);
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::close() {
        writer->close();
    }
    
    void OutputFileImpl::save() {
        writer->save();
    }
    
    void OutputFileImpl::writeTrial(const FreeResponseTrial &trial) {
        if (!justWroteFreeResponseTrial)
            writeFreeResponseTrialHeading();
        write(formatTrial(trial));
        justWroteFreeResponseTrial = true;
    }

    std::string OutputFileImpl::formatTrial(const FreeResponseTrial &trial) {
        FormattedStream stream;
        stream.insert(trial.target);
        stream.insertCommaAndSpace();
        stream.insert(trial.response);
        stream.insertNewLine();
        return stream.str();
    }
    
}

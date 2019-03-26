#include <av-coordinated-response-measure/Model.h>
#include <sstream>

class Writer {
public:
    virtual ~Writer() = default;
    virtual void write(std::string) = 0;
};

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
    
    auto str() const {
        return stream.str();
    }
};

class OutputFile {
    Writer *writer;
public:
    OutputFile(Writer *writer) : writer{writer} {}
    void writeTrial(av_coordinated_response_measure::Trial trial) {
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
        stream.insert("\n");
        writer->write(stream.str());
    }
    
private:
    std::string colorName(av_coordinated_response_measure::Color c) {
        switch (c) {
        case av_coordinated_response_measure::Color::green:
            return "green";
        case av_coordinated_response_measure::Color::red:
            return "red";
        }
    }
};

#include "assert-utility.h"
#include <gtest/gtest.h>

class WriterStub : public Writer {
    std::string written_{};
public:
    auto written() const {
        return written_;
    }
    
    void write(std::string s) override {
        written_ = std::move(s);
    }
};

class OutputFileTests : public ::testing::Test {
protected:
    WriterStub writer{};
    OutputFile file{&writer};
    av_coordinated_response_measure::Trial trial{};
};

TEST_F(OutputFileTests, writeTrial) {
    trial.SNR_dB = 1;
    trial.correctNumber = 2;
    trial.subjectNumber = 3;
    trial.correctColor = av_coordinated_response_measure::Color::green;
    trial.subjectColor = av_coordinated_response_measure::Color::red;
    trial.reversals = 4;
    file.writeTrial(trial);
    assertEqual(
        "1, 2, 3, green, red, 4\n",
        writer.written()
    );
}

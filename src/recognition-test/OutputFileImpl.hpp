#ifndef recognition_test_OutputFileImpl_hpp
#define recognition_test_OutputFileImpl_hpp

#include "RecognitionTestModel.hpp"

namespace recognition_test {
    class Writer {
    public:
        virtual ~Writer() = default;
        virtual void write(std::string) = 0;
        virtual void open(std::string) = 0;
        virtual bool failed() = 0;
    };
    
    class OutputFilePath {
    public:
        virtual ~OutputFilePath() = default;
        virtual std::string generateFileName(
            const av_coordinated_response_measure::Model::Test &
        ) = 0;
        virtual std::string homeDirectory() = 0;
        virtual std::string outputDirectory() = 0;
    };

    class OutputFileImpl : public OutputFile {
        Writer *writer;
        OutputFilePath *path;
    public:
        OutputFileImpl(Writer *, OutputFilePath *);
        void writeTrial(const av_coordinated_response_measure::Trial &) override;
        void writeTrialHeading();
        void openNewFile(const av_coordinated_response_measure::Model::Test &) override;
        class OpenFailure {};
    private:
        std::string formatTrial(const av_coordinated_response_measure::Trial &);
        std::string formatTrialHeading();
        std::string generateNewFilePath(const av_coordinated_response_measure::Model::Test &);
        std::string colorName(av_coordinated_response_measure::Color);
    };
}


#endif

#ifndef recognition_test_OutputFileImpl_hpp
#define recognition_test_OutputFileImpl_hpp

#include "RecognitionTestModel.hpp"

namespace recognition_test {
    class Writer {
    public:
        virtual ~Writer() = default;
        virtual void write(std::string) = 0;
    };

    class OutputFileImpl : public OutputFile {
        Writer *writer;
    public:
        OutputFileImpl(Writer *);
        void writeTrial(const av_coordinated_response_measure::Trial &);
    private:
        std::string colorName(av_coordinated_response_measure::Color);
    };
}


#endif

#include <recognition-test/OutputFile.hpp>
#include <string>

namespace av_speech_in_noise {
struct TestSetupState {
    std::string masker;
};

class TestSetupStateWriter {
    Writer &writer;
public:
    explicit TestSetupStateWriter(Writer &writer) : writer{writer} {}

    void write(const TestSetupState &) {
        writer.write("masker: a\n");
    }
};
}

#include "assert-utility.h"
#include "LogString.h"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests { namespace {
class WriterStub : public Writer {
    LogString written_;
    std::string filePath_;
    bool closed_{};
    bool saved_{};

  public:
    void save() override { saved_ = true; }

    auto saved() const { return saved_; }

    void close() override { closed_ = true; }

    void open(std::string f) override { filePath_ = std::move(f); }

    auto filePath() const { return filePath_; }

    auto closed() const { return closed_; }

    auto &written() const { return written_; }

    void write(std::string s) override { written_.insert(std::move(s)); }

    bool failed() override { return {}; }
};

class TestSetupStateWriterTests : public ::testing::Test {
protected:
    void setMasker(std::string s) {
        state.masker = std::move(s);
    }

    void write() {
        writer.write(state);
    }

    void assertLabeledEntryWritten(std::string label, std::string entry) {
        assertWrittenContains(label + ": " + entry + "\n");
    }

    const auto &written() {
        return stub.written();
    }

    void assertWrittenContains(std::string s) {
        assertTrue(written().contains(std::move(s)));
    }
private:
    WriterStub stub;
    TestSetupStateWriter writer{stub};
    TestSetupState state;
};

TEST_F(TestSetupStateWriterTests, tbd) {
    setMasker("a");
    write();
    assertLabeledEntryWritten("masker", "a");
}
}}

#ifndef TESTS_LOGSTRING_H_
#define TESTS_LOGSTRING_H_

#include <sstream>
#include <utility>
#include <string>

class LogString {
    std::stringstream log{};

  public:
    void insert(const std::string &s_) { log << s_; }

    auto isEmpty() const -> bool { return log.str().empty(); }

    auto beginsWith(std::string const &beginning) const -> bool {
        if (log.str().length() >= beginning.length())
            return 0 == log.str().compare(0, beginning.length(), beginning);

        return false;
    }

    auto endsWith(std::string const &ending) const -> bool {
        if (log.str().length() >= ending.length())
            return 0 ==
                log.str().compare(log.str().length() - ending.length(),
                    ending.length(), ending);

        return false;
    }

    auto contains(const std::string &s2) const -> bool {
        return log.str().find(s2) != std::string::npos;
    }

    operator std::string() const { return log.str(); }
};

#endif

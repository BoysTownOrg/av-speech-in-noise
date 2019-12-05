#ifndef TESTS_LOGSTRING_H_
#define TESTS_LOGSTRING_H_

#include <sstream>
#include <utility>
#include <string>

class LogString {
    std::stringstream log{};

  public:
    void insert(const std::string &s_) { log << s_; }

    bool isEmpty() const { return log.str().empty(); }

    bool beginsWith(std::string const &beginning) const {
        if (log.str().length() >= beginning.length())
            return 0 == log.str().compare(0, beginning.length(), beginning);

        return false;
    }

    bool endsWith(std::string const &ending) const {
        if (log.str().length() >= ending.length())
            return 0 ==
                log.str().compare(log.str().length() - ending.length(),
                    ending.length(), ending);

        return false;
    }

    bool contains(const std::string &s2) const {
        return log.str().find(s2) != std::string::npos;
    }

    operator std::string() const { return log.str(); }
};

#endif

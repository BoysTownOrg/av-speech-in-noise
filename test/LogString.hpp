#ifndef TESTS_LOGSTRING_HPP_
#define TESTS_LOGSTRING_HPP_

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

inline void insert(std::stringstream &log, const std::string &s_) { log << s_; }

inline auto isEmpty(const std::stringstream &log) -> bool {
    return log.str().empty();
}

inline auto beginsWith(
    const std::stringstream &log, std::string const &beginning) -> bool {
    if (log.str().length() >= beginning.length())
        return 0 == log.str().compare(0, beginning.length(), beginning);

    return false;
}

inline auto endsWith(const std::stringstream &log, std::string const &ending)
    -> bool {
    if (log.str().length() >= ending.length())
        return 0 ==
            log.str().compare(
                log.str().length() - ending.length(), ending.length(), ending);

    return false;
}

inline auto contains(const std::stringstream &log, const std::string &s2)
    -> bool {
    return log.str().find(s2) != std::string::npos;
}

inline auto string(const std::stringstream &log) -> std::string {
    return log.str();
}

#endif

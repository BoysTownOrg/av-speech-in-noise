#ifndef TESTS_LOGSTRING_HPP_
#define TESTS_LOGSTRING_HPP_

#include <gsl/gsl>
#include <sstream>
#include <string>

inline auto string(const std::stringstream &log) -> std::string {
    return log.str();
}

inline auto length(const std::string &s) -> gsl::index { return s.length(); }

inline auto length(const std::stringstream &s) -> gsl::index {
    return length(string(s));
}

inline void insert(std::stringstream &s, const std::string &what) { s << what; }

inline auto isEmpty(const std::stringstream &s) -> bool {
    return string(s).empty();
}

inline auto matches(const std::stringstream &s, gsl::index position,
    const std::string &other) -> bool {
    return 0 == string(s).compare(position, length(other), other);
}

inline auto isAsLongAs(const std::stringstream &s, const std::string &what)
    -> bool {
    return length(s) >= length(what);
}

inline auto beginsWith(
    const std::stringstream &log, std::string const &beginning) -> bool {
    if (isAsLongAs(log, beginning))
        return matches(log, 0, beginning);
    return false;
}

inline auto endsWith(const std::stringstream &log, std::string const &ending)
    -> bool {
    if (isAsLongAs(log, ending))
        return matches(log, length(log) - length(ending), ending);
    return false;
}

inline auto contains(const std::stringstream &log, const std::string &s)
    -> bool {
    return string(log).find(s) != std::string::npos;
}

#endif

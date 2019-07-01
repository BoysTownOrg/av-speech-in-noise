#ifndef LogString_h
#define LogString_h

#include <sstream>

class LogString {
    std::stringstream log{};
public:
    void insert(std::string s_) {
        log << std::move(s_);
    }

    bool isEmpty() const {
        return log.str().empty();
    }

    bool beginsWith(std::string const &beginning) const {
        if (log.str().length() >= beginning.length())
            return 0 == log.str().compare(0, beginning.length(), beginning);
        else
            return false;
    }

    bool endsWith(std::string const &ending) const {
        if (log.str().length() >= ending.length())
            return 0 == log.str().compare(
                log.str().length() - ending.length(),
                ending.length(),
                ending);
        else
            return false;
    }

    bool contains(std::string s2) const {
        return log.str().find(std::move(s2)) != std::string::npos;
    }
    
    operator std::string() const { return log.str(); }
};

#endif

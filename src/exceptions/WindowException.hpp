#ifndef CLIENTBRAVERATS_WINDOWEXCEPTION_HPP
#define CLIENTBRAVERATS_WINDOWEXCEPTION_HPP

#include "Exception.hpp"

class WindowException : public Exception {
public:
    explicit WindowException(const char* message) : Exception(message){};
};

#endif //CLIENTBRAVERATS_WINDOWEXCEPTION_HPP

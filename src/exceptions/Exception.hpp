#ifndef CLIENTBRAVERATS_EXCEPTION_HPP
#define CLIENTBRAVERATS_EXCEPTION_HPP

#include <exception>
#include <string>

class Exception : public std::exception {
protected:
    std::string message;

public:
    explicit Exception(const char* _message);

    [[nodiscard]] const char* what() const noexcept override;
};

#endif //CLIENTBRAVERATS_EXCEPTION_HPP

#include "Exception.hpp"

Exception::Exception(const char* _message) : message(_message){}

const char* Exception::what() const noexcept {

    return message.c_str();

}
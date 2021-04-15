#pragma once

#include <exception>
#include <string>
#include <utility>

class NotFoundException : public std::exception
{
public:
    explicit NotFoundException(std::string errMsg) noexcept
        : _message( std::move(errMsg) )
    {}

    /**
     *  @return C-style character string describing the general cause of the current error.
     */
    [[nodiscard]] const char* what() const noexcept override { return _message.c_str(); }

private:
    std::string _message;
};
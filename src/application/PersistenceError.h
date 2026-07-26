#pragma once

#include <stdexcept>
#include <string>

/** @brief Recoverable persistence-layer failure during an application operation. */
class PersistenceError : public std::runtime_error
{
public:
    /** @brief Creates a persistence error. @param message Diagnostic message. */
    explicit PersistenceError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

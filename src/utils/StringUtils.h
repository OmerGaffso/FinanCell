#pragma once

#include <string>

namespace StringUtils
{
/** @brief Trims and lowercases text. @param text Input text. @return Normalized text. */
std::string normalize(const std::string& text);
/** @brief Removes surrounding whitespace. @param text Input text. @return Trimmed text. */
std::string trim(const std::string& text);
/** @brief Converts text to lowercase. @param text Input text. @return Lowercase text. */
std::string toLower(const std::string& text);
}

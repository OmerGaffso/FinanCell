#include "utils/StringUtils.h"

#include <algorithm>
#include <cctype>

namespace StringUtils
{
std::string normalize(const std::string& text)
{
    return toLower(trim(text));
}

std::string trim(const std::string& text)
{
    const std::size_t start = text.find_first_not_of(" \t");
    const std::size_t end = text.find_last_not_of(" \t");

    if (start == std::string::npos || end == std::string::npos)
    {
        return "";
    }

    return text.substr(start, end - start + 1);
}

std::string toLower(const std::string& text)
{
    std::string result = text;
    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(std::tolower(character));
        });

    return result;
}
}

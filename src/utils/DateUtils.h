#pragma once

#include <optional>
#include <string>
#include <utility>

namespace DateUtils
{
/** @brief Validates an empty date or an ISO YYYY-MM-DD date. @param date Date text. @return True when empty or a valid ISO date. */
bool isIsoDateValid(const std::string& date);
/** @brief Returns the inclusive first and last dates for a month. @param month Month in YYYY-MM format. @return Date range, or empty for an invalid month. */
std::optional<std::pair<std::string, std::string>> monthRange(const std::string& month);
}

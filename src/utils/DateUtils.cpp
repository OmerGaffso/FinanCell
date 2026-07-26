#include "utils/DateUtils.h"

#include <cctype>

namespace
{
bool isLeapYear(int year)
{
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int year, int month)
{
    constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && isLeapYear(year) ? 29 : days[month - 1];
}
}

bool DateUtils::isIsoDateValid(const std::string& date)
{
    if (date.empty()) return true;
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t index = 0; index < date.size(); ++index)
    {
        if (index != 4 && index != 7 &&
            !std::isdigit(static_cast<unsigned char>(date[index])))
        {
            return false;
        }
    }

    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));
    return year >= 1 && month >= 1 && month <= 12 &&
           day >= 1 && day <= daysInMonth(year, month);
}

std::optional<std::pair<std::string, std::string>> DateUtils::monthRange(
    const std::string& month)
{
    if (month.size() != 7 || !isIsoDateValid(month + "-01")) return std::nullopt;
    const int year = std::stoi(month.substr(0, 4));
    const int monthNumber = std::stoi(month.substr(5, 2));
    return std::make_pair(
        month + "-01",
        month + "-" + std::to_string(daysInMonth(year, monthNumber)));
}

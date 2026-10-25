#include "Date.h"

Date::Date(Day day, Month month, Year year) {
  if (day < 1)
    throw std::runtime_error("Day must be > 0");
  if (year < 2000)
    throw std::runtime_error("Year must be >= 2000");

  Day max_day = 0;
  switch (month) {
  case Month::JAN:
  case Month::MAR:
  case Month::MAY:
  case Month::JUL:
  case Month::AUG:
  case Month::OCT:
  case Month::DEC:
    max_day = 31;
    break;
  case Month::APR:
  case Month::JUN:
  case Month::SEP:
  case Month::NOV:
    max_day = 30;
    break;
  case Month::FEB:
    max_day = (!(year % 4) && year % 100) || !(year % 400) ? 29 : 28;
  }

  if (day > max_day)
    throw std::runtime_error(std::string("Day must be <= ") +
                             std::to_string(max_day));

  day_ = day;
  month_ = month;
  year_ = year;
}

Day Date::day() const { return day_; }
Month Date::month() const { return month_; }
Year Date::year() const { return year_; }

DateTime::DateTime(Day day, Month month, Year year, Time mins)
    : date_(day, month, year), mins_(mins) {}

const Date &DateTime::date() const { return date_; }
Time DateTime::mins() const { return mins_; }

#include "Date.h"
#include <iomanip>
#include <type_traits>
#include <sstream>
#include <stdexcept>

DateTime::DateTime(Day day, Month month, Year year, Time mins) {
  check_valid_date(day, month, year, mins);
  day_ = day;
  month_ = month;
  year_ = year;
  mins_ = mins;
}

void DateTime::set_date(Day day, Month month, Year year, Time mins) {
  check_valid_date(day, month, year, mins);
  day_ = day;
  month_ = month;
  year_ = year;
  if (mins != std::numeric_limits<std::uint16_t>::max())
    mins_ = mins;
}

Day DateTime::day() const { return day_; }
Month DateTime::month() const { return month_; }
Year DateTime::year() const { return year_; }
Time DateTime::mins() const { return mins_; }

std::string DateTime::to_string() const {
  std::ostringstream result;
  result << std::setfill('0') << std::setw(2) << day_ << "-" << std::setw(2)
         << static_cast<int>(month_) + 1 << "-" << std::setw(4) << year_ << "@"
         << std::setfill('0') << std::setw(2) << mins_ / 60 << ":"
         << std::setw(2) << mins_ % 60;

  return result.str();
}

void DateTime::check_valid_date(Day day, Month month, Year year, Time mins) {
  if (mins > 1439 && mins != std::numeric_limits<std::uint16_t>::max())
    throw std::runtime_error("Time must be < 1440");
  if (day < 1)
    throw std::runtime_error("Day must be > 0");
  if (year < 2000)
    throw std::runtime_error("Year must be >= 2000");

  Day max_day = get_max_day(month, year);

  if (day > max_day)
    throw std::runtime_error(std::string("Day must be <= ") +
                             std::to_string(max_day) + std::string("!"));
}

void DateTime::increment(std::size_t amt) {
  while (amt > 0) {
    const Day max_day = get_max_day(month_, year_);
    const std::size_t days_left = static_cast<std::size_t>(max_day - day_);

    if (amt <= days_left) {
      day_ = static_cast<Day>(day_ + amt);
      return;
    }

    amt -= days_left + 1;
    day_ = 1;

    if (month_ == Month::DEC) {
      ++year_;
      month_ = Month::JAN;
    } else
      month_ = static_cast<Month>(
          static_cast<std::underlying_type_t<Month>>(month_) + 1);
  }
}

Day DateTime::get_max_day(Month month, Year year) const {
  switch (month) {
  case Month::JAN:
  case Month::MAR:
  case Month::MAY:
  case Month::JUL:
  case Month::AUG:
  case Month::OCT:
  case Month::DEC:
    return 31;
  case Month::APR:
  case Month::JUN:
  case Month::SEP:
  case Month::NOV:
    return 30;
  case Month::FEB:
    return (!(year % 4) && year % 100) || !(year % 400) ? 29 : 28;
  default:
    throw std::logic_error("Invalid month!");
  }
}

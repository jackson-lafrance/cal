#ifndef DATE_H
#define DATE_H
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>

using Time = std::uint32_t;
using Day = std::uint8_t;
using Year = std::uint16_t;

enum class Month {
  JAN,
  FEB,
  MAR,
  APR,
  MAY,
  JUN,
  JUL,
  AUG,
  SEP,
  OCT,
  NOV,
  DEC,
};

class Date {
public:
  friend bool operator<(Date lhs, Date rhs) {
    return std::tie(lhs.year_, lhs.month_, lhs.day_) <
           std::tie(rhs.year_, rhs.month_, rhs.day_);
  }

  Date(Day day, Month month, Year year) {
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
  };

  Day day() const { return day_; }
  Month month() const { return month_; }
  Year year() const { return year_; }

private:
  Day day_;
  Month month_;
  int year_;
};

class DateTime {
public:
  friend bool operator<(DateTime lhs, DateTime rhs) {
    return std::tie(lhs.date_, lhs.mins_) < std::tie(rhs.date_, rhs.mins_);
  }

  DateTime(Day day, Month month, Year year, Time mins)
      : date_(day, month, year), mins_(mins) {}
  const Date &date() const { return date_; }
  Time mins() const { return mins_; }

private:
  Date date_;
  Time mins_;
};

#endif // DATE_H

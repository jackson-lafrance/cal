#ifndef DATE_H
#define DATE_H
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>

using Time = std::uint32_t;
using Day = std::uint16_t;
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

  Date(Day day, Month month, Year year);

  Day day() const;
  Month month() const;
  Year year() const;
  std::string to_string() const;

private:
  Day day_;
  Month month_;
  Year year_;
};

class DateTime {
public:
  friend bool operator<(DateTime lhs, DateTime rhs) {
    return std::tie(lhs.date_, lhs.mins_) < std::tie(rhs.date_, rhs.mins_);
  }

  DateTime(Day day, Month month, Year year, Time mins);
  const Date &date() const;
  Time mins() const;
  std::string to_string() const;

private:
  Date date_;
  Time mins_;
};

#endif // DATE_H

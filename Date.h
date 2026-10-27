#ifndef DATE_H
#define DATE_H
#include <cstdint>
#include <iostream>
#include <limits>
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

class DateTime {
public:
  friend bool operator<(DateTime lhs, DateTime rhs) {
    return std::tie(lhs.year_, lhs.month_, lhs.day_, lhs.mins_) <
           std::tie(rhs.year_, rhs.month_, rhs.day_, rhs.mins_);
  }

  DateTime(Day day, Month month, Year year, Time mins);
  Day day() const;
  Month month() const;
  Year year() const;
  Time mins() const;
  std::string time_string() const;
  std::string date_string() const;

  void set_date(Day day, Month month, Year year, Time mins);
  void change(int amt);

private:
  Time mins_;
  Day day_;
  Month month_;
  Year year_;

  void check_valid_date(Day day, Month month, Year year, Time mins);
  Day get_max_day(Month month, Year year) const;
};

#endif // DATE_H

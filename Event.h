#ifndef EVENT_H
#define EVENT_H
#include "Date.h"
#include "Task.h"
#include <set>
#include <string>
#include <tuple>
#include <utility>

class Event {
public:
  friend bool operator<(const Event &lhs, const Event &rhs) {
    return std::tie(lhs.start_, rhs.end_, lhs.title_) <
           std::tie(rhs.start_, lhs.end_, rhs.title_);
  }

  Event(Day start_day, Month start_month, Year start_year, Time start_mins,
        Day end_day, Month end_month, Year end_year, Time end_mins,
        std::string title)
      : start_(start_day, start_month, start_year, start_mins),
        end_(end_day, end_month, end_year, end_mins),
        title_(std::move(title)) {};

  const DateTime &start() const { return start_; }
  const DateTime &end() const { return end_; }
  const std::string &title() const { return title_; }

private:
  DateTime start_;
  DateTime end_;
  std::string title_;
};

#endif // EVENT_H

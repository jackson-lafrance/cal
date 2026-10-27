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
    return std::tie(lhs.start_, lhs.end_, lhs.title_) <
           std::tie(rhs.start_, rhs.end_, rhs.title_);
  }

  friend bool operator<(const Event &lhs, const DateTime &rhs) {
    return std::tie(lhs.start_) < std::tie(rhs);
  }

  friend bool operator<(const DateTime &lhs, const Event &rhs) {
    return std::tie(lhs) < std::tie(rhs.start_);
  }

  Event(Day start_day, Month start_month, Year start_year, Time start_mins,
        Day end_day, Month end_month, Year end_year, Time end_mins,
        std::string title)
      : start_(start_day, start_month, start_year, start_mins),
        end_(end_day, end_month, end_year, end_mins), title_(std::move(title)) {
    if (end_ < start_)
      throw std::runtime_error("End time cannot be before start time!");
  };

  const DateTime &start() const { return start_; }
  const DateTime &end() const { return end_; }
  void
  change_start_date(Day day, Month month, Year year,
                    Time mins = std::numeric_limits<std::uint16_t>::max()) {
    start_.set_date(day, month, year, mins);
  }

  void change_end_date(Day day, Month month, Year year,
                       Time mins = std::numeric_limits<std::uint16_t>::max()) {
    end_.set_date(day, month, year, mins);
  };

  const std::string &title() const { return title_; }
  void change_both_date(int amt) {
    end_.change(amt);
    start_.change(amt);
  }

  void change_start_date(int amt) {
    DateTime temp(start_);
    temp.change(amt);
    if (end_ < temp)
      throw std::runtime_error("Cannot change start date past end date!");

    start_.change(amt);
  }

  void change_end_date(int amt) { end_.change(amt); }

private:
  DateTime start_;
  DateTime end_;
  std::string title_;
};

using Events = std::set<Event, std::less<>>;

#endif // EVENT_H

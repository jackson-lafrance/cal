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
  Event(Day start_day, Month start_month, Year start_year, Time start_mins,
        Day end_day, Month end_month, Year end_year, Time end_mins,
        std::string title)
      : start_(start_day, start_month, start_year, start_mins),
        end_(end_day, end_month, end_year, end_mins),
        title_(std::move(title)) {};

  const DateTime &getStartTime() const { return start_; }
  const DateTime &getEndTime() const { return end_; }
  const std::string &getTitle() const { return title_; }

private:
  DateTime start_;
  DateTime end_;
  std::string title_;
};

#endif // EVENT_H

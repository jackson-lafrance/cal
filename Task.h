#ifndef TASK_H
#define TASK_H
#include "Date.h"
#include <set>
#include <string>
#include <tuple>

class Task {
public:
  friend bool operator<(const Task &lhs, const Task &rhs) {
    return std::tie(lhs.due_, lhs.title_) < std::tie(rhs.due_, rhs.title_);
  }

  Task(Day due_day, Month due_month, Year due_year, Time due_mins,
       std::string title)
      : due_(due_day, due_month, due_year, due_mins), title_(title) {}

  const DateTime &due() const { return due_; }
  const std::string &title() const { return title_; }

private:
  DateTime due_;
  std::string title_;
};

#endif // TASK_H

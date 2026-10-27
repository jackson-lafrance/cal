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

  friend bool operator<(const Task &lhs, const DateTime &rhs) {
    return std::tie(lhs.due_) < std::tie(rhs);
  }

  friend bool operator<(const DateTime &lhs, const Task &rhs) {
    return std::tie(lhs) < std::tie(rhs.due_);
  }

  Task(Day due_day, Month due_month, Year due_year, Time due_mins,
       std::string title)
      : due_(due_day, due_month, due_year, due_mins), title_(title) {}

  const DateTime &due() const { return due_; }
  const std::string &title() const { return title_; }
  void change_due_date(Day day, Month month, Year year,
                       Time mins = std::numeric_limits<std::uint16_t>::max()) {
    due_.set_date(day, static_cast<Month>(month), year, mins);
  }

  void change_due_date(int amt) { due_.change(amt); }

private:
  DateTime due_;
  std::string title_;
};

using Tasks = std::set<Task, std::less<>>;

#endif // TASK_H

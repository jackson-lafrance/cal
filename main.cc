#include "Event.h"
#include "Task.h"
#include <array>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <math.h>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

using Events = std::set<Event>;
using Tasks = std::set<Task>;
using EventOrTask = std::variant<Event, Task>;

EventOrTask add(const std::string &command);
Event add_event(const std::string &command);
Task add_task(const std::string &command);

Time parse_time(const std::string &time);

int main() {
  Events event_set;
  Tasks task_set;

  for (;;) {
    std::string command;

    if (!std::getline(std::cin, command)) {
      break;
    }

    if (command.compare(0, 3, "add")) {
      EventOrTask result = add(command);
      if (const Event *event = std::get_if<Event>(&result))
        event_set.insert(*event);
      else if (const Task *task = std::get_if<Task>(&result))
        task_set.insert(*task);
      else
        throw std::logic_error("Invalid type result of `add`");
    } else if (command.compare(0, 4, "quit"))
      break;
    else
      std::cout << "Invalid command!" << std::endl;

    auto e = event_set.begin();
    auto t = task_set.begin();

    while (e != event_set.end() || t != task_set.end()) {
      if (e == event_set.end()) {
        std::cout << "TASK " << t->title() << ": " << t->due().to_string()
                  << std::endl;
        ++t;
      } else if (t == task_set.end()) {
        std::cout << "EVENT " << e->title() << ": " << e->start().to_string()
                  << " to " << e->end().to_string() << std::endl;
        ++e;
      } else if (e->start() < t->due()) {
        std::cout << "EVENT " << e->title() << ": " << e->start().to_string()
                  << " to " << e->end().to_string() << std::endl;
        ++e;
      } else {
        std::cout << "TASK " << t->title() << ": " << t->due().to_string();
        ++t;
      }
    }
  }

  return 0;
}

EventOrTask add(const std::string &command) {
  std::string tester;
  for (std::size_t i{0}; i < command.length(); ++i) {
    if (command[i] == ' ')
      continue;

    tester += command[i];
    if (tester.length() == 4 && tester == "task")
      return add_task(command.substr(i + 1));
    else if (tester.length() == 5 && tester == "event")
      return add_event(command.substr(i + 1));
    else if (tester.length() >= 5)
      throw std::runtime_error("Invalid call to `add` with " + tester + "!");
  }

  throw std::runtime_error("Invalid empty call to `add`!");
}

Event add_event(const std::string &command) {
  std::size_t tracker = 0;
  std::array<std::string, 9> fields;

  for (const char &letter : command) {
    if (letter == ' ' && (tracker != 8 || fields[8].empty()))
      continue;

    if (letter == ',') {
      ++tracker;
      continue;
    }

    if (tracker != 8 && !std::isdigit(letter) &&
        !((tracker == 3 || tracker == 7) && letter == ':'))
      throw std::runtime_error("Cannot parse argument (" +
                               std::to_string(tracker + 1) +
                               ") to `add event`");

    fields.at(tracker) += letter;
  }

  if (tracker != 8)
    throw std::runtime_error("Incorrect amount of arguments to `add event` (" +
                             std::to_string(tracker + 1) + ")!");

  return Event(
      std::stoi(fields[0]), static_cast<Month>(std::stoi(fields[1]) - 1),
      std::stoi(fields[2]), parse_time(fields[3]), std::stoi(fields[4]),
      static_cast<Month>(std::stoi(fields[5]) - 1), std::stoi(fields[6]),
      parse_time(fields[7]), std::move(fields[8]));
}

Task add_task(const std::string &command) {
  std::size_t tracker = 0;
  std::array<std::string, 5> fields;

  for (const char &letter : command) {
    if (letter == ' ' && (tracker != 4 || fields[4].empty()))
      continue;

    if (letter == ',') {
      ++tracker;
      continue;
    }

    if (tracker != 4 && !std::isdigit(letter) &&
        !(tracker == 3 && letter == ':'))
      throw std::runtime_error("Cannot parse argument (" +
                               std::to_string(tracker + 1) + ") to `add task`");

    fields.at(tracker) += letter;
  }

  if (tracker != 8)
    throw std::runtime_error("Incorrect amount of arguments to `add task` (" +
                             std::to_string(tracker + 1) + ")!");

  return Task(
      std::stoi(fields[0]), static_cast<Month>(std::stoi(fields[1]) - 1),
      std::stoi(fields[2]), parse_time(fields[3]), std::move(fields[4]));
};

Time parse_time(const std::string &time) {
  bool swap = false;
  std::string hours;
  std::string mins;

  for (const char &letter : time) {
    if (letter == ':') {
      swap = true;
      continue;
    }

    if (!std::isdigit(letter))
      throw std::runtime_error("Invalid time format!");

    if (swap) {
      mins += letter;
      continue;
    }

    hours += letter;
  }

  if (hours.empty() || mins.empty() || !swap)
    throw std::runtime_error("Invalid time format!");

  Time result_hours = std::stoi(hours);
  Time result_mins = std::stoi(mins);

  if (result_hours > 23 || result_mins > 59)
    throw std::runtime_error("Invalid time amounts!");

  return result_hours * 60 + result_mins;
}

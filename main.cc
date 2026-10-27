#include "Display.h"
#include "Event.h"
#include "Task.h"
#include <array>
#include <cctype>
#include <cstddef>
#include <ctime>
#include <exception>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

using EventOrTask = std::variant<Event, Task>;

EventOrTask add(const std::string &command);
Event add_event(const std::string &command);
Task add_task(const std::string &command);
Time parse_time(const std::string &time);

int main() {
  std::time_t now = std::time(nullptr);
  std::tm *localTime = std::localtime(&now);

  int day = localTime->tm_mday;
  int month = localTime->tm_mon;
  int year = localTime->tm_year + 1900;

  Event start_event(day, static_cast<Month>(month), year, 0, day,
                    static_cast<Month>(month), year, 0, "ZZZZZZ");
  Event end_event(day, static_cast<Month>(month), year, 1439, day,
                  static_cast<Month>(month), year, 1439, "ZZZZZZ");

  Task start_task(day, static_cast<Month>(month), year, 0, "ZZZZZZ");
  Task end_task(day, static_cast<Month>(month), year, 1439, "ZZZZZZ");

  end_event.change_both_date(6);
  end_task.change_due_date(6);

  Events event_set;
  Tasks task_set;

  for (;;) {
    std::string command;

    if (!std::getline(std::cin, command)) {
      break;
    }

    if (command.compare(0, 3, "add") == 0) {
      try {
        EventOrTask result = add(command);
        if (const Event *event = std::get_if<Event>(&result))
          event_set.insert(*event);
        else if (const Task *task = std::get_if<Task>(&result))
          task_set.insert(*task);
        else
          throw std::logic_error("Invalid type result of `add`");
      } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        continue;
      }
    } else if (command.compare(0, 4, "next") == 0) {
      start_task.change_due_date(1);
      end_task.change_due_date(1);
      start_event.change_both_date(1);
      end_event.change_both_date(1);
    } else if (command.compare(0, 4, "prev") == 0) {
      start_task.change_due_date(-1);
      end_task.change_due_date(-1);
      start_event.change_both_date(-1);
      end_event.change_both_date(-1);
    } else if (command.compare(0, 4, "quit") == 0)
      break;
    else
      std::cout << "Invalid command!" << std::endl;

    Events events_slice(event_set.lower_bound(start_event),
                        event_set.upper_bound(end_event));
    Tasks tasks_slice(task_set.lower_bound(start_task),
                      task_set.upper_bound(end_task));

    std::cout << "Showing tasks from " << start_event.start().to_string()
              << " to " << end_event.end().to_string() << std::endl;
    Display::print_events_and_tasks(events_slice, tasks_slice);
  }

  return 0;
}

EventOrTask add(const std::string &command) {
  std::string tester;
  for (std::size_t i{3}; i < command.length(); ++i) {
    if (command[i] == ' ')
      continue;

    tester += command[i];
    if (tester.length() == 4 && tester == "task")
      return add_task(command.substr(i + 1));
    else if (tester.length() == 5 && tester == "event")
      return add_event(command.substr(i + 1));
    else if (tester.length() >= 5)
      throw std::runtime_error("Invalid call to `add` with " + command + "!");
  }

  throw std::runtime_error("Invalid call to `add` with " + command + "!");
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

  if (tracker != 4)
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

#include "Display.h"
#include "Event.h"
#include "Store.h"
#include "Task.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
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
void seed_fake_data(Events &events, Tasks &tasks);

int main() {
  std::time_t now = std::time(nullptr);
  std::tm *localTime = std::localtime(&now);

  int day = localTime->tm_mday;
  int month = localTime->tm_mon + 1;
  int year = localTime->tm_year + 1900;

  DateTime start_cursor(day, month, year, 0);

  Events event_set;
  Tasks task_set;
  Storage::load(event_set, task_set);

  const auto render = [&](const std::string &error) {
    try {
      Display::print_screen(event_set, task_set, start_cursor, error);
      return true;
    } catch (const std::exception &e) {
      std::cout << e.what();
      return false;
    }
  };

  if (!render(""))
    return 1;

  for (;;) {
    std::string command;
    std::string error;

    if (!std::getline(std::cin, command)) {
      break;
    }

    std::transform(command.begin(), command.end(), command.begin(),
                   [](unsigned char letter) { return std::tolower(letter); });

    try {
      if (!command.compare(0, 3, "add")) {
        EventOrTask result = add(command);
        if (const Event *event = std::get_if<Event>(&result))
          event_set.insert(*event);
        else if (const Task *task = std::get_if<Task>(&result))
          task_set.insert(*task);
        else
          throw std::logic_error("Invalid type result of `add`");
      } else if (!command.compare(0, 4, "next") ||
                 !command.compare(0, 4, "prev")) {
        std::size_t i = 4;
        int amt = 0;
        while (i < command.length()) {
          if (command[i] == ' ') {
            ++i;
            continue;
          }

          if (!std::isdigit(command[i])) {
            throw std::runtime_error(
                "Unable to parse argument to " +
                std::string(command.compare(0, 4, "next") ? "prev" : "next"));
          }

          amt = amt * 10 + (command[i] - '0');
          ++i;
        }

        if (amt == 0)
          amt = 1;

        if (!command.compare(0, 4, "prev"))
          amt = -amt;

        start_cursor.change(amt);
      } else if (!command.compare(0, 5, "check")) {
        std::size_t i = 5;
        while (i < command.length() && command[i] == ' ')
          ++i;

        const auto task = std::find_if(
            task_set.begin(), task_set.end(), [&](const Task &candidate) {
              return command.compare(i, 100, candidate.title()) == 0;
            });

        if (task != task_set.end()) {
          auto node = task_set.extract(task);
          node.value().complete();
          task_set.insert(std::move(node));
        }
      } else if (!command.compare(0, 4, "quit"))
        break;
      else
        throw std::runtime_error("Invalid command!");
    } catch (const std::exception &e) {
      error = e.what();
    }

    Storage::save(event_set, task_set);

    if (!render(error))
      return 1;
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

  return Event(std::stoi(fields[0]), std::stoi(fields[1]),
               std::stoi(fields[2]), parse_time(fields[3]),
               std::stoi(fields[4]), std::stoi(fields[5]),
               std::stoi(fields[6]), parse_time(fields[7]),
               std::move(fields[8]));
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

  return Task(std::stoi(fields[0]), std::stoi(fields[1]),
              std::stoi(fields[2]), parse_time(fields[3]),
              std::move(fields[4]));
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

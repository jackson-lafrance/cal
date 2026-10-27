#include "Display.h"
#include "Event.h"
#include "Task.h"
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
  int month = localTime->tm_mon;
  int year = localTime->tm_year + 1900;

  DateTime start_cursor(day, static_cast<Month>(month), year, 0);

  Events event_set;
  Tasks task_set;
  seed_fake_data(event_set, task_set);

  for (;;) {
    std::string command;

    if (!std::getline(std::cin, command)) {
      break;
    }

    if (!command.compare(0, 3, "add")) {
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
    } else if (!command.compare(0, 4, "quit"))
      break;
    else
      std::cout << "Invalid command!" << std::endl;

    Display::print_screen(event_set, task_set, start_cursor);
  }

  return 0;
}

void seed_fake_data(Events &events, Tasks &tasks) {
  constexpr Year year = 2026;

  const auto at = [](Time hour, Time minute) {
    return hour * 60 + minute;
  };

  // Tuesday, October 27
  events.emplace(27, Month::OCT, year, at(7, 30), 27, Month::OCT, year,
                 at(8, 0), "Coffee and inbox review");
  events.emplace(27, Month::OCT, year, at(8, 30), 27, Month::OCT, year,
                 at(9, 0), "Daily planning");
  events.emplace(27, Month::OCT, year, at(10, 0), 27, Month::OCT, year,
                 at(11, 15), "C++ lecture");
  tasks.emplace(27, Month::OCT, year, at(9, 15),
                "Reply to project emails");
  tasks.emplace(27, Month::OCT, year, at(12, 0), "Submit parser exercise");
  events.emplace(27, Month::OCT, year, at(12, 30), 27, Month::OCT, year,
                 at(13, 15), "Lunch with Alex");
  tasks.emplace(27, Month::OCT, year, at(14, 30),
                "Update calendar README");
  tasks.emplace(27, Month::OCT, year, at(18, 0), "Buy groceries");
  events.emplace(27, Month::OCT, year, at(19, 0), 27, Month::OCT, year,
                 at(20, 30), "Evening gym session");
  tasks.emplace(27, Month::OCT, year, at(21, 30),
                "Prepare tomorrow's notes");

  // Wednesday, October 28
  events.emplace(28, Month::OCT, year, at(7, 45), 28, Month::OCT, year,
                 at(8, 15), "Breakfast and news");
  tasks.emplace(28, Month::OCT, year, at(8, 45), "Pay phone bill");
  events.emplace(28, Month::OCT, year, at(9, 30), 28, Month::OCT, year,
                 at(11, 0), "Project pair programming");
  events.emplace(28, Month::OCT, year, at(11, 30), 28, Month::OCT, year,
                 at(13, 0), "Library study block");
  events.emplace(28, Month::OCT, year, at(14, 0), 28, Month::OCT, year,
                 at(15, 0), "Dentist appointment");
  tasks.emplace(28, Month::OCT, year, at(13, 30),
                "Refactor date validation");
  tasks.emplace(28, Month::OCT, year, at(17, 30),
                "Review DateTime edge cases");
  events.emplace(28, Month::OCT, year, at(18, 0), 28, Month::OCT, year,
                 at(19, 0), "Cook dinner");
  tasks.emplace(28, Month::OCT, year, at(19, 30), "Call family");
  tasks.emplace(28, Month::OCT, year, at(22, 0),
                "Read two C++ chapters");

  // Thursday, October 29
  events.emplace(29, Month::OCT, year, at(7, 30), 29, Month::OCT, year,
                 at(8, 15), "Morning run");
  events.emplace(29, Month::OCT, year, at(8, 30), 29, Month::OCT, year,
                 at(9, 0), "Breakfast");
  tasks.emplace(29, Month::OCT, year, at(9, 30),
                "Check compiler warnings");
  events.emplace(29, Month::OCT, year, at(10, 0), 29, Month::OCT, year,
                 at(11, 20), "Algorithms lecture");
  tasks.emplace(29, Month::OCT, year, at(12, 0),
                "Finish display header design");
  events.emplace(29, Month::OCT, year, at(13, 0), 29, Month::OCT, year,
                 at(15, 30), "Calendar UI implementation");
  tasks.emplace(29, Month::OCT, year, at(16, 0),
                "Send weekly progress update");
  tasks.emplace(29, Month::OCT, year, at(17, 0), "Wash laundry");
  events.emplace(29, Month::OCT, year, at(18, 0), 29, Month::OCT, year,
                 at(19, 0), "Grocery run");
  tasks.emplace(29, Month::OCT, year, at(20, 30),
                "Plan weekend schedule");

  // Friday, October 30
  events.emplace(30, Month::OCT, year, at(7, 30), 30, Month::OCT, year,
                 at(9, 0), "Coffee shop work session");
  events.emplace(30, Month::OCT, year, at(9, 0), 30, Month::OCT, year,
                 at(9, 30), "Team stand-up");
  tasks.emplace(30, Month::OCT, year, at(10, 0), "Push project backup");
  events.emplace(30, Month::OCT, year, at(11, 0), 30, Month::OCT, year,
                 at(12, 30), "Finish terminal layout and wrapping");
  events.emplace(30, Month::OCT, year, at(12, 45), 30, Month::OCT, year,
                 at(13, 30), "Lunch break");
  tasks.emplace(30, Month::OCT, year, at(13, 45),
                "Review command parser errors");
  tasks.emplace(30, Month::OCT, year, at(15, 0),
                "Fix printing alignment bug");
  tasks.emplace(30, Month::OCT, year, at(17, 30), "Clean apartment");
  events.emplace(30, Month::OCT, year, at(20, 0), 30, Month::OCT, year,
                 at(22, 0), "Movie night");
  tasks.emplace(30, Month::OCT, year, at(23, 0),
                "Charge laptop and headphones");

  // Saturday, October 31
  events.emplace(31, Month::OCT, year, at(8, 0), 31, Month::OCT, year,
                 at(9, 0), "Morning walk");
  tasks.emplace(31, Month::OCT, year, at(9, 30),
                "Finish weekend assignment");
  events.emplace(31, Month::OCT, year, at(10, 0), 31, Month::OCT, year,
                 at(11, 30), "Halloween brunch");
  tasks.emplace(31, Month::OCT, year, at(12, 0),
                "Confirm party address");
  events.emplace(31, Month::OCT, year, at(19, 0), 31, Month::OCT, year,
                 at(22, 0), "Costume party");
  tasks.emplace(31, Month::OCT, year, at(13, 0),
                "Pick up Halloween candy");
  events.emplace(31, Month::OCT, year, at(14, 0), 31, Month::OCT, year,
                 at(16, 0), "Pumpkin carving");
  tasks.emplace(31, Month::OCT, year, at(16, 30),
                "Take costume photos");
  events.emplace(31, Month::OCT, year, at(17, 30), 31, Month::OCT, year,
                 at(19, 0), "Dinner with friends");
  tasks.emplace(31, Month::OCT, year, at(23, 0),
                "Submit October retrospective");
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

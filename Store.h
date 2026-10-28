#ifndef STORE_H
#define STORE_H

#include "Event.h"
#include "Task.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace Storage {
inline const std::filesystem::path EVENT_FILE =
    std::filesystem::path(std::getenv("HOME")) / ".local/share/cal++/events";

inline const std::filesystem::path TASK_FILE =
    std::filesystem::path(std::getenv("HOME")) / ".local/share/cal++/tasks";

inline void save(const Events &events, const Tasks &tasks) {
  std::filesystem::create_directories(EVENT_FILE.parent_path());
  std::ofstream efile(EVENT_FILE);
  std::ofstream tfile(TASK_FILE);

  if (!efile || !tfile) {
    throw std::runtime_error("Could not open files for writing!");
  }

  for (const Event &event : events) {
    efile << event.start().day() << ' ' << event.start().month() << ' '
          << event.start().year() << ' ' << event.start().mins() << ' '
          << event.end().day() << ' ' << event.end().month() << ' '
          << event.end().year() << ' ' << event.end().mins() << ' '
          << std::quoted(event.title()) << '\n';
  }

  if (!efile) {
    throw std::runtime_error("Failed to write event file!");
  }

  for (const Task &task : tasks) {
    tfile << task.due().day() << ' ' << task.due().month() << ' '
          << task.due().year() << ' ' << task.due().mins() << ' '
          << task.isDone() << ' ' << std::quoted(task.title()) << '\n';
  }

  if (!tfile) {
    throw std::runtime_error("Failed to write event file!");
  }
}

inline void load(Events &events, Tasks &tasks) {
  std::ifstream efile(EVENT_FILE);
  std::ifstream tfile(TASK_FILE);

  if (!efile || !tfile) {
    return;
  }

  std::string line;
  std::size_t line_number = 0;

  while (std::getline(efile, line)) {
    ++line_number;

    if (line.empty()) {
      continue;
    }

    std::istringstream row(line);

    Day start_day;
    Month start_month;
    Year start_year;
    Time start_mins;
    Day end_day;
    Month end_month;
    Year end_year;
    Time end_mins;
    std::string title;

    if (!(row >> start_day >> start_month >> start_year >> start_mins >>
          end_day >> end_month >> end_year >> end_mins >> std::quoted(title)))
      throw std::runtime_error("Malformed event on line " +
                               std::to_string(line_number));

    if (start_month < 1 || start_month > 12 || end_month < 1 || end_month > 12)
      throw std::runtime_error("Invalid month on line " +
                               std::to_string(line_number));

    events.emplace(start_day, start_month, start_year, start_mins, end_day,
                   end_month, end_year, end_mins, std::move(title));
  }

  if (!efile.eof()) {
    throw std::runtime_error("Failed while reading event file");
  }

  line_number = 0;

  while (std::getline(tfile, line)) {
    ++line_number;

    if (line.empty()) {
      continue;
    }

    std::istringstream row(line);

    Day due_day;
    Month due_month;
    Year due_year;
    Time due_mins;
    bool done = false;
    std::string title;

    if (!(row >> due_day >> due_month >> due_year >> due_mins >> done >>
          std::quoted(title)))
      throw std::runtime_error("Malformed task on line " +
                               std::to_string(line_number));

    if (due_month < 1 || due_month > 12)
      throw std::runtime_error("Invalid month on line " +
                               std::to_string(line_number));

    tasks.emplace(due_day, due_month, due_year, due_mins, std::move(title),
                  done);
  }

  if (!tfile.eof()) {
    throw std::runtime_error("Failed while reading task file");
  }
}
} // namespace Storage

#endif // STORE_H

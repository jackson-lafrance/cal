#ifndef DISPLAY_H
#define DISPLAY_H

#include "Date.h"
#include "Event.h"
#include "Task.h"
#include <iomanip>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

inline const char *CSI = "\033[";
inline const int W = 34;
inline const int H = 24;

class Display {
public:
  static void print_screen(Events event_set, Tasks task_set, DateTime start,
                           const std::string &error) {
    Display display;

    std::cout << CSI << "2J" << CSI << "H";

    display.print_all(event_set, task_set, start, error);

    std::cout << std::flush;
  }

private:
  int top_ = 0;
  int left_ = 0;

  Display() = default;

  void print_empty_day() {
    int starting_top = top_;
    std::cout << "├──────────────────────────┼──────┤";
    top_++;
    for (std::size_t i{0}; i < H; ++i) {
      jump();
      std::cout << "│                          │      │";
      top_++;
    }

    top_ = starting_top;
    jump();
  }

  void print_all(Events event_set, Tasks task_set, DateTime start,
                 const std::string &error) {
    DateTime cursor(start);
    DateTime end(start);

    winsize size{};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1)
      throw std::runtime_error("Could not determine terminal size!");

    if (size.ws_col < 2 * W || size.ws_row < 2 * H)
      throw std::runtime_error("Terminal too small!");

    cursor.change(1);
    end.change((size.ws_col / W - 1) * (size.ws_row / (H + 2)));

    int i = 0;
    int default_top = 0;

    while (start < end) {
      Events event_slice(event_set.lower_bound(start),
                         event_set.lower_bound(cursor));
      Tasks task_slice(task_set.lower_bound(start),
                       task_set.lower_bound(cursor));

      top_ = default_top;
      jump();

      print_empty_day();
      top_++;
      print_day(event_slice, task_slice);

      start.change(1);
      cursor.change(1);
      ++i;
      left_ += W;

      if (!(i % (size.ws_col / W - 1))) {
        left_ = 0;
        default_top += H;
      }
    }

    top_ = size.ws_row - 2;
    left_ = 0;
    jump();
    std::cout << CSI << "2K" << "\r" << error;
    top_++;
    jump();
    std::cout << CSI << "2K" << "\r" << " > ";
  }

  void print_day(Events event_set, Tasks task_set) {
    auto e = event_set.begin();
    auto t = task_set.begin();

    while (e != event_set.end() || t != task_set.end()) {
      jump();
      if (e == event_set.end()) {
        std::cout << "│" << std::left << std::setw(26) << t->title(1) << "│"
                  << t->due().time_string() << " │";
        if (t->title().length() > 26) {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << t->title(2)
                    << "│      │";
        }
        ++t;
      } else if (t == task_set.end()) {
        std::cout << "│" << std::left << std::setw(26) << e->title(1) << "│"
                  << e->start().time_string() << "-│";
        if (e->title().length() > 26) {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << e->title(2)
                    << "│      │";
        } else {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << "" << "│"
                    << e->end().time_string() << " │";
          --top_;
        }
        ++e;
      } else if (e->start() < t->due()) {
        std::cout << "│" << std::left << std::setw(26) << e->title(1) << "│"
                  << e->start().time_string() << "-│";
        if (e->title().length() > 26) {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << e->title(2) << "│"
                    << e->end().time_string() << " │";
        } else {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << "" << "│"
                    << e->end().time_string() << " │";
          --top_;
        }
        ++e;
      } else {
        std::cout << "│" << std::left << std::setw(26) << t->title(1) << "│"
                  << t->due().time_string() << " │";
        if (t->title().length() > 26) {
          ++top_;
          jump();
          std::cout << "│" << std::left << std::setw(26) << t->title(2)
                    << "│      │";
        }
        ++t;
      }

      ++top_;
      ++top_;
    }
  }

  void jump() { std::cout << CSI << (top_ + 1) << ";" << (left_ + 1) << "H"; }
};

#endif // DISPLAY_H

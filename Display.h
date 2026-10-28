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
inline const int INPUT_AREA_HEIGHT = 5;

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
    const bool is_top_row = top_ == 0;
    const bool is_left_column = left_ == 0;

    const char *left_border = is_top_row ? (is_left_column ? "┌" : "┬")
                                         : (is_left_column ? "├" : "┼");
    const char *middle_border = is_top_row ? "┬" : "┼";
    const char *right_border = is_top_row ? "┐" : "┤";

    std::cout << left_border << "──────────────────────────" << middle_border
              << "──────" << right_border;
    top_++;
    for (std::size_t i{0}; i < H - 1; ++i) {
      jump();
      std::cout << "│                          │      │";
      top_++;
    }

    top_ = starting_top + H;
    jump();
    std::cout << (is_left_column ? "└" : "┴")
              << "──────────────────────────┴──────┘";

    top_ = starting_top;
    jump();
  }

  void print_all(Events event_set, Tasks task_set, DateTime start,
                 const std::string &error) {
    DateTime range_start(start);
    DateTime cursor(start);
    DateTime end(start);

    winsize size{};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1)
      throw std::runtime_error("Could not determine terminal size!");

    if (size.ws_col < 2 * W || size.ws_row < 2 * H)
      throw std::runtime_error("Terminal too small!");

    const int day_columns = size.ws_col / W - 1;
    const int workspace_bottom = size.ws_row - INPUT_AREA_HEIGHT;
    const int day_rows = workspace_bottom / H;
    const int grid_bottom = day_rows * H;

    cursor.change(1);
    end.change(day_columns * day_rows);

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

      if (!(i % day_columns)) {
        left_ = 0;
        default_top += H;
      }
    }

    fill_calendar_to_bottom(day_columns, grid_bottom, workspace_bottom);

    left_ = day_columns * W;
    top_ = 0;
    jump();
    Tasks task_slice(task_set.lower_bound(range_start),
                     task_set.lower_bound(end));

    print_tasks(task_slice, size.ws_col, workspace_bottom, grid_bottom);

    print_input_box(size, error, day_columns, workspace_bottom);
  }

  void fill_calendar_to_bottom(int day_columns, int grid_bottom,
                               int workspace_bottom) {
    if (grid_bottom >= workspace_bottom)
      return;

    top_ = grid_bottom;
    for (int column = 0; column < day_columns; ++column) {
      left_ = column * W;
      jump();
      std::cout << (column == 0 ? "├" : "┼")
                << "──────────────────────────┼──────┤";
    }

    for (top_ = grid_bottom + 1; top_ < workspace_bottom; ++top_) {
      for (int column = 0; column < day_columns; ++column) {
        left_ = column * W;
        jump();
        std::cout << "│                          │      │";
      }
    }
  }

  void print_input_box(const winsize &size, const std::string &error,
                       int day_columns, int workspace_bottom) {
    const int box_width = size.ws_col - 1;
    const int content_width = box_width - 2;
    const bool has_error = !error.empty();

    top_ = workspace_bottom;
    left_ = 0;
    jump();
    if (has_error)
      std::cout << CSI << "31m";
    print_workspace_border(box_width, day_columns);

    top_++;
    jump();
    std::cout << "│" << std::left << std::setw(content_width)
              << error.substr(0, content_width) << "│";

    top_++;
    jump();
    print_horizontal_border("├", "┤", box_width);
    if (has_error)
      std::cout << CSI << "0m";

    top_++;
    jump();
    std::cout << "│ > " << std::string(box_width - 5, ' ') << "│";

    top_++;
    jump();
    print_horizontal_border("└", "┘", box_width);

    top_ = workspace_bottom + 3;
    left_ = 4;
    jump();
  }

  void print_workspace_border(int width, int day_columns) {
    std::cout << "├";
    for (int column = 1; column < width - 1; ++column) {
      const bool joins_calendar =
          column <= day_columns * W &&
          (!(column % W) || column % W == 27);
      std::cout << (joins_calendar ? "┴" : "─");
    }
    std::cout << "┤";
  }

  void print_horizontal_border(const char *left_border,
                               const char *right_border, int width) {
    std::cout << left_border;
    for (int i = 0; i < width - 2; ++i)
      std::cout << "─";
    std::cout << right_border;
  }

  void print_tasks(Tasks task_set, int width, int workspace_bottom,
                   int grid_bottom) {
    const int panel_width = width - left_ - 1;
    const int content_width = panel_width - 2;

    jump();
    print_horizontal_border("┬", "┐", panel_width);
    top_++;

    const int time_width = 6;
    const int date_width = 11;
    const int first_title_width = content_width - time_width;
    const int second_title_width = content_width - date_width;

    for (const auto &task : task_set) {
      const int title_length = static_cast<int>(task.title().length());
      const int remaining_title_length =
          title_length - first_title_width - second_title_width;
      const int extra_line_count =
          remaining_title_length > 0
              ? (remaining_title_length + content_width - 1) / content_width
              : 0;
      const int line_count = 2 + extra_line_count;
      const int required_rows = line_count + 1;

      if (top_ + required_rows > workspace_bottom)
        break;

      std::size_t offset = 0;
      for (int line = 0; line < line_count; ++line) {
        const int title_width = line == 0   ? first_title_width
                                : line == 1 ? second_title_width
                                            : content_width;

        jump();
        const char *left_border =
            top_ <= grid_bottom && !(top_ % H) ? "┤" : "│";
        const std::string title_segment =
            offset < task.title().length()
                ? task.title().substr(offset, title_width)
                : "";
        std::cout << left_border << std::left << std::setw(title_width)
                  << title_segment;

        if (line == 0)
          std::cout << " " << task.due().time_string();
        else if (line == 1)
          std::cout << " " << task.due().date_string();

        std::cout << "│";
        offset += title_width;
        top_++;
      }

      jump();
      const char *left_border =
          top_ <= grid_bottom && !(top_ % H) ? "┤" : "│";
      std::cout << left_border << std::string(content_width, ' ') << "│";
    }

    while (top_ < workspace_bottom) {
      jump();
      const char *left_border =
          top_ <= grid_bottom && !(top_ % H) ? "┤" : "│";
      std::cout << left_border << std::string(content_width, ' ') << "│";
      top_++;
    }
  }

  void print_day(Events event_set, Tasks task_set) {
    auto e = event_set.begin();
    auto t = task_set.begin();
    const int day_bottom = top_ + H - 1;

    while (e != event_set.end() || t != task_set.end()) {
      const bool task_is_next =
          e == event_set.end() ||
          (t != task_set.end() && !(e->start() < t->due()));
      const int rows_needed =
          task_is_next && t->title().length() <= 26 ? 1 : 2;

      if (top_ + rows_needed > day_bottom)
        break;

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

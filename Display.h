#ifndef DISPLAY_H
#define DISPLAY_H

#include "Event.h"
#include "Task.h"

namespace Display {
inline void print_events_and_tasks(Events event_set, Tasks task_set) {
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
      std::cout << "TASK " << t->title() << ": " << t->due().to_string() << std::endl;
      ++t;
    }
  }
}
} // namespace Display

#endif // DISPLAY_H

/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_TASK_SCHEDULER_H
#define SRSLTE_TASK_SCHEDULER_H

#include "interfaces_common.h"
#include "multiqueue.h"
#include "thread_pool.h"

namespace srslte {

class task_scheduler : public srslte::task_handler_interface
{
public:
  explicit task_scheduler(uint32_t default_extern_tasks_size = 512,
                          uint32_t nof_background_threads    = 0,
                          uint32_t nof_timers_prealloc       = 100) :
    external_tasks{default_extern_tasks_size},
    timers{nof_timers_prealloc},
    background_tasks{nof_background_threads}
  {
    background_queue_id = external_tasks.add_queue();

    // Start background thread
    if (background_tasks.nof_workers() > 0) {
      background_tasks.start();
    }
  }

  srslte::timer_handler::unique_timer get_unique_timer() final { return timers.get_unique_timer(); }

  //! Creates new queue for tasks coming from external thread
  srslte::task_multiqueue::queue_handler make_task_queue() final { return external_tasks.get_queue_handler(); }
  srslte::task_multiqueue::queue_handler make_task_queue(uint32_t size) final
  {
    return external_tasks.get_queue_handler(size);
  }

  //! Delays a task processing by duration_ms
  void defer_callback(uint32_t duration_ms, std::function<void()> func) final
  {
    timers.defer_callback(duration_ms, func);
  }

  //! Enqueues internal task to be run in next tic
  void defer_task(srslte::move_task_t func) final { internal_tasks.push_back(std::move(func)); }

  //! Delegates a task to a thread pool that runs in the background
  void enqueue_background_task(std::function<void(uint32_t)> f) final
  {
    if (background_tasks.nof_workers() > 0) {
      background_tasks.push_task(std::move(f));
    } else {
      external_tasks.push(background_queue_id,
                          std::bind([](const std::function<void(uint32_t)>& task) { task(-1); }, std::move(f)));
    }
  }

  //! Defer the handling of the result of a background task to next tic
  void notify_background_task_result(srslte::move_task_t task) final
  {
    // run the notification in next tic
    external_tasks.push(background_queue_id, std::move(task));
  }

  //! Updates timers, and run any pending internal tasks.
  //  CAUTION: Should be called in main thread
  void tic()
  {
    timers.step_all();
    run_all_internal_tasks();
  }

  //! Processes the next task in the multiqueue.
  //  CAUTION: This is a blocking call
  bool run_next_external_task()
  {
    srslte::move_task_t task{};
    if (external_tasks.wait_pop(&task) >= 0) {
      task();
      return true;
    }
    return false;
  }

  //! Processes the next task in the multiqueue if it exists.
  bool try_run_next_external_task()
  {
    srslte::move_task_t task{};
    if (external_tasks.try_pop(&task) >= 0) {
      task();
      return true;
    }
    return false;
  }

  srslte::timer_handler* get_timer_handler() { return &timers; }

private:
  void run_all_internal_tasks()
  {
    // Perform pending stack deferred tasks
    // Note: Keep it indexed-based, bc a task may enqueue another task, which may cause vector reallocation,
    //       and iterator invalidation
    for (size_t i = 0; i < internal_tasks.size(); ++i) {
      internal_tasks[i]();
    }
    internal_tasks.clear();
  }

  int                      background_queue_id = -1; ///< Queue for handling the outcomes of tasks run in the background
  srslte::task_multiqueue  external_tasks;
  srslte::timer_handler    timers;
  srslte::task_thread_pool background_tasks;       ///< Thread pool used for long, low-priority tasks
  std::vector<srslte::move_task_t> internal_tasks; ///< enqueues stack tasks from within main thread. Avoids locking
};

//! Handle to provide to classes/functions running within main thread
class task_sched_handle
{
public:
  task_sched_handle(task_scheduler* sched_) : sched(sched_) {}

  srslte::timer_handler::unique_timer get_unique_timer() { return sched->get_unique_timer(); }
  void enqueue_background_task(std::function<void(uint32_t)> f) { sched->enqueue_background_task(std::move(f)); }
  void notify_background_task_result(srslte::move_task_t task)
  {
    sched->notify_background_task_result(std::move(task));
  }
  void defer_callback(uint32_t duration_ms, std::function<void()> func)
  {
    sched->defer_callback(duration_ms, std::move(func));
  }
  void defer_task(srslte::move_task_t func) { sched->defer_task(std::move(func)); }

private:
  task_scheduler* sched;
};

} // namespace srslte

#endif // SRSLTE_TASK_SCHEDULER_H
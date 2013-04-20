//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __DCXSCHEDULER_H__
#define __DCXSCHEDULER_H__



namespace dcx {

template <typename TIME_TYPE,
          const size_t NUMBER_OF_TASKS> class dcxSchedulerB {
public:
  typedef TIME_TYPE time_t;

public:

  dcxSchedulerB(time_t current_time, time_t period) : period_(period) {
    next_task_ = 0;
    time_t offset = (period_ / NUMBER_OF_TASKS) / 2;
    for (unsigned int i = 0; i < NUMBER_OF_TASKS; ++i) {
      last_task_times_[i] = current_time + offset + (i * period_) / NUMBER_OF_TASKS;
    }
  }

public:

  // Returns the next task to be executed, if it is necessary to do so.
  unsigned int poll(time_t time,
                    time_t* time_since_last_executed) {
    unsigned int task_index = next_task_;
    time_t delay = time - last_task_times_[task_index];
    if (delay < period_) return unsigned int(-1);
    last_task_times_[task_index] = time;
    *time_since_last_executed = delay;
    next_task_ = (task_index + 1) % NUMBER_OF_TASKS;
    return task_index;
  }

private:
  time_t last_task_times_[NUMBER_OF_TASKS];
  unsigned int next_task_;
  const time_t period_;
};



// Tasks in a scheduler group are always performed sequentially--no task
// will be executed more than once more than any other task.
template <typename TIME_TYPE> class dcxSchedulerGroup {
public:
  typedef TIME_TYPE time_t;

public:
  void configure(time_t period, unsigned int number_of_tasks) {
    period_ = period;
    task_ids_ = new int[number_of_tasks];
    last_task_times_ = new time_t[number_of_tasks];
    number_of_tasks_ = number_of_tasks;
    next_task_ = 0;
  }

  void assignTaskID(unsigned int index, int task_id) {
    task_ids_[index] = task_id;
    last_task_times_[index] = time_t(0);
  }

  void assignDefaultTaskTimes(time_t current_time) {
    time_t offset = period_ / number_of_tasks_ / 2;
    for (unsigned int i = 0; i < number_of_tasks_; ++i) {
      last_task_times_[i] = current_time + offset + (i * period_) / number_of_tasks_;
    }
  }

public:

  // Returns the next task to be executed, if it is necessary to do so.
  bool nextTask(time_t time,
                int* task_id,
                time_t* time_since_last_executed) {
    unsigned int task_index = next_task_;
    time_t delay = time - last_task_times_[task_index];
    if (delay < period_) return false;
    last_task_times_[task_index] = time;
    *task_id = task_ids_[task_index];
    *time_since_last_executed = delay;
    next_task_ = (task_index + 1) % number_of_tasks_;
    return true;
  }

private:
  time_t period_;
  int* task_ids_;
  time_t* last_task_times_;
  unsigned int number_of_tasks_;
  unsigned int next_task_;
};


}

#endif

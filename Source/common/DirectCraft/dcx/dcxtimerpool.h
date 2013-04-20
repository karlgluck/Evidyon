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
#ifndef __DCXTIMERS_H__
#define __DCXTIMERS_H__

#include "dcxclock.h"
#include "dcxmanagedmemorypool.h"

namespace dcx {

/*****************************   Timer Pool  **********************************
  This is a really cool class that makes it easy and efficient to handle as
  many asynchronous timers as you want.  A timer is polled to see if it
  has expired and only does so once.  If you need something that occurs
  repeatedly, you want a scheduler/task not timer pool/timers.

  Usage:
    typedef dcxTimerPool<10> TimerPool;
    TimerPool timers;
    timers.update(time());
    TimerPool::Timer timer;
    timers.createDelayedTimer(15.0, &timer);
    ...
    if (timer.expired()) {
      // do something!
    }


Scheduler
  Task
    update(Scheduler* scheduler) {
      if (timer.expired()) {
        scheduler->create(timer.getExpirationTime() + period);
        times_to_execute = (times_to_execute <= 0) ? 1 : (times_to_execute + 1);
      }
    }
    bool execute() {
      if (times_to_execute <= 0) return false;
      --times_to_execute;
      return true;
    }

  createTask(Task* task, double time_offset);

StaticTaskScheduler<Tasks>
  Task next(time_t time, time_t* time_since_last_execution) {
  }





Scheduler scheduler;
while (true) {
  switch (scheduler.next(time(), &time_since_last_execution)) {
  case MANAGER_CLIENTS:
  case MANAGER_LIFEFORM_AI:
  case MANAGER_MAGIC:
  case MANAGER_WORLD:
  case MANAGER_
  }
}


*/
template <const size_t TIMERS, typename TIME_TYPE> class dcxTimerPool {
public:
  typedef TIME_TYPE time_t;

private:
  class TimerContainer {
  public:

    // Puts this container into the linked list of timer containers.  It
    // sorts itself into the list by the time at which it expires, and
    // returns the new head of the list.
    TimerContainer* create(time_t expires,
                           TimerContainer* first_container) {
      back_ = NULL;
      front_ = NULL;

      TimerContainer* container = first_container;
      while (container) {
        if (container->back_) {
          if (container->back_->expires_ > expires) {
            container->back_ = this;
            container->back_->front_ = this;
            front_ = first_container;
            back_ = first_container->back_;
            break;
          }
        } else {
          container->back_ = this;
          //back_ = NULL;
          front_ = container;
          break;
        }
        container = container->back_;
      }

      return (first_container == NULL || first_container == back_) ? this : first_container;
    }

    // Removes this timer from the linked list.  This method returns the
    // timer that should now be at the start of the list.
    TimerContainer* destroy(TimerContainer* first_container) {
      // remove from the linked list
      if (back_) {
        back_->front_ = front_;
      }
      if (front_) {
        front_->back_ = back_;
      }
      if (this == first_container) {
        return back_;
      } else {
        return first_container;
      }
    }

    bool expired(time_t current_time) const {
      return current_time > expires_;
    }

  private:
    time_t expires_;
    TimerContainer* front_;
    TimerContainer* back_;
  };

  typedef dcx::dcxManagedMemoryPoolPointer<TimerContainer> TimerContainerPointer;

public:
  class Timer {
    friend class dcxTimerPool<TIMERS>;
  public:
    bool expired() const {
      return pointer_.invalid();
    }

  private:
    TimerContainerPointer pointer_;
  };

public:

  dcxTimerPool() {
    last_update_time_ = 0.0;
    head_of_list_ = NULL;
  }

  // Allocates the timer pool's memory and initializes the structure.
  // Be sure to call update() to set the last update time before
  // adding any delay timers!
  void create() {
    pool_.create();
    last_update_time_ = 0.0;
    head_of_list_ = NULL;
  }

  // Erases this class.  Accessing a timer will now cause memory access
  // errors, so only do this when absolutely sure the pool will not
  // be used again.
  void destroy() {
    head_of_list_ = NULL;
    pool_.destroy();
  }

  // Generates a new timer that expires a given delay into the future
  bool createDelayTimer(time_t time_from_last_update, Timer* timer) {
    return createAbsoluteTimer(time_from_last_update + last_update_time_);
  }

  // Creates a new timer that expires at a defined time
  bool createAbsoluteTimer(time_t time, Timer* timer) {
    if (!timer) return false;
    if (!pool_.acquire(&timer->pointer_)) return false;
    head_of_list_ = timer->pointer_.dereferenceAssumingValid()->create(
        time,
        head_of_list_);
    return true;
  }

  // Forcibly terminates the timer.  All references to this timer will
  // now indicate that it has expired.
  void terminateTimer(Timer* timer) {
    if (!timer) return;
    TimerContainer* container = timer->pointer_.dereference();
    if (!container) return;
    head_of_list_ = container->destroy(head_of_list_);
  }

  // Updates the state of the pool.  Timers that have expired will
  // now indicate this fact.
  void update(time_t current_time) {
    last_update_time_ = current_time;
    while (head_of_list_ && head_of_list_->expired(current_time)) {
      head_of_list_ = head_of_list_->destroy(head_of_list_);
    }
  }

private:
  time_t last_update_time_;
  dcx::dcxManagedMemoryPool<TimerController> pool_;
  TimerContainer* head_of_list_;
};

}


#endif
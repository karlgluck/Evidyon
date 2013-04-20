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
#include "timermanager.h"
#include "timerimpl.h"
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <math.h>


namespace Evidyon {
namespace Time {

TimerManager* TimerManager::singleton = NULL;



//----[  TimerManager  ]-------------------------------------------------------
TimerManager::TimerManager(double base_time) {
  assert(!singleton);
  singleton = this;
  base_time_ = base_time;
  memset(next_update_, 0, sizeof(next_update_));
}



//----[  ~TimerManager  ]------------------------------------------------------
TimerManager::~TimerManager() {
  singleton = NULL;
}



//----[  update  ]-------------------------------------------------------------
void TimerManager::update(double time) {
  double timer_time = (time - base_time_) * 1000.0;
  double milli_time = floor(timer_time);
  Milliseconds millis = Milliseconds(milli_time);

  Timers::iterator i, end;

#define UPDATE(ms)\
  if (millis >= next_update_[RESOLUTION_##ms##_MS]) {\
  next_update_[RESOLUTION_##ms##_MS] = millis + (ms/2 > 0 ? ms/2 : 1);\
    end = timers_[RESOLUTION_##ms##_MS].end();\
    for (i = timers_[RESOLUTION_##ms##_MS].begin(); i != end; ++i) {\
    TimerImpl* impl = *i;\
    if (!impl->armed_ && impl->set_) impl->expiration_time_ += (millis > ms/2) ? (millis - ms/2) : ms/2;\
    impl->expired_ = impl->expiration_time_ <= millis;\
    impl->armed_ = true;\
    }

  UPDATE(1)
    // account for missing fractional part
    base_time_ -= (timer_time - milli_time)/1000.0;
    UPDATE(10)
      UPDATE(100)
        UPDATE(500)
          UPDATE(60000)
          }
        }
      }
    }
  }
}



//----[  add  ]----------------------------------------------------------------
void TimerManager::add(TimerResolution resolution, TimerImpl* timer) {
  assert(singleton);
  Timers::_Pairib result = singleton->timers_[resolution].insert(timer);
  assert(result.second);
}



//----[  remove  ]-------------------------------------------------------------
void TimerManager::remove(TimerResolution resolution, TimerImpl* timer) {
  assert(singleton);
  singleton->timers_[resolution].erase(timer);
}



}
}


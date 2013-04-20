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
#ifndef __SHARED_TIME_TIMERIMPL_H__
#define __SHARED_TIME_TIMERIMPL_H__
#pragma once

#include "timerresolution.h"

namespace Evidyon {
namespace Time {
template<const TimerResolution> class Timer;
}
}

namespace Evidyon {
namespace Time {

  
//----[  TimerImpl  ]----------------------------------------------------------
class TimerImpl {
  friend class TimerManager;
  friend class Timer<RESOLUTION_1_MS>;
  friend class Timer<RESOLUTION_10_MS>;
  friend class Timer<RESOLUTION_100_MS>;
  friend class Timer<RESOLUTION_500_MS>;
  friend class Timer<RESOLUTION_60000_MS>;
  inline TimerImpl() { reset(); }
public:

  inline void set(Milliseconds delay) {
    set_ = true;
    expired_ = false;
    armed_ = false;
    expiration_time_ = delay;
  }
  
  inline void reset() {
    expiration_time_ = 0;
    set_ = false;
    armed_ = true;
    expired_ = true;
  }

  inline bool expired() const { return set_ && expired_; }

  inline bool poll() {
    if (set_ && expired_) {
      set_ = false;
      return true;
    } else {
      return false;
    }
  }

  inline void trigger() {
    expiration_time_ = 0;
    set_ = true;
    armed_ = true;
    expired_ = true;
  }

  inline void earliestTime(const TimerImpl* other) {
    if (!other->set_) return;
    if (!set_) {
      set_ = true;
      armed_ = false;
      expired_ = false;
      expiration_time_ = other->expiration_time_;
    } else {
      if (expiration_time_ >= other->expiration_time_) {
        expiration_time_ = other->expiration_time_;
      }
    }
  }

private:
  Milliseconds expiration_time_;
  bool set_;
  bool armed_;
  bool expired_;
};


}
}

#endif
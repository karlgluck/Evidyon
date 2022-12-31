//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __SHARED_TIME_TIMER_H__
#define __SHARED_TIME_TIMER_H__
#pragma once

#include "timerresolution.h"
#include "timermanager.h"
#include "timerimpl.h"

namespace Evidyon {
namespace Time {


  
//----[  Timer  ]--------------------------------------------------------------
template <const TimerResolution _R> class Timer {
public:
  __forceinline void enable() { /*reset();*/ TimerManager::add(_R, &impl_); }
  __forceinline void enable(Milliseconds delay) { impl_.set(delay); TimerManager::add(_R, &impl_); }
  __forceinline void disable() { reset(); TimerManager::remove(_R, &impl_); }
  __forceinline void set(Milliseconds delay) { impl_.set(delay); }
  __forceinline void reset() { impl_.reset(); }
  __forceinline bool expired() const { return impl_.expired(); }
  __forceinline bool poll() { return impl_.poll(); }
  __forceinline void trigger() { impl_.trigger(); }
  template <const TimerResolution _oR> __forceinline void earliestTime(const Timer<_oR>* other) { impl_.earliestTime(&other->impl_); }
private:
  TimerImpl impl_;
};

}
}

#endif
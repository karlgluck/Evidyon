//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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
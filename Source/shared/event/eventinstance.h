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
#ifndef __SHARED_EVENT_EVENTINSTANCE_H__
#define __SHARED_EVENT_EVENTINSTANCE_H__
#pragma once

namespace Evidyon {
namespace SpecialFX {
class SpecialFXManager;
}
}

namespace Evidyon {
namespace Event {

//----[  EventInstance  ]------------------------------------------------------
class EventInstance {
public:
  virtual ~EventInstance() = 0;

  // When update() returns 'true', this instance will be destroyed.  The
  // terminate() method is NOT invoked in this case.
  virtual bool update(double time, double time_since_last_update) = 0;

  // Called when the event is terminated forcibly (not by update() returning
  // false)
  virtual void terminate(double time,
                         SpecialFX::SpecialFXManager* special_fx_manager) = 0;
};

}
}

#endif
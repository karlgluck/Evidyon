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
#ifndef __MAPCOMPILATIONCONTEXT_H__
#define __MAPCOMPILATIONCONTEXT_H__
#pragma once



#include "maplocationdata.h"
#include <vector>
#include "shared/trigger/trigger.h"
#include "shared/trigger/triggerindex.h"

namespace Evidyon {
namespace World {
namespace Tools {

//----[  MapCompilationContext  ]----------------------------------------------
struct MapCompilationContext {
  unsigned int width, height;
  MapLocationData* locations;

  // Edge locations reference this location
  MapLocationData default_location;

  // Gets the map location data structure for the given coordinates.  If the
  // coordinates are beyond the size of the map, this method returns a
  // reference to the default location.
  MapLocationData* atOrDefault(int x, int y);

  // Gets the map location data structure for the given coordinates, or
  // returns NULL if the coordinates are invalid for this context.
  MapLocationData* atOrNull(int x, int y);

  // Set to true whenever atOrDefault or atOrNull is invoked.  This can be used
  // to monitor changes to the context.
  bool accessed;

  // All of the triggers on the map in the most recently compiled region of
  // the map.  This list is cleared before each content build.
  typedef std::vector<Trigger::Trigger> Triggers;
  Triggers triggers;

  // Adds a new trigger to the context.  Returns the trigger's index.
  Trigger::TriggerIndex addTrigger(const Trigger::Trigger* trigger);

  // Writes out all of the triggers into the given array.  The array
  // should be triggers.size() in length or greater.
  size_t compileTriggers(Trigger::Trigger* triggers, size_t buffer_length);
};

}
}
}

#endif
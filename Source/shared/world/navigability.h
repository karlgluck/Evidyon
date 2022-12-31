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
#ifndef __NAVIGABILITY_H__
#define __NAVIGABILITY_H__
#pragma once



namespace Evidyon {
namespace World {

//----[  Navigability  ]-------------------------------------------------------
// Defines how players can interact with map locations.
enum Navigability {
  NAVIGABILITY_NORMAL,            // walk, teleport, fly, etc.
  NAVIGABILITY_NO_TELEPORTING,    // can't teleport on, from or over
  NAVIGABILITY_WADE,              // move character down
  NAVIGABILITY_SWIM,              // water is too deep to wade; must swim
  NAVIGABILITY_PIT,               // fly over, otherwise player falls & dies
  NAVIGABILITY_LOWWALL,           // fly over but can't walk through
  NAVIGABILITY_HIGHWALL,          // must teleport over this location
  NAVIGABILITY_SAFEZONE,          // NORMAL, plus safe-zone behavior ///////////// TODO: ERASE, REPLACED w/ ZONE RULES
  NAVIGABILITY_IMPASSABLE,        // high wall + no teleporting
  NAVIGABILITY_INVALID,
};



//----[  NavigabilityString  ]-------------------------------------------------
const char* NavigabilityString(Navigability navigability);

//----[  NavigabilitySolidToWalking  ]-----------------------------------------
// Can a tile with the given navigability be moved over by an entity that
// cannot fly or swim?
bool NavigabilitySolidToWalking(Navigability navigability);

//----[  NavigabilitySolidToFlying  ]------------------------------------------
// Can a tile with the given navigability be flown over?
bool NavigabilitySolidToFlying(Navigability navigability);

//----[  NavigabilitySolidToSwimming  ]----------------------------------------
// Can an entity that is only able to swim move over the given navigability?
bool NavigabilitySolidToSwimming(Navigability navigability);

}
}

#endif
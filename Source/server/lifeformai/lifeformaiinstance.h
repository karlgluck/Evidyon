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
#ifndef __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_H__
#define __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_H__
#pragma once

#include "shared/avatar/avatarlevel.h"
#include "common/number.h"

namespace Evidyon {
class Map;
class GlobalActorManager;
class WorldRegion;
namespace LifeformAI {
struct LifeformAI;
}
}

namespace Evidyon {
namespace Server {




//----[  LifeformAIInstance  ]-------------------------------------------------
class LifeformAIInstance {
public:

  // Generates a new instance of whatever lifeform time is specified by the
  // 'description' parameter.
  static LifeformAIInstance* New(
    Avatar::AvatarLevel intended_level,
    Number x,
    Number z,
    const LifeformAI::LifeformAI* description);

public:
  virtual bool acquire(Map* map,
                       GlobalActorManager* actor_manager) = 0;

  // If this method returns 'false', the lifeform will be released by the
  // manager.  It is possible that this method will be called after release()
  // is invoked, so be sure to validate the state of the instance.
  virtual bool update(double time, double time_since_last_update) = 0; 

  // Returns whether or not this lifeform instance needs to be released when
  // the world region in the argument is being deallocated.
  virtual bool shouldReleaseWithWorldRegion(WorldRegion* region) = 0;

  // Called when the spawn point that created this lifeform is being erased.
  // This should cause the actor to portal itself out of the world and
  // eventually return 'false' from update().
  virtual void despawn() = 0;

  // Frees the data used by this lifeform.  May be called multiple times.
  virtual void release(GlobalActorManager* actor_manager) = 0;
};



}
}

#endif
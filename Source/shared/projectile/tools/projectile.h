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
#ifndef __SHARED_PROJECTILE_TOOLS_PROJECTILE_H__
#define __SHARED_PROJECTILE_TOOLS_PROJECTILE_H__
#pragma once

#include <dc/boolean>
#include <dc/float>
#include "shared/specialfx/tools/specialfx.h"

namespace Evidyon {
namespace Event {
namespace Tools {
class EventCompiler;
}
}
namespace Projectile {
struct Projectile;
}
}

namespace Evidyon {
namespace Projectile {
namespace Tools {



//----[  Projectile  ]---------------------------------------------------------
class Projectile : public dc::dcResource<Projectile> {
public:
  Projectile();
  void compile(Event::Tools::EventCompiler* compiler,
               Evidyon::Projectile::Projectile* description);
  float getRange() const;

private:
  dc::dcFloat speed_;
  dc::dcFloat range_;
  dc::dcFloat homing_rate_; // set to 0 to disable
  dc::dcFloat radius_;
  dc::dcBoolean bouncing_;  // rebound off of objects, play intermediate fx
  dc::dcBoolean piercing_;  // go through actors, play intermediate fx
  Evidyon::Tools::SpecialFXReference creation_special_fx_;
  Evidyon::Tools::SpecialFXReference intermediate_special_fx_;
  dc::dcFloat intermediate_special_fx_duration_;
  Evidyon::Tools::SpecialFXReference destruction_special_fx_;
  dc::dcFloat destruction_special_fx_duration_;
};



}
}
}

#endif
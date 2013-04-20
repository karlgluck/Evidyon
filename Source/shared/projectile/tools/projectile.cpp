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
#include "projectile.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/event/event_projectile.h"
#include "shared/projectile/projectile.h"

namespace Evidyon {
namespace Projectile {
namespace Tools {


  
//----[  Projectile  ]---------------------------------------------------------
Projectile::Projectile() {
  member("Speed", &speed_);
  member("Range", &range_);
  member("Homing Rate", &homing_rate_);
  member("Radius", &radius_);
  member("Bouncing", &bouncing_);
  member("Piercing", &piercing_);
  member("Projectile SpecialFX", &creation_special_fx_);
  member("Intermediate SpecialFX", &intermediate_special_fx_);
  member("Intermediate FX Duration", &intermediate_special_fx_duration_);
  member("Destruction SpecialFX", &destruction_special_fx_);
  member("Destruction FX Duration", &destruction_special_fx_duration_);

  speed_.setValue(5.0f);
  range_.setValue(10.0f);
  radius_.setValue(0.5f);
}



//----[  compile  ]------------------------------------------------------------
void Projectile::compile(Event::Tools::EventCompiler* compiler,
                         Evidyon::Projectile::Projectile* description) {
  Event::Event_Projectile projectile_event;
  projectile_event.creation_special_fx
    = creation_special_fx_.getReferencedResourceIndex();
  projectile_event.lifetime = range_.getValue() / speed_.getValue();
  projectile_event.speed = speed_.getValue();
  projectile_event.intermediate_special_fx
    = intermediate_special_fx_.getReferencedResourceIndex();
  projectile_event.intermediate_special_fx_duration
    = intermediate_special_fx_duration_.getValue();
  projectile_event.destruction_special_fx
    = destruction_special_fx_.getReferencedResourceIndex();
  projectile_event.destruction_special_fx_duration
    = destruction_special_fx_duration_.getValue();
  
  description->event_index = compiler->add(&projectile_event);
  description->speed = projectile_event.speed;
  description->duration
    = Time::Milliseconds(1000.0 * projectile_event.lifetime);
  description->radius = radius_.getValue();
  description->homing = homing_rate_.getValue() > 0.0f;
  description->homing_rate = homing_rate_.getValue();
  description->bouncing = bouncing_.getValue();
  description->piercing = piercing_.getValue();
  description->piercing_period
    = Time::Milliseconds(1.0 / projectile_event.speed);
  
}




//----[  getRange  ]-----------------------------------------------------------
float Projectile::getRange() const {
  return range_.getValue();
}



}
}
}


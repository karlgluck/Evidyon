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
#include "specialfxemitterinstance.h"
#include "specialfxemitter.h"
#include "specialfxemitterinstance_interval.h"
#include <assert.h>
#include "common/safemacros.h"

namespace Evidyon {
namespace SpecialFX {


  
//----[  New  ]----------------------------------------------------------------
SpecialFXEmitterInstance* SpecialFXEmitterInstance::New(
    double time,
    double generate_expiration_time,
    float intensity,
    const SpecialFXEmitter* data,
    SpecialFXBindingSource* source) {

  SpecialFXEmitterInstance* instance = NULL;
  switch (data->type) {
  default:
    assert(false);
    return NULL;
  case SpecialFXEmitter::INTERVAL: {
      instance = new SpecialFXEmitterInstance_Interval(
          time,
          generate_expiration_time,
          intensity,
          &data->interval);
    } break;
  }

  if (!instance->initialize(source)) {
    SAFE_DELETE(instance); // instance = NULL
  }

  return instance;
}



//----[  ~SpecialFXEmitterInstance  ]------------------------------------------
SpecialFXEmitterInstance::~SpecialFXEmitterInstance() {
}



}
}


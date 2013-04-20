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
#ifndef __SHARED_SPECIALFX_SPECIALFXEMITTERINSTANCE_H__
#define __SHARED_SPECIALFX_SPECIALFXEMITTERINSTANCE_H__
#pragma once


#include "specialfx.h"

namespace Evidyon {
namespace SpecialFX {
class SpecialFXManager;
struct SpecialFXEmitter;
class SpecialFXBindingSource;
}
}

namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXEmitterInstance  ]-------------------------------------------
class SpecialFXEmitterInstance {
public:
  static SpecialFXEmitterInstance* New(double time,
                                       double generate_expiration_time,
                                       float intensity,
                                       const SpecialFXEmitter* data,
                                       SpecialFXBindingSource* source);

public:
  virtual ~SpecialFXEmitterInstance();
  virtual bool initialize(SpecialFXBindingSource* source) = 0;

  // Returns 'false' when the emitter has expired and should be deallocated.
  virtual bool update(double time,
                      double time_since_last_update,
                      SpecialFXManager* special_fx_manager) = 0;
};

}
}

#endif
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
#ifndef __SHARED_SPECIALFX_TOOLS_SPECIALFXEMITTER_INTERVAL_H__
#define __SHARED_SPECIALFX_TOOLS_SPECIALFXEMITTER_INTERVAL_H__
#pragma once

#include "specialfx.h"
#include "specialfxemitter.h"
#include "specialfxbinding.h"
#include <dc/float>
#include <dc/integer>


namespace Evidyon {
namespace SpecialFX {
namespace Tools {

//----[  SpecialFXEmitter_Interval  ]------------------------------------------
class SpecialFXEmitter_Interval
  : public dc::dcResource<SpecialFXEmitter_Interval, SpecialFXEmitter> {
  static dc::dcClass<SpecialFXEmitter_Interval>
           ::Implements<SpecialFXEmitter> implementsSpecialFXEmitter;
public:
  static std::string staticTypeName() { return "SpecialFXEmitter_Interval"; }
public:
  SpecialFXEmitter_Interval();
  virtual SpecialFXEmitterIndex compile(SpecialFXCompiler* compiler);
private:
  SpecialFXInterface special_fx_;
  dc::dcFloat lifetime_;
  dc::dcFloat emit_period_;
  dc::dcInterface<SpecialFXBinding> source_binding_;
  dc::dcInterface<SpecialFXBinding> target_binding_;
  dc::dcInteger maximum_number_to_emit_;
};

}
}
}

#endif
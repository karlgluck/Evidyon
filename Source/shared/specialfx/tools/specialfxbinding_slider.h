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
#ifndef __SHARED_SPECIALFX_TOOLS_SPECIALFXBINDING_SLIDER_H__
#define __SHARED_SPECIALFX_TOOLS_SPECIALFXBINDING_SLIDER_H__
#pragma once

#include "specialfxbinding.h"
#include <dc/interface>
#include <dc/float>


namespace Evidyon {
namespace SpecialFX {
namespace Tools {

//----[  SpecialFXBinding_Slider  ]--------------------------------------------
class SpecialFXBinding_Slider
  : public dc::dcResource<SpecialFXBinding_Slider, SpecialFXBinding> {
  static dc::dcClass<SpecialFXBinding_Slider>
           ::Implements<SpecialFXBinding> implementsSpecialFXBinding;
public:
  static std::string staticTypeName() { return "SpecialFXBinding_Slider"; }
public:
  SpecialFXBinding_Slider();
  virtual SpecialFXBindingIndex compile(SpecialFXCompiler* compiler);
private:
  dc::dcInterface<SpecialFXBinding> source_binding_;
  dc::dcInterface<SpecialFXBinding> target_binding_;
  dc::dcFloat arrival_time_;
};

}
}
}


#endif
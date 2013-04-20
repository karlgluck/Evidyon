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
#ifndef __SHARED_SPECIALFX_SPECIALFXBINDINGINSTANCE_ORBIT_H__
#define __SHARED_SPECIALFX_SPECIALFXBINDINGINSTANCE_ORBIT_H__
#pragma once


#include "specialfxbindinginstance.h"
#include "shared/binding/bindingptr.h"
#include <d3dx9math.h>

namespace Evidyon {
namespace SpecialFX {
struct SpecialFXBinding_Orbit;
}
}


namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXBindingInstance_Orbit  ]-------------------------------------
class SpecialFXBindingInstance_Orbit : public SpecialFXBindingInstance {
public:
  SpecialFXBindingInstance_Orbit(const SpecialFXBinding_Orbit* data);

public:
  virtual bool initialize(SpecialFXBindingSource* source);
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

private:
  const SpecialFXBinding_Orbit* data_;
  float current_angle_;
  Evidyon::Binding::BindingPtr input_binding_;
  D3DXMATRIXA16 transform_;
};

}
}

#endif
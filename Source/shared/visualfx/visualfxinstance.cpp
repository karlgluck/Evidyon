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
#include "visualfxinstance.h"
#include "shared/binding/binding.h"
#include <assert.h>


namespace Evidyon {
namespace VisualFX {


  
//----[  ~VisualFXInstance  ]--------------------------------------------------
VisualFXInstance::~VisualFXInstance() {
}


//----[  render  ]-------------------------------------------------------------
void VisualFXInstance::render(Texture::TextureIndex /*texture*/) {
}



//----[  getSourceBinding  ]---------------------------------------------------
bool VisualFXInstance::getSourceLocation(float* x, float* y, float* z) {
  Binding::Binding* binding = source_binding.get();
  D3DXVECTOR3 coord(0.0f, 0.0f, 0.0f);
  if (binding) {
    D3DXVec3TransformCoord(&coord, &coord, binding->getTransform());
    *x = coord.x;
    *y = coord.y;
    *z = coord.z;
    return true;
  } else {
    return false;
  }
}



//----[  getTargetBinding  ]---------------------------------------------------
bool VisualFXInstance::getTargetLocation(float* x, float* y, float* z) {
  Binding::Binding* binding = target_binding.get();
  D3DXVECTOR3 coord(0.0f, 0.0f, 0.0f);
  if (binding) {
    D3DXVec3TransformCoord(&coord, &coord, binding->getTransform());
    *x = coord.x;
    *y = coord.y;
    *z = coord.z;
    return true;
  } else {
    return false;
  }
}




//----[  sourceHidden  ]-------------------------------------------------------
bool VisualFXInstance::sourceHidden() {
  return !source_binding || !source_binding->visible();
}



//----[  targetHidden  ]-------------------------------------------------------
bool VisualFXInstance::targetHidden() {
  return !target_binding || !target_binding->visible();
}



//----[  hidden  ]-------------------------------------------------------------
bool VisualFXInstance::hidden() {
  return sourceHidden() && targetHidden();
}



//----[  sourceExpired  ]------------------------------------------------------
bool VisualFXInstance::sourceExpired() {
  return !source_binding || source_binding->hasExpired();
}



//----[  targetExpired  ]------------------------------------------------------
bool VisualFXInstance::targetExpired() {
  return !target_binding || target_binding->hasExpired();
}


//----[  expired  ]------------------------------------------------------------
bool VisualFXInstance::expired() {
  return sourceExpired() || targetExpired();
}


//----[  renderer  ]-----------------------------------------------------------
VisualFXRenderer* VisualFXInstance::renderer() {
  return renderer_;
}



//----[  intensity  ]----------------------------------------------------------
float VisualFXInstance::intensity() const {
  return intensity_;
}


//----[  sourceBinding  ]------------------------------------------------------
Binding::BindingPtr VisualFXInstance::sourceBinding() {
  return source_binding;
}


//----[  targetBinding  ]------------------------------------------------------
Binding::BindingPtr VisualFXInstance::targetBinding() {
  return target_binding;
}

}
}

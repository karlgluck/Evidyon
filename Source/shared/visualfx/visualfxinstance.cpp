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

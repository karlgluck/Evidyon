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
#include "visualfxinstance_scenery.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/scenery/sceneryrenderer.h"
#include "visualfx_scenery.h"


namespace Evidyon {
namespace VisualFX {



  
//----[  VisualFXInstance_Scenery  ]-------------------------------------------
VisualFXInstance_Scenery::VisualFXInstance_Scenery(
    const Evidyon::VisualFX::VisualFX_Scenery* parameters) {
  parameters_ = parameters;
  instance_ = NULL;
}




//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_Scenery::destroy() {
  if (instance_) {
    renderer()->sceneryRenderer()->release(instance_);
  }
}




//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_Scenery::update(double time,
                                      double time_since_last_update) {
  float x, y, z;
  if (targetExpired() || getTargetLocation(&x, &y, &z) == false) return false;
  if (!instance_) {
    getSourceLocation(&x, &y, &z);
    instance_ = renderer()->sceneryRenderer()->acquire(parameters_->scenery_index);
  }
  if (instance_) {
    if (x != last_x_ || z != last_z_) {
      float angle
        = parameters_->turn_to_follow_path ? atan2f(-(z - last_z_), x - last_x_) : 0.0f;
      D3DXMATRIXA16 rot;
      D3DXMatrixRotationY(&rot, angle);
      D3DXMATRIXA16 mat;
      D3DXMatrixTranslation(&mat, x, y, z);
      D3DXMatrixMultiply(&mat, &rot, &mat);
      instance_->setTransform(&mat);
      last_x_ = x;
      last_z_ = z;
    }
  }
  return true;
}


}
}
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
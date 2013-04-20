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
#include "sceneryinstance.h"
#include "scenery.h"
#include "shared/binding/scenerybinding.h"


namespace Evidyon {
namespace Scenery {


//----[  setTransform  ]-------------------------------------------------------
void SceneryInstance::setTransform(const D3DXMATRIX* world_matrix) {
  D3DXMatrixMultiply(&combined_transform,
                     &scenery->local_transform,
                     world_matrix);
  updateBoundVisualFX();
}




//----[  updateBoundVisualFX  ]------------------------------------------------
void SceneryInstance::updateBoundVisualFX() {
  Binding::SceneryBinding* binding = visual_fx_binding.get();
  if (binding) binding->updateSceneryTransform();
}



//----[  updateBoundVisualFX  ]------------------------------------------------
void SceneryInstance::updateBoundVisualFX(float x, float y, float z) {
  Binding::SceneryBinding* binding = visual_fx_binding.get();
  if (binding) binding->updateSceneryTransform(x, y, z);
}






}
}


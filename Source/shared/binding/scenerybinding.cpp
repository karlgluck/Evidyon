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
#include "scenerybinding.h"
#include "shared/scenery/scenery.h"
#include "shared/scenery/sceneryinstance.h"
#include <assert.h>


namespace Evidyon {
namespace Binding {



  
//----[  New  ]----------------------------------------------------------------
SceneryBindingPtr SceneryBinding::New(const Scenery::SceneryInstance* scenery) {
  SceneryBindingPtr binding(new SceneryBinding(scenery));
  return binding;
}

  
//----[  SceneryBinding  ]-----------------------------------------------------
SceneryBinding::SceneryBinding(const Scenery::SceneryInstance* scenery) {
  scenery_ = scenery;
}


//----[  update  ]-------------------------------------------------------------
void SceneryBinding::update(double time, double time_since_last_update) {
}


//----[  visible  ]------------------------------------------------------------
bool SceneryBinding::visible() {
  return scenery_ && !scenery_->hidden;
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* SceneryBinding::getTransform() const {
  return &transform_;
}


//----[  updateSceneryTransform  ]---------------------------------------------
void SceneryBinding::updateSceneryTransform() {
  assert(scenery_);
  const Evidyon::Scenery::Scenery* scenery = scenery_->scenery;
  D3DXMATRIXA16 t;
  D3DXMatrixTranslation(&t,
                        scenery->visual_fx_offset.x,
                        scenery->visual_fx_offset.y,
                        scenery->visual_fx_offset.z);
  D3DXMatrixMultiply(&transform_, &scenery_->combined_transform, &t);
}


//----[  updateSceneryTransform  ]---------------------------------------------
void SceneryBinding::updateSceneryTransform(float x, float y, float z) {
  assert(scenery_);
  const Evidyon::Scenery::Scenery* scenery = scenery_->scenery;
  D3DXMATRIXA16 t;
  D3DXMatrixTranslation(&t, x, y, z);
  D3DXMatrixMultiply(&transform_, &scenery_->combined_transform, &t);
}



//----[  releaseScenery  ]-----------------------------------------------------
void SceneryBinding::releaseScenery() {
  scenery_ = NULL;
  setExpired();
}


}
}


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


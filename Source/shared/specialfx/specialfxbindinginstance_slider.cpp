//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "specialfxbindinginstance_slider.h"
#include "specialfxbinding_slider.h"
#include "specialfxbindingsource.h"
#include "common/null.h"


namespace Evidyon {
namespace SpecialFX {


  
//----[  SpecialFXBindingInstance_Slider  ]------------------------------------
SpecialFXBindingInstance_Slider::SpecialFXBindingInstance_Slider(
    const SpecialFXBinding_Slider* data) {
  data_ = data;
  percent_ = 0.0f;
}




//----[  initialize  ]---------------------------------------------------------
bool SpecialFXBindingInstance_Slider::initialize(SpecialFXBindingSource* source) {
  source_binding_ = source->createBinding(data_->source_binding);
  target_binding_ = source->createBinding(data_->target_binding);
  return source_binding_.get() != NULL && target_binding_.get() != NULL;
}



//----[  update  ]-------------------------------------------------------------
void SpecialFXBindingInstance_Slider::update(double time,
                                             double time_since_last_update) {
  source_binding_->update(time, time_since_last_update);
  target_binding_->update(time, time_since_last_update);
  if (source_binding_->hasExpired() ||
      target_binding_->hasExpired()) setExpired();

  percent_ += data_->increment * time_since_last_update;
  percent_ = percent_ < 1.0f ? percent_ : 1.0f;

  D3DXVECTOR3 source(0.0f, 0.0f, 0.0f);
  D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);

  D3DXVec3TransformCoord(&source, &source, source_binding_->getTransform());
  D3DXVec3TransformCoord(&target, &target, target_binding_->getTransform());

  D3DXVec3Lerp(&source, &source, &target, percent_);
  D3DXVec3Subtract(&target, &target, &source);
  
  D3DXMATRIX m, r;
  D3DXMatrixTranslation(&m, source.x, source.y, source.z);
  D3DXMatrixRotationY(&r, atan2f(target.z, target.x));

  D3DXMatrixMultiply(&transform_, &r, &m);
}



//----[  visible  ]------------------------------------------------------------
bool SpecialFXBindingInstance_Slider::visible() {
  return source_binding_->visible() || target_binding_->visible();
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* SpecialFXBindingInstance_Slider::getTransform() const {
  return &transform_;
}


}
}


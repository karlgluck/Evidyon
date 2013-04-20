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
#include "specialfxbindinginstance_oscillation.h"
#include "specialfxbinding_oscillation.h"
#include "specialfxbindingsource.h"
#include "common/null.h"
#include <assert.h>


namespace Evidyon {
namespace SpecialFX {


  
//----[  SpecialFXBindingInstance_Oscillation  ]-------------------------------
SpecialFXBindingInstance_Oscillation::SpecialFXBindingInstance_Oscillation(
    const SpecialFXBinding_Oscillation* data) {
  data_ = data;
  horizontal_angle_ = 0.0f;
  vertical_angle_ = 0.0f;
}



//----[  initialize  ]---------------------------------------------------------
bool SpecialFXBindingInstance_Oscillation::initialize(SpecialFXBindingSource* source) {
  input_binding_ = source->createBinding(data_->input_binding);
  return input_binding_.get() != NULL;
}



//----[  update  ]-------------------------------------------------------------
void SpecialFXBindingInstance_Oscillation::update(double time,
                                                  double time_since_last_update) {
  input_binding_->update(time, time_since_last_update);
  if (input_binding_->hasExpired()) setExpired();
  horizontal_angle_ += time_since_last_update * data_->speed_horizontal;
  vertical_angle_ += time_since_last_update * data_->speed_vertical;
  D3DXMATRIXA16 m;
  D3DXMatrixTranslation(
    &m,
    cosf(horizontal_angle_) * data_->amplitude_horizontal + data_->phase_offset_horizontal,
    sinf(vertical_angle_) * data_->amplitude_vertical + data_->phase_offset_vertical,
    0.0f);
  D3DXMatrixMultiply(&transform_, input_binding_->getTransform(), &m);
}



//----[  visible  ]------------------------------------------------------------
bool SpecialFXBindingInstance_Oscillation::visible() {
  return input_binding_->visible();
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* SpecialFXBindingInstance_Oscillation::getTransform() const {
  return &transform_;
}


}
}


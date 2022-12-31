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
#include "specialfxbindinginstance_orbit.h"
#include "specialfxbinding_orbit.h"
#include "specialfxbindingsource.h"
#include "common/null.h"


namespace Evidyon {
namespace SpecialFX {


  
//----[  SpecialFXBindingInstance_Orbit  ]-------------------------------------
SpecialFXBindingInstance_Orbit::SpecialFXBindingInstance_Orbit(
    const SpecialFXBinding_Orbit* data) {
  data_ = data;
  current_angle_ = 0.0f;
}



//----[  initialize  ]---------------------------------------------------------
bool SpecialFXBindingInstance_Orbit::initialize(SpecialFXBindingSource* source) {
  input_binding_ = source->createBinding(data_->input_binding);
  return input_binding_.get() != NULL;
}



//----[  update  ]-------------------------------------------------------------
void SpecialFXBindingInstance_Orbit::update(double time,
                                            double time_since_last_update) {
  input_binding_->update(time, time_since_last_update);
  if (input_binding_->hasExpired()) setExpired();

  current_angle_ += time_since_last_update * data_->speed;

  D3DXMATRIXA16 m;
  D3DXMatrixTranslation(&m,
                        cosf(current_angle_)*data_->radius,
                        0.0f,
                        sinf(current_angle_)*data_->radius);
  D3DXMatrixMultiply(&transform_, &m, input_binding_->getTransform());
}



//----[  visible  ]------------------------------------------------------------
bool SpecialFXBindingInstance_Orbit::visible() {
  return input_binding_->visible();
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* SpecialFXBindingInstance_Orbit::getTransform() const {
  return &transform_;
}

}
}


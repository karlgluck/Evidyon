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
#include "specialfxbindinginstance_offset.h"
#include "specialfxbinding_offset.h"
#include "specialfxbindingsource.h"
#include "common/null.h"


namespace Evidyon {
namespace SpecialFX {


  
//----[  SpecialFXBindingInstance_Offset  ]------------------------------------
SpecialFXBindingInstance_Offset::SpecialFXBindingInstance_Offset(
  const SpecialFXBinding_Offset* data) {
  data_ = data;
}




//----[  initialize  ]---------------------------------------------------------
bool SpecialFXBindingInstance_Offset::initialize(SpecialFXBindingSource* source) {
  input_binding_ = source->createBinding(data_->input_binding);
  return input_binding_.get() != NULL;
}



//----[  update  ]-------------------------------------------------------------
void SpecialFXBindingInstance_Offset::update(double time,
                                             double time_since_last_update) {
  input_binding_->update(time, time_since_last_update);
  if (input_binding_->hasExpired()) setExpired();

  D3DXMatrixMultiply(&transform_,
                     (const D3DXMATRIX*)&data_->offset,
                     input_binding_->getTransform());
}



//----[  visible  ]------------------------------------------------------------
bool SpecialFXBindingInstance_Offset::visible() {
  return input_binding_->visible();
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* SpecialFXBindingInstance_Offset::getTransform() const {
  return &transform_;
}

}
}


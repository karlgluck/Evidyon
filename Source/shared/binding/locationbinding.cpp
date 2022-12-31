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
#include "locationbinding.h"


namespace Evidyon {
namespace Binding {


//----[  New  ]----------------------------------------------------------------
LocationBindingPtr LocationBinding::New(float x, float y, float z) {
  LocationBindingPtr binding(new LocationBinding());
  binding->set(x, y, z, 0.0);
  return binding;
}



//----[  LocationBinding  ]----------------------------------------------------
LocationBinding::LocationBinding() {
}



//----[  set  ]----------------------------------------------------------------
void LocationBinding::set(float x, float y, float z, float y_angle) {
  D3DXMATRIXA16 r;
  D3DXMatrixRotationY(&r, 0.0f);
  D3DXMatrixTranslation(&transform_, x, y, z);
  D3DXMatrixMultiply(&transform_, &r, &transform_);
}


//----[  update  ]-------------------------------------------------------------
void LocationBinding::update(double time, double time_since_last_update) {
}


//----[  visible  ]------------------------------------------------------------
bool LocationBinding::visible() {
  return true; // TODO: link to map manager
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* LocationBinding::getTransform() const {
  return &transform_;
}



}
}


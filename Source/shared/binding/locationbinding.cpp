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


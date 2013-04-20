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
#include "delaybinding.h"


namespace Evidyon {
namespace Binding {


//----[  New  ]----------------------------------------------------------------
DelayBindingPtr DelayBinding::New(BindingPtr source, double expiration_time) {
  DelayBindingPtr binding(new DelayBinding(source, expiration_time));
  return binding;
}

  
//----[  DelayBinding  ]-----------------------------------------------------
DelayBinding::DelayBinding(BindingPtr source, double expiration_time) {
  source_ = source;
  expiration_time_ = expiration_time;
}


//----[  update  ]-------------------------------------------------------------
void DelayBinding::update(double time, double time_since_last_update) {
  if (time >= expiration_time_ || source_->hasExpired()) setExpired();
  source_->update(time, time_since_last_update);
}


//----[  visible  ]------------------------------------------------------------
bool DelayBinding::visible() {
  return source_->visible();
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* DelayBinding::getTransform() const {
  return source_->getTransform();
}



}
}


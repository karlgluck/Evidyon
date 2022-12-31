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


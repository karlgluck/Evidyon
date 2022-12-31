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
#ifndef __SHARED_BINDING_DELAYBINDING_H__
#define __SHARED_BINDING_DELAYBINDING_H__
#pragma once


#include "binding.h"
#include "bindingptr.h"
#include "delaybindingptr.h"


namespace Evidyon {
namespace Delay {
struct RenderableActorInstance;
}
}

namespace Evidyon {
namespace Binding {

  
//----[  DelayBinding  ]----------------------------------------------------
// This binding just expires after a certain amount of time.  All other
// information is passed directly through to the source, including
// update commands.
class DelayBinding : public Evidyon::Binding::Binding {
public:
  static DelayBindingPtr New(BindingPtr source, double expiration_time);

public:
  // update() will call the source's update method automatically
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

private:
  BindingPtr source_;
  double expiration_time_;

private:
  DelayBinding(BindingPtr source, double expiration_time);
  DelayBinding(const DelayBinding&);
  void operator =(DelayBinding&);
};



}
}

#endif
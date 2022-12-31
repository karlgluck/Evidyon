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
#ifndef __SHARED_BINDING_DELAYLOCATIONBINDING_H__
#define __SHARED_BINDING_DELAYLOCATIONBINDING_H__
#pragma once


#include "binding.h"
#include "delaylocationbindingptr.h"


namespace Evidyon {
namespace DelayLocation {
struct RenderableActorInstance;
}
}

namespace Evidyon {
namespace Binding {

  
  
//----[  DelayLocationBinding  ]-----------------------------------------------
// This class is a combination of the 'delay' and 'location' bindings that
// is a bit faster and takes less memory.
class DelayLocationBinding : public Evidyon::Binding::Binding {
public:
  static DelayLocationBindingPtr New(double expiration_time,
                                     float x, float y, float z);

public:
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

private:
  double expiration_time_;
  D3DXMATRIXA16 transform_;

private:
  DelayLocationBinding(double expiration_time, float x, float y, float z);
  DelayLocationBinding(const DelayLocationBinding&);
  void operator =(DelayLocationBinding&);
};



}
}

#endif
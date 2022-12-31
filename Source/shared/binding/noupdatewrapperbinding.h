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
#ifndef __SHARED_BINDING_NOUPDATEWRAPPERBINDING_H__
#define __SHARED_BINDING_NOUPDATEWRAPPERBINDING_H__
#pragma once

#include "shared/binding/binding.h"
#include "shared/binding/bindingptr.h"

namespace Evidyon {
namespace Binding {



//----[  NoUpdateWrapperBinding  ]---------------------------------------------
class NoUpdateWrapperBinding : public Evidyon::Binding::Binding {
public:
  // This New() is special because it doesn't return a specialized wrapper
  // binding type
  static BindingPtr New(Evidyon::Binding::BindingPtr source);

public:
  // update() does not pass through to the source, but all other
  // methods do.
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

private:
  BindingPtr source_;

private:
  NoUpdateWrapperBinding(Evidyon::Binding::BindingPtr source);
  NoUpdateWrapperBinding(const NoUpdateWrapperBinding&);
  void operator =(NoUpdateWrapperBinding&);
};

}
}

#endif
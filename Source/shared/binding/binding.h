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
#ifndef __SHARED_BINDING_BINDING_H__
#define __SHARED_BINDING_BINDING_H__
#pragma once


#include <d3dx9math.h> // for D3DXMATRIX

namespace Evidyon {
namespace Binding {


//----[  Binding  ]------------------------------------------------------------
// All bindings should be managed using BindingPtr, which implements reference
// counting.
class Binding {
public:
  Binding();
  virtual ~Binding() = 0;

  // Advances any dynamic components of this binding
  virtual void update(double time, double time_since_last_update) = 0;

  // Returns whether or not this binding is currently visible to the client's
  // avatar.  The effects bound to an invisible binding should continue to
  // update normally, but not render.
  virtual bool visible() = 0;

  // Obtains the transformation matrix of this binding.  This remains valid
  // even if a binding has expired.
  virtual const D3DXMATRIX* getTransform() const = 0;


public:

  // Returns whether or not this binding has expired.  Various things cause a
  // binding to expire depending on its type; actor bindings expire when the
  // actor logs off, some bindings expire after a certain amount of time.
  bool hasExpired() const;

  // Enables the expired flag
  void setExpired();


private:

  // Whether or not this binding has expired.  This value defaults to false and
  // can only be set, never reset.
  bool expired_;
};



}
}

#endif
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
#ifndef __SHARED_BINDING_SCENERYBINDING_H__
#define __SHARED_BINDING_SCENERYBINDING_H__
#pragma once


#include "binding.h"
#include "scenerybindingptr.h"

namespace Evidyon {
namespace Scenery {
struct SceneryInstance;
}
}


namespace Evidyon {
namespace Binding {


//----[  SceneryBinding  ]-----------------------------------------------------
// This binding type is created so that a scenery object can attach a visual
// effect to it.  This is a push-style binding; the update method is empty.
class SceneryBinding : public Evidyon::Binding::Binding {
public:
  static SceneryBindingPtr New(const Scenery::SceneryInstance* scenery);

public:
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;


public:
  void updateSceneryTransform();

  // updates the scenery transform, providing override coordinates for the
  // attachment point while the scenery is being edited
  void updateSceneryTransform(float x, float y, float z);
  void releaseScenery();

private:
  const Scenery::SceneryInstance* scenery_;
  D3DXMATRIXA16 transform_;

private:
  SceneryBinding(const Scenery::SceneryInstance* scenery);
  SceneryBinding(const SceneryBinding&);
  void operator =(SceneryBinding&);
};




//----[  SharedSceneryBindingPtr  ]--------------------------------------------
typedef std::tr1::shared_ptr<SceneryBinding> SharedSceneryBindingPtr;



}
}

#endif
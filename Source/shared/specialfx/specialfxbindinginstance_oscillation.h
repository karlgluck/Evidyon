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
#ifndef __SHARED_SPECIALFX_SPECIALFXBINDINGINSTANCE_OSCILLATION_H__
#define __SHARED_SPECIALFX_SPECIALFXBINDINGINSTANCE_OSCILLATION_H__
#pragma once


#include "specialfxbindinginstance.h"
#include "shared/binding/bindingptr.h"
#include <d3dx9math.h>

namespace Evidyon {
namespace SpecialFX {
struct SpecialFXBinding_Oscillation;
}
}


namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXBindingInstance_Oscillation  ]-------------------------------
class SpecialFXBindingInstance_Oscillation : public SpecialFXBindingInstance {
public:
  SpecialFXBindingInstance_Oscillation(const SpecialFXBinding_Oscillation* data);

public:
  virtual bool initialize(SpecialFXBindingSource* source);
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

private:
  const SpecialFXBinding_Oscillation* data_;
  float horizontal_angle_, vertical_angle_;
  Evidyon::Binding::BindingPtr input_binding_;
  D3DXMATRIXA16 transform_;
};

}
}

#endif
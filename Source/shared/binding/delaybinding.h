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
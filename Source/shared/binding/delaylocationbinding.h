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
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
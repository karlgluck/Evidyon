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
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
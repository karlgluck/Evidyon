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
#ifndef __SHARED_BINDING_ACTORBINDING_H__
#define __SHARED_BINDING_ACTORBINDING_H__
#pragma once



#include "binding.h"
#include "actorbindingptr.h"

namespace Evidyon {
namespace Actor {
struct RenderableActorInstance;
}
}

namespace Evidyon {
namespace Binding {


//----[  ActorBinding  ]-------------------------------------------------------
// Attaches a binding to the actor's feet such that the axes move and rotate
// with the actor.  This is a push-style binding; one exists for each actor.
class ActorBinding : public Evidyon::Binding::Binding {
public:
  static ActorBindingPtr New(const Actor::RenderableActorInstance* actor);

public:
  virtual void update(double time, double time_since_last_update);
  virtual bool visible();
  virtual const D3DXMATRIX* getTransform() const;

public:
  void updateActorTransform(const D3DXMATRIX* root_matrix);
  void releaseActor();
  void rebind(const Actor::RenderableActorInstance* actor); 

private:
  const Actor::RenderableActorInstance* actor_;
  D3DXMATRIXA16 transform_;

private:
  ActorBinding(const Actor::RenderableActorInstance* actor);
  ActorBinding(const ActorBinding&);
  void operator =(ActorBinding&);
};



}
}

#endif
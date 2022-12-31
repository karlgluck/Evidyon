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
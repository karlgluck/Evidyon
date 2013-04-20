//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "actorbinding.h"
#include "shared/actor/renderableactorinstance.h"
#include "shared/skinned-mesh/skinnedmeshinstance.h"
#include <assert.h>


namespace Evidyon {
namespace Binding {



  
//----[  New  ]----------------------------------------------------------------
ActorBindingPtr ActorBinding::New(const Actor::RenderableActorInstance* actor) {
  ActorBindingPtr binding(new ActorBinding(actor));
  return binding;
}


//----[  ActorBinding  ]-------------------------------------------------------
ActorBinding::ActorBinding(const Actor::RenderableActorInstance* actor) {
  actor_ = actor;
}


//----[  update  ]-------------------------------------------------------------
void ActorBinding::update(double time, double time_since_last_update) {
  if (!actor_ || !actor_->skinned_mesh_instance) setExpired();
}


//----[  visible  ]------------------------------------------------------------
bool ActorBinding::visible() {
  assert(hasExpired() == !actor_);
  return actor_ && actor_->skinned_mesh_instance && !actor_->skinned_mesh_instance->hidden;
}



//----[  getTransform  ]-------------------------------------------------------
const D3DXMATRIX* ActorBinding::getTransform() const {
  return &transform_;
}



//----[  updateActorTransform  ]-----------------------------------------------
void ActorBinding::updateActorTransform(const D3DXMATRIX* root_matrix) {
  assert(actor_);
  if (actor_->base_animation_state == Actor::RenderableActorInstance::SWIM ||
      actor_->base_animation_state == Actor::RenderableActorInstance::SWIM_IDLE) {
    D3DXMATRIXA16 m;
    D3DXMatrixTranslation(&m, 0.0f, -0.5f, 0.0f);
    D3DXMatrixMultiply(&transform_, root_matrix, &m);
  } else {
    memcpy(&transform_, root_matrix, sizeof(D3DXMATRIX));
  }
}



//----[  releaseActor  ]-------------------------------------------------------
void ActorBinding::releaseActor() {
  actor_ = NULL;
  setExpired();
}



//----[  rebind  ]-------------------------------------------------------------
void ActorBinding::rebind(const Actor::RenderableActorInstance* actor) {
  actor_ = actor;
}

}
}


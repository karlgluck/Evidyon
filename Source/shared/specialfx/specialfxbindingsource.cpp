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
#include "specialfxbindingsource.h"
#include "shared/binding/delaybinding.h"
#include "shared/binding/locationbinding.h"
#include "shared/binding/delaylocationbinding.h"
#include "shared/binding/noupdatewrapperbinding.h"
#include "shared/specialfx/specialfxbinding.h"
#include <assert.h>


namespace Evidyon {
namespace SpecialFX {


  
//----[  SpecialFXBindingSource  ]---------------------------------------------
SpecialFXBindingSource::SpecialFXBindingSource(
    const SpecialFXBinding* bindings,
    size_t number_of_bindings) {
  bindings_ = bindings;
  number_of_bindings_ = number_of_bindings;
}


//----[  set  ]----------------------------------------------------------------
void SpecialFXBindingSource::set(double expiration_time,
                                 float source_x,
                                 float source_z,
                                 Binding::BindingPtr source_object,
                                 float target_x,
                                 float target_z,
                                 Binding::BindingPtrSet* bindings_used) {

  expiration_time_ = expiration_time;
  source_x_ = source_x;
  source_z_ = source_z;
  target_x_ = target_x;
  target_z_ = target_z;
  source_object_ = source_object;
  target_object_.reset();

  source_location_.reset();
  target_location_.reset();
  delayed_source_object_.reset();
  delayed_target_object_.reset();

  bindings_used_ = bindings_used;
}


//----[  setTargetObject  ]----------------------------------------------------
void SpecialFXBindingSource::setTargetObject(Binding::BindingPtr target_object) {
  target_object_ = target_object;
  delayed_target_object_.reset();
}



//----[  createBinding  ]------------------------------------------------------
Binding::BindingPtr SpecialFXBindingSource::createBinding(
    SpecialFXBindingIndex binding) {
  assert(binding != INVALID_SPECIALFX_BINDING_INDEX);
  if (binding < number_of_bindings_) {
    return SpecialFXBindingInstance::New(&bindings_[binding], this);
  } else {
    switch (binding) {
      default: assert(false);
      case SPECIALFX_BINDING_SOURCE_LOCATION:
        if (source_location_ == NULL) {
          if (expiration_time_ >= 0.0) {
            source_location_ = Binding::DelayLocationBinding::New(
              expiration_time_,
              source_x_,
              0.0f,
              source_z_);
          } else {
            source_location_ = Binding::LocationBinding::New(
              source_x_,
              0.0f,
              source_z_);
          }
        }
        if (bindings_used_) bindings_used_->insert(source_location_);
        return source_location_;

      case SPECIALFX_BINDING_SOURCE_OBJECT:
        if (delayed_source_object_ == NULL) {
          if (source_object_ == NULL) {
            delayed_source_object_
              = createBinding(SPECIALFX_BINDING_SOURCE_LOCATION);
          } else {
            if (expiration_time_ >= 0.0) {
              delayed_source_object_ = Binding::DelayBinding::New(
                source_object_,
                expiration_time_);
            } else {
              delayed_source_object_
                = Binding::NoUpdateWrapperBinding::New(source_object_);
            }
          }
        }
        if (bindings_used_) bindings_used_->insert(delayed_source_object_);
        return delayed_source_object_;

      case SPECIALFX_BINDING_TARGET_LOCATION:
        if (target_location_ == NULL) {
          if (expiration_time_ >= 0.0) {
            target_location_ = Binding::DelayLocationBinding::New(
              expiration_time_,
              target_x_,
              0.0f,
              target_z_);
          } else {
            target_location_ = Binding::LocationBinding::New(
              target_x_,
              0.0f,
              target_z_);
          }
        }
        if (bindings_used_) bindings_used_->insert(target_location_);
        return target_location_;

      case SPECIALFX_BINDING_TARGET_OBJECT:
        if (delayed_target_object_ == NULL) {
          if (target_object_ == NULL) {
            delayed_target_object_
              = createBinding(SPECIALFX_BINDING_TARGET_LOCATION);
          } else {
            if (expiration_time_ >= 0.0) {
              delayed_target_object_ = Binding::DelayBinding::New(
                target_object_,
                expiration_time_);
            } else {
              delayed_target_object_
                = Binding::NoUpdateWrapperBinding::New(target_object_);
            }
          }
        }
        if (bindings_used_) bindings_used_->insert(delayed_target_object_);
        return delayed_target_object_;

    }
  }

  assert(false); // one of the other paths should have returned
}



//----[  getSourceX  ]---------------------------------------------------------
float SpecialFXBindingSource::getSourceX() const {
  return source_x_;
}



//----[  getSourceZ  ]---------------------------------------------------------
float SpecialFXBindingSource::getSourceZ() const {
  return source_z_;
}


}
}


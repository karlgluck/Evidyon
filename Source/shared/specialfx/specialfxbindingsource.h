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
#ifndef __SHARED_SPECIALFX_SPECIALFXBINDINGSOURCE_H__
#define __SHARED_SPECIALFX_SPECIALFXBINDINGSOURCE_H__
#pragma once


#include "specialfxbindinginstance.h"
#include "shared/binding/bindingptr.h"
#include "shared/binding/bindingptrset.h"
#include "specialfxbindingindex.h"

namespace Evidyon {
namespace SpecialFX {

//----[  SpecialFXBindingSource  ]---------------------------------------------
class SpecialFXBindingSource {
public:
  SpecialFXBindingSource(
    const SpecialFXBinding* bindings,
    size_t number_of_bindings);

  // Fills the creator with data
  void set(double expiration_time,
           float source_x,
           float source_z,
           Binding::BindingPtr source_object,
           float target_x,
           float target_z,
           Binding::BindingPtrSet* bindings_used);

  // Called multiple times while the other parameters stay the same when the
  // manager is generating an effect for each target parameter
  void setTargetObject(Binding::BindingPtr target_object);

  // After being invoked by the special FX manager, this method creates the
  // full chain of special FX bindings.
  Binding::BindingPtr createBinding(SpecialFXBindingIndex binding);

  // Gets the location
  float getSourceX() const;
  float getSourceZ() const;

private:
  const SpecialFXBinding* bindings_;
  size_t number_of_bindings_;
  double expiration_time_;  // < 0 = ignore
  float source_x_, source_z_;
  float target_x_, target_z_;

  // The original source/target parameters
  Binding::BindingPtr source_object_;
  Binding::BindingPtr target_object_;

  // Once created, bindings are placed here.  Note that the source object and
  // target object bindings are only used if there is no delay--otherwise,
  // a delay object wraps them so that derived objects expire properly
  // without affecting higher objects.
  Binding::BindingPtr source_location_;
  Binding::BindingPtr target_location_;
  Binding::BindingPtr delayed_source_object_;
  Binding::BindingPtr delayed_target_object_;

  // If this member is valid, all bindings that are returned by createBinding
  // are added to the set.
  Binding::BindingPtrSet* bindings_used_;
};

}
}

#endif
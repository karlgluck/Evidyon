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
#ifndef __VISUALFXINSTANCE_H__
#define __VISUALFXINSTANCE_H__
#pragma once




#include "shared/texture/textureindex.h"
#include "shared/binding/bindingptr.h"


namespace Evidyon {
namespace VisualFX {
class VisualFXRenderer;

//----[  VisualFXInstance  ]---------------------------------------------------
class VisualFXInstance {
  friend class VisualFXRenderer;

public:
  virtual ~VisualFXInstance() = 0;

  // Cleans up the FX instance's allocated data.  This method can be called at
  // any time--don't assume update() has to return false first.
  virtual void destroy() = 0;

  // Advances the state of this FX instance.  This method should return 'true'
  // if the update proceeded normally.
  // When this method returns 'false', the visual FX instance is destroyed
  // and removed from the update list.
  virtual bool update(double time,
                      double time_since_last_update) = 0;

  // Draws this FX instance's data into the renderer.  For FX that only
  // renders geometry via the scenery renderer, this method is never called.
  virtual void render(Texture::TextureIndex texture);


protected:

  // Obtains the coordinates from the given binding.  If the binding is NULL
  // or the binding's return value is 'false', the method returns false.
  // Since a NULL binding can't return a position, if either method finds it
  // has a NULL object, it returns the other binding's coordinates.
  // If both bindings are NULL, the output coordinates are not modified.
  bool getSourceLocation(float* x, float* y, float* z);
  bool getTargetLocation(float* x, float* y, float* z);

  // Returns whether or not various combinations of the source/target
  // bindings are visible.  If a binding is invalid, it is considered to be
  // "hidden".
  bool sourceHidden();
  bool targetHidden();
  bool hidden();
  bool sourceExpired();
  bool targetExpired();
  bool expired();

  // Gets the FX rendering object associated this instance should use
  VisualFXRenderer* renderer();

  // Reads this FX instance's configured intensity
  float intensity() const;

  // Obtains the source and target binding structures.  Intended for use with
  // compound effects; it is preferable to use the above methods to
  // interact with them.
  Binding::BindingPtr sourceBinding();
  Binding::BindingPtr targetBinding();
  Binding::BindingPtr sourceOrValidBinding();
  Binding::BindingPtr targetOrValidBinding();

private:

  // Main rendering object.  Guaranteed to be valid before update() is called
  // and to stay valid for the life of the object--until destroy()
  VisualFXRenderer* renderer_;

  // Intensity factor for the effect specified when it is created.  This value
  // ranges from 0 (least intense) to 1 (most intense).  This is provided for
  // use by spell effects and other configurable effects and is safe to ignore
  // if it's not needed.
  float intensity_;

  // The objects that provide positioning references for this FX object
  // These INTENTIONALLY do not have _ at the end (to denote private
  // membership) because the visual fx renderer manipulates these variables.
  Binding::BindingPtr source_binding;
  Binding::BindingPtr target_binding;
};

}
}


#endif
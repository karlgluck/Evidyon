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
#ifndef __SHARED_SCENERY_SCENERYINSTANCE_H__
#define __SHARED_SCENERY_SCENERYINSTANCE_H__
#pragma once


#include <d3dx9.h> // D3DXMATRIX
#include "shared/binding/scenerybindingptr.h"

namespace Evidyon {
namespace Scenery {
struct Scenery;
}
}

namespace Evidyon {
namespace Scenery {



//----[  SceneryInstance  ]----------------------------------------------------
struct SceneryInstance {

  // Source data for this instance to draw
  const Scenery* scenery;

  // Set this flag to prevent ths instance from being rendered.  This will
  // also hide the special FX bound to this instance, if any.
  bool hidden;

  // Transform used to render the scenery.  This is the combination of the
  // word and local matrices, so it's updated every time the scenery moves.
  D3DXMATRIXA16 combined_transform;

  // Initializes combined_transform based on the provided word matrix and the
  // scenery-specified local transform.
  void setTransform(const D3DXMATRIX* world_matrix);

  // Updates the binding location of the visual fx, if it is attached.  This
  // method should be called whenever combined_transform changes.  It is
  // invoked automatically by setTransform, but should remain separate because
  // the scenery editor modifies the combined transform without
  // using the setTransform method.
  // The second method overrides the source scenery object's attachment point
  void updateBoundVisualFX();
  void updateBoundVisualFX(float x, float y, float z);

  // Visual effect attached to the scenery.  When the scenery is destroyed,
  // this effect is unbound automatically.
  Binding::SceneryBindingPtr visual_fx_binding;
};


}
}

#endif
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
#ifndef __MAPLAYER_H__
#define __MAPLAYER_H__
#pragma once



#include <dc/integer>
#include "win32-rect.h"

#ifndef D3DCOLOR_DEFINED
typedef unsigned long D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

namespace Evidyon {
namespace World {
struct LocationVisualData;

namespace Tools {
struct MapCompilationContext;
class MapMask;


//----[  MapLayer  ]-----------------------------------------------------------
class MapLayer : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "MapLayer"; }

public:
  MapLayer();

  // Writes the data for this layer into the compilation context.  To improve
  // the speed of updates when editing the map, pass an 'area' that is smaller
  // than the map.  Layers are *NOT* strictly limited to updating this region,
  // but most will take the hint and not bother processing anything beyond it.
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context) = 0;

  // Writes some sample data for this layer into the tile if the referenced
  // mask/color is the same as the one used by this layer.  Not all layers
  // need implement this method (for example, blending layers).  This method is
  // called for each of the tile locations that the brush is being used to
  // modify before the data is actually written.
  // The default implementation does nothing.
  virtual void brushPreview(MapMask* mask,
                            D3DCOLOR color,
                            LocationVisualData* visual_data);

  // Sorts map layers by their priority
  int compareTo(const dc::dcGenericResource* other) const;

  // Changes the priority of this layer
  void setPriority(int priority);

private:

  // Used for sequencing the layers
  dc::dcInteger priority_;
};

}
}
}

#endif
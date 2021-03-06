//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#ifndef __TERRAINBLENDINGMAPLAYER_H__
#define __TERRAINBLENDINGMAPLAYER_H__
#pragma once



#include "maplayer.h"
#include "mapmaskcolorreference.h"
#include "maplayerblendingtype.h"
#include <dc/boolean>
#include <dc/list>
#include <dc/table>
#include "shared/texture/tools/texture.h"

namespace Evidyon {
namespace World {
namespace Tools {

//----[  TerrainBlendingMapLayer  ]--------------------------------------------
// Fills seams at the interface between colors in other map layers.  This is
// used to make nice boundaries and blending effects using defined tile types.
// From the raw texture types, this map layer will automatically rotate tiles
// to create variations if the flag specifying this behavior is set.  At the
// very least, it's only necessary to create three types to achieve a full set
// of blendings: north, northeast-outside and northeast-inside.
class TerrainBlendingMapLayer :
    public dc::dcResource<TerrainBlendingMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "TerrainBlendingMapLayer"; }
  static dc::dcClass<TerrainBlendingMapLayer>::Implements<MapLayer> implements_map_layer_;
  typedef dc::dcList<Evidyon::Tools::TextureReference> TerrainTextureReferenceList;
  typedef dc::dcTable<MapLayer>::Reference MapLayerReference;

public:
  TerrainBlendingMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);

private:
  MapLayerReference inner_, outer_;
  
  // If this flag is set, the layer will automatically create rotations of
  // the defined blending types to create more unique types of blends.
  dc::dcBoolean create_variations_by_rotation_;

  // All types blending from inner to outer.
  TerrainTextureReferenceList blends_[NUMBER_OF_MAP_LAYER_BLENDING_TYPES];
};

}
}
}

#endif
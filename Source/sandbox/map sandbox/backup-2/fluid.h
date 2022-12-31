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
#ifndef __FLUID_H__
#define __FLUID_H__

#include "terrain.h"

namespace Evidyon {
namespace Map {

class MapManager;

class Fluid : public TerrainQuadBuffer {
public:
  Fluid();
  void update(MapManager* map,
              int center_x,
              int center_y,
              const TileVisibility* visibility,
              bool visible_areas_valid[2],
              const VisibleTilesRect::Area new_visible_areas[2]);

private:

  // Updates the vertices of the given tiles in the buffer.  This
  // does not write the diffuse color.
  void writeVertices(const VisibleTilesRect::Area* area,
                     TerrainVertex* vertices);

  // Pulls the textures in the given area into the cache.  This method
  // returns 'true' if either a texture has changed or there are
  // textured fluid quads visible.  If the first parameter is 'true',
  // the return value is always true.
  bool updateCachedTextures(bool already_found_fluid,
                            const MapManager* map, 
                            const VisibleTilesRect::Area* area);

private:

  // Allows the implementing class to do a one-time write of the entire vertex
  // buffer to set up its persistent state.
  virtual void initializeVertexBuffer(TerrainVertex* vertices) const;
};

}
}




#endif
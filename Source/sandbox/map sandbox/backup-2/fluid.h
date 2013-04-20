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
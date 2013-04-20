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
#ifndef __MAP_H__
#define __MAP_H__

#include "../shared/evidyontextureindextype.h"
#include "land.h"
#include "fluid.h"

namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Map {


class MapManager {
public:
  MapManager();
  bool create(LPDIRECT3DDEVICE9 d3d_device);
  void destroy();

  // Finds the height of the terrain at a coordinate
  float terrainHeight(int x, int y) const;

  // Obtains the texture index to apply to the terrain tile at a given location
  TextureIndex terrainTexture(int x, int y) const;

  // Which texture to draw for a fluid at this point.  If no fluid should be
  // drawn here, this method should return INVALID_TEXTURE_INDEX
  TextureIndex fluidTexture(int x, int y) const;

  // Synchronizes the map for a viewer at the given coordinates
  void update(int viewer_x, int viewer_y);

  // Draw the map to the screen
  bool render(LPDIRECT3DDEVICE9 d3d_device,
              TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);


private:

  // Fills in the visibility_ member for a viwer in the given location.
  void constructVisibility(int viewer_x, int viewer_y);

private:

  // Tiles that are currently being rendered
  VisibleTilesRect visible_tiles_;

  Land land_;
  Fluid fluid_;

  TileVisibility visibility_[VISIBLE_TILES];
};


}
}



#endif
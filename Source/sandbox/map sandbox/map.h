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
#include "evidyonmaprenderer.h"


namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Map {


class MapManager : public EvidyonMapData {
public:
  MapManager();
  bool create(LPDIRECT3DDEVICE9 d3d_device,
              Scenery::EvidyonSceneryRenderer* scenery_renderer);
  void destroy();

  virtual void updateMapTileData(int x, int y, Evidyon::World::LocationVisualData* data);

  // Synchronizes the map for a viewer at the given coordinates
  void update(int viewer_x, int viewer_y);

  // Draw the map to the screen
  bool render(LPDIRECT3DDEVICE9 d3d_device,
              TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);


private:
  EvidyonMapRenderer map_;
};


}
}



#endif
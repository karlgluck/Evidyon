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
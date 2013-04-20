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
#ifndef __MAP_H__
#define __MAP_H__
#pragma once



#include "mapmask.h"
#include "maplayer.h"
#include "shared/geosid/tools/geosid.h"
#include <dc/list>
#include <dc/table>
#include "win32-rect.h"
#include "shared/trigger/tools/trigger.h"
#include "shared/trigger/triggerindex.h"
#include "shared/zone/tools/zone.h"
#include "lifeformaiinhabitantspawn.h"
#include "shared/magic/magicindex.h"
#include "shared/geosid/geonite.h"
#include "geoniteburst.h"
#include "maptrigger.h"

namespace Evidyon {
namespace Geosid {
namespace Tools {
class GeosidCompiler;
}
}
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
}

namespace Evidyon {
namespace World {
namespace Tools {

struct MapCompilationContext;

//----[  Map  ]----------------------------------------------------------------
// Holds all the information about a plane of the world:
//  - monsters
//  - npcs
//  - scenery
//  - terrain
//  - fluids (water/lava/etc)
//  - navigability type
//  - named zones
//  - triggers
class Map : public dc::dcResource<Map> {
public:
  static std::string staticTypeName() { return "Map"; }
public:
  Map();

  // Generates a context used to compile the map layers.  This method does not
  // fill the context with any useful data from the layers; call
  // buildContextContents to do that.
  bool createCompilationContext(LPDIRECT3DDEVICE9 d3d_device,
                                MapCompilationContext* context);

  // Frees data in the compilation context
  void destroyCompilationContext(MapCompilationContext* context);

  // Orders the map layers by their priority.  This is called when the
  // compilation context is created and whenever else it is deemed necessary.
  // It is not automatically invoked by buildContextContents, since that
  // method would be made even more expensive if it were and they should
  // have been sorted before that method is ever called.
  void sortMapLayers();

  // Updates the contents of the context for the given area.  If the area is
  // NULL, an area the size of the entire map will be substituted.
  // There is very little fixed cost associated with calling this method, so
  // tuning the area to be as small as feasible can greatly improve speed.
  void buildContextContents(const RECT* area,
                            MapCompilationContext* context);

  // Writes data for the location as if the given mask/color pair was active.
  void buildBrushPreview(MapMask* mask,
                         D3DCOLOR color,
                         LocationVisualData* visual_data);

  // Returns the array of elements in masks_
  const dc::dcGenericResource::Array& getMapMasks();

  // Returns the array of elements in layers_
  const dc::dcGenericResource::Array& getMapLayers();

  // Used during compilation to build the triggers for this map
  size_t getNumberOfTriggers() const;
  void compileTrigger(Evidyon::Trigger::TriggerIndex index,
                      Evidyon::Trigger::Trigger* trigger);

  // Saves changes to in-memory map masks
  void saveChangesToMapMasks(LPDIRECT3DDEVICE9 d3d_device);

  // Generates a new map layer
  template <typename T> T* createLayer(const std::string& name) {
    T* layer = (T*)layers_.create<T>(name);
    layer->setPriority(layers_.getMembers().size());
    return layer;
  }

  // Creates an inhabitant at the given coordinates.  If one already exists
  // there, that inhabitant is erased.
  void createInhabitant(int x, int z, Evidyon::Tools::LifeformAIElement* lifeform);

  // Removes the inhabitant present at the given coordinates
  void eraseInhabitant(int x, int z);

  // Writes all the geosids in this map into the compiler
  void compileGeosids(Geosid::Tools::GeosidCompiler* geosid_compiler,
                      MapCompilationContext* context);

  size_t numberOfGeoniteBurstLocations();
  void compileGeoniteBurstLocation(
    size_t index,
    Magic::Tools::MagicCompiler* magic_compiler,
    int* x,
    int* y,
    Magic::MagicIndex* magic,
    Geosid::Geonite* geonite);

  const dc::dcGenericResource::Array& getGeosids() { return geosids_.getMembers(); }

private:

  // Writes the border information into the compilation context
  //void compileBorders(const RECT* area,
  //                    MapCompilationContext* compilation_context);

  void compileTriggers(MapCompilationContext* compilation_context);

private:

  // All the masks that define this map
  dc::dcList<MapMask> masks_;

  // Layers that reference map masks and define everything about the map.  This
  // table is sorted by layer priority before compiling the map.
  dc::dcTable<MapLayer> layers_;

  // All of the geosids on this map
  Evidyon::Tools::GeosidList geosids_;

  // All locations are initialized to be in this zone
  Evidyon::Tools::ZoneReference default_zone_;

  // Lifeforms that always populate this map
  dc::dcList<LifeformAIInhabitantSpawn> inhabitants_;

  // The border of the map is configured so that walking over it wraps players
  // around to the other side.
  dc::dcInteger border_size_;
  Evidyon::Tools::ZoneReference border_zone_;

  // Defines locations where burst has a special effect
  //GeoniteBurstList geonite_burst_locations_;

  dc::dcList<MapTrigger> map_triggers_;
};

}
}
}

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::World::Tools::Map> MapList;
typedef dc::dcList<Evidyon::World::Tools::Map>::Element MapElement;
typedef dc::dcList<Evidyon::World::Tools::Map>::Reference MapReference;
}
}

#endif
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
#include "map.h"
#include "mapcompilationcontext.h"
#include "common/safemacros.h"
#include <assert.h>


namespace Evidyon {
namespace World {
namespace Tools {


static const unsigned int MAX_MAP_DIMENSION = 2048;



//----[  Map  ]----------------------------------------------------------------
Map::Map() {
  member("Masks", &masks_);
  member("Layers", &layers_);
  member("Geosids", &geosids_);
  //member("Geonite Burst Locations", &geonite_burst_locations_);
  member("Default Zone", &default_zone_);
  member("Inhabitants", &inhabitants_);
  member("Triggers", &map_triggers_);
}





//----[  createCompilationContext  ]-------------------------------------------
bool Map::createCompilationContext(LPDIRECT3DDEVICE9 d3d_device,
                                   MapCompilationContext* context) {
  const dc::dcGenericResource::Array& masks = masks_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;

  // Determine the overall dimensions of the map
  unsigned int width = 0, height = 0;
  for (i = masks.begin(); i != masks.end(); ++i) {
    MapMask* mask = (MapMask*)(*i);
    if (!mask->load(d3d_device, false)) return false;
    mask->expandDimensionsToContainMask(&width, &height);
  }

  // Allocate the map
  if (width > MAX_MAP_DIMENSION || height > MAX_MAP_DIMENSION) return false;


  MapLocationData* default_location = &context->default_location;
  default_location->this_layer.last_modified_by.map_layer_index = size_t(-1);
  default_location->this_layer.navigability = NAVIGABILITY_IMPASSABLE;
  default_location->this_layer.number_of_spawns = 0;
  default_location->this_layer.spawn_type = MapLocationData::INVALID_SPAWN_TYPE;
  default_location->this_layer.spawn_intended_level = -1;
  default_location->this_layer.zone
    = (Zone::ZoneIndex)default_zone_.getReferencedResourceIndex();
  default_location->this_layer.trigger = Evidyon::Trigger::INVALID_TRIGGER_INDEX;
  for (int i = 0; i < MapLocationData::NUMBER_OF_SPAWNS; ++i) {
    default_location->this_layer.spawn[i]
      = Evidyon::LifeformAI::INVALID_LIFEFORM_AI_INDEX;
  }
  default_location->this_layer.visual_data.terrain_height = 0.0f;
  default_location->this_layer.visual_data.fluid_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location->this_layer.visual_data.occluding = false;
  default_location->this_layer.visual_data.scenery = Scenery::INVALID_SCENERY_INDEX;
  default_location->this_layer.visual_data.skinned_mesh = SkinnedMesh::INVALID_SKINNED_MESH_INDEX;
  default_location->this_layer.visual_data.terrain_texture_rotation = 0;
  default_location->this_layer.visual_data.terrain_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location->this_layer.visual_data.wall_texture = Texture::INVALID_TEXTURE_INDEX;
  default_location->this_layer.visual_data.wall_type = WALLTYPE_FLOOR;
  memcpy(&default_location->last_layer,
         &default_location->this_layer,
         sizeof(default_location->last_layer));

  context->default_location.neighbors_nesw[0] = default_location;
  context->default_location.neighbors_nesw[1] = default_location;
  context->default_location.neighbors_nesw[2] = default_location;
  context->default_location.neighbors_nesw[3] = default_location;

  unsigned int number_of_locations = width*height;
  MapLocationData* locations = new MapLocationData[number_of_locations];
  memset(locations,0,sizeof(MapLocationData)*number_of_locations);
  context->locations = locations;
  context->width = width;
  context->height = height;

  // Initialize all of the neighbor pointers
  MapLocationData* location = locations;
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      location->neighbors_nesw[0] = (y == 0) ? default_location : &locations[(y-1)*width+x];
      location->neighbors_nesw[1] = (x + 1 == width)  ? default_location : &locations[y*width+x+1];
      location->neighbors_nesw[2] = (y + 1 == height) ? default_location : &locations[(y+1)*width+x];
      location->neighbors_nesw[3] = (x == 0) ? default_location : &locations[y*width+x-1];
      ++location;
    }
  }

  // Success
  return true;
}





//----[  destroyCompilationContext  ]------------------------------------------
void Map::destroyCompilationContext(MapCompilationContext* context) {
  SAFE_DELETE_ARRAY(context->locations);
  context->width = 0;
  context->height = 0;
}








//----[  sortMapLayers  ]------------------------------------------------------
void Map::sortMapLayers() {
  layers_.sort();
}





//----[  buildContextContents  ]-----------------------------------------------
void Map::buildContextContents(const RECT* area,
                               MapCompilationContext* context) {
  assert(context);

  unsigned int width = context->width;
  unsigned int height = context->height;
  RECT full_area = { 0, 0, width, height };

  // Erase all triggers; they will have to be recreated by the layers
  context->triggers.clear();

  // Reset the data in the context for this area
  if (!area) area = &full_area;
  //if (area)
  {
    
    // No part of the area intersects the valid region
    if (area->left >= long(width) ||
        area->top >= long(height) ||
        area->bottom < 0 ||
        area->right < 0) return;

    // Bound the area to valid coordinates
    RECT valid_area = *area;
    valid_area.left = max(valid_area.left, full_area.left);
    valid_area.top  = max(valid_area.top,  full_area.top);
    valid_area.right = min(valid_area.right, full_area.right);
    valid_area.bottom = min(valid_area.bottom, full_area.bottom);
    area = &valid_area;

    // Reset to the default type
    for (int y = area->top; y < area->bottom; ++y) {
      for (int x = area->left; x < area->right; ++x) {
        MapLocationData* location = context->atOrNull(x,y);
        assert(location);
        memcpy(&location->this_layer,
               &context->default_location.this_layer,
               sizeof(location->this_layer));
      }
    }

    { // Sort the masks and apply them
      masks_.sort();
      const dc::dcGenericResource::Array& masks = masks_.getMembers();
      dc::dcGenericResource::Array::const_iterator i;
      for (i = masks.begin(); i != masks.end(); ++i) {
        dc::dcList<MapMask>::Element* map_mask
          = (dc::dcList<MapMask>::Element*)(*i);
        map_mask->compile(context, area);
      }

      //// Copy all of the data down for all of the tiles in the map
      //for (unsigned int y = 0; y < height; ++y) {
      //  for (unsigned int x = 0; x < width; ++x) {
      //    MapLocationData* location = &context->locations[y*width+x];
      //    memcpy(&location->last_layer,
      //           &location->this_layer,
      //           sizeof(location->last_layer));
      //  }
      //}

      // updated code 9/26/09: to allow for bigger maps to not explode the
      // editor (by making it really slow) this now only copies back
      // stuff in the area that was updated
      for (unsigned int y = valid_area.top; y < valid_area.bottom; ++y) {
        for (unsigned int x = valid_area.left; x < valid_area.right; ++x) {
          MapLocationData* location = &context->locations[y*width+x];
          memcpy(&location->last_layer,
                 &location->this_layer,
                 sizeof(location->last_layer));
        }
      }
    }

    // Compile each of the layers into the map
    sortMapLayers();
    const dc::dcGenericResource::Array& layers = layers_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = layers.begin(); i != layers.end(); ++i) {
      const dc::dcTable<MapLayer>::Element* map_layer_interface
        = (dc::dcTable<MapLayer>::Element*)(*i);
      MapLayer* map_layer = map_layer_interface->getImplementation();

      context->accessed = false;
      map_layer->compile(area, context);

      if (context->accessed) {

        // Copy all of the layer data down for all of the entries in this area
        for (int y = area->top; y < area->bottom; ++y) {
          for (int x = area->left; x < area->right; ++x) {
            MapLocationData* location = context->atOrNull(x,y);
            assert(location);
            memcpy(&location->last_layer,
                   &location->this_layer,
                   sizeof(location->last_layer));
          }
        }
      }
    }

/*
    { // Finalize the map navigability by setting all transitional
      // navigabilities equal to those of the surrounding 
      // Copy all of the location data down for the map
      MapLocationData* location = context->locations;
      for (unsigned int i = 0; i < width*height; ++i, ++location) {
        switch (location->last_layer.navigability) {
        case NAVIGABILITY_PIT:
          // if this location is in a corner of non-pit navigabilities,
          // it should have the same navigability as those that form the
          // corner
          break;
        case NAVIGABILITY_WADE:
          // if this location is next to a NORMAL navigability, make
          // this location a NORMAL navigability
          break;
        default:
          break;
        }
      }
    }*/
  }
  //else {
  //  unsigned int size = width * height;
  //  MapLocationData* location = context->locations;
  //  for (unsigned int i = 0; i < size; ++i, ++location) {
  //    memcpy(location, &context->default_location, sizeof(*location));
  //  }

  //  area = &full_area;

  //  // Compile each of the layers into the map
  //  const dc::dcGenericResource::Array& layers = layers_.getMembers();
  //  dc::dcGenericResource::Array::const_iterator i;
  //  for (i = layers.begin(); i != layers.end(); ++i) {
  //    const dc::dcTable<MapLayer>::Element* map_layer_interface
  //      = (dc::dcTable<MapLayer>::Element*)(*i);
  //    MapLayer* map_layer = map_layer_interface->getImplementation();
  //    map_layer->compile(area, context);

  //    // Copy all of the location data down for the map
  //    location = context->locations;
  //    for (unsigned int i = 0; i < size; ++i, ++location) {
  //      memcpy(&location->last_layer,
  //             &location->this_layer,
  //             sizeof(location->last_layer));
  //    }
  //  }
  //}

  { // Write the geosids into the map
    const dc::dcGenericResource::Array& geosids = geosids_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = geosids.begin(); i != geosids.end(); ++i) {
      Evidyon::Tools::GeosidElement* geosid = 
        (Evidyon::Tools::GeosidElement*)(*i);
      geosid->compile(context);
    }
  }

  { // Write the inhabitants into the map
    const dc::dcGenericResource::Array& inhabitants = inhabitants_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = inhabitants.begin(); i != inhabitants.end(); ++i) {
      const dc::dcList<LifeformAIInhabitantSpawn>::Element* inhabitant = 
        (dc::dcList<LifeformAIInhabitantSpawn>::Element*)(*i);
      inhabitant->compile(context);
    }
  }

  compileTriggers(context);
}



//----[  buildBrushPreview  ]--------------------------------------------------
void Map::buildBrushPreview(MapMask* mask,
                            D3DCOLOR color,
                            LocationVisualData* visual_data) {
  const dc::dcGenericResource::Array& layers = layers_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  for (i = layers.begin(); i != layers.end(); ++i) {
    const dc::dcTable<MapLayer>::Element* map_layer_interface
      = (dc::dcTable<MapLayer>::Element*)(*i);
    MapLayer* map_layer = map_layer_interface->getImplementation();
    map_layer->brushPreview(mask, color, visual_data);
  }
}


//----[  getMapMasks  ]--------------------------------------------------------
const dc::dcGenericResource::Array& Map::getMapMasks() {
  return masks_.getMembers();
}


//----[  getMapLayers  ]-------------------------------------------------------
const dc::dcGenericResource::Array& Map::getMapLayers() {
  return layers_.getMembers();
}


//----[  saveChangesToMapMasks  ]----------------------------------------------
void Map::saveChangesToMapMasks(LPDIRECT3DDEVICE9 d3d_device) {
  const dc::dcGenericResource::Array& masks = masks_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  unsigned int width = 0, height = 0;
  for (i = masks.begin(); i != masks.end(); ++i) {
    MapMask* mask = (MapMask*)(*i);
    mask->saveChangesToMask(d3d_device);
  }
}




//----[  createInhabitant  ]---------------------------------------------------
void Map::createInhabitant(int x, int z, Evidyon::Tools::LifeformAIElement* lifeform) {
  char name[256];
  sprintf_s(name, sizeof(name), "%s at %i, %i", lifeform->getName().c_str(), x, z);
  eraseInhabitant(x, z);
  dc::dcList<LifeformAIInhabitantSpawn>::Element* inhabitant = 
    (dc::dcList<LifeformAIInhabitantSpawn>::Element*)
      (inhabitants_.generateElement(name));
  inhabitant->set(x, z, lifeform);
}




//----[  eraseInhabitant  ]----------------------------------------------------
void Map::eraseInhabitant(int x, int z) {
  const dc::dcGenericResource::Array& inhabitants = inhabitants_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  for (i = inhabitants.begin(); i != inhabitants.end(); ++i) {
    dc::dcList<LifeformAIInhabitantSpawn>::Element* inhabitant = 
      (dc::dcList<LifeformAIInhabitantSpawn>::Element*)(*i);
    if (inhabitant->locatedAt(x, z)) {
      inhabitants_.releaseElement(inhabitant);
      i = inhabitants.begin();
    }
  }
}





//----[  compileGeosids  ]-----------------------------------------------------
void Map::compileGeosids(Geosid::Tools::GeosidCompiler* geosid_compiler,
                         MapCompilationContext* context) {
  World::MapIndex this_map_index = getIndex();

  const dc::dcGenericResource::Array& geosids = geosids_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  for (i = geosids.begin(); i != geosids.end(); ++i) {
    Evidyon::Tools::GeosidElement* geosid = 
      (Evidyon::Tools::GeosidElement*)(*i);
    geosid->compile(this_map_index, geosid_compiler, context);
  }
}





//----[  numberOfGeoniteBurstLocations  ]--------------------------------------
size_t Map::numberOfGeoniteBurstLocations() {
  //return geonite_burst_locations_.getMembers().size();
  return 0;
}



//----[  compileGeoniteBurstLocation  ]----------------------------------------
void Map::compileGeoniteBurstLocation(
    size_t index,
    Magic::Tools::MagicCompiler* magic_compiler,
    int* x,
    int* y,
    Magic::MagicIndex* magic,
    Geosid::Geonite* geonite) {
      assert(false);
  //GeoniteBurstElement* burst = (GeoniteBurstElement*)
  //  (geonite_burst_locations_.getMembers().at(index));
  //burst->compile(magic_compiler, x, y, magic, geonite);
}





//----[  compileTriggers  ]----------------------------------------------------
void Map::compileTriggers(MapCompilationContext* compilation_context) {

  const dc::dcGenericResource::Array& triggers = map_triggers_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;

  // Determine the overall dimensions of the map
  unsigned int width = 0, height = 0;
  for (i = triggers.begin(); i != triggers.end(); ++i) {
    MapTrigger* map_trigger = (MapTrigger*)(*i);
    map_trigger->compile(compilation_context);
  }

}

//
////----[  compileBorders  ]-----------------------------------------------------
//void Map::compileBorders(const RECT* area,
//                         MapCompilationContext* compilation_context) {

  //int border_zone_size = border_size_.getValue();
  //int border_trigger_size = border_zone_size / 2;
  //int wrap_width = compilation_context->width - border_zone_size;
  //int wrap_height = compilation_context->height - border_zone_size;
  //Zone::ZoneIndex border_zone = border_zone_.getReferencedResourceIndex();
  //int border_trigger_width = compilation_context->width - border_trigger_size;
  //int border_trigger_height = compilation_context->height - border_trigger_size;

  //// Add the wrapping triggers
  //// 0 1 2    0->H    4->D
  ////  ABC     1->G    5->C
  //// 3D E4    2->F    6->B
  ////  FGH     3->E    7->A
  //// 5 6 7    
  //Evidyon::Trigger::Trigger trigger;
  //Evidyon::Trigger::TriggerIndex trigger_indices[8];
  //trigger.type = Trigger::Trigger::TELEPORT_OFFSET;

  //trigger.teleport_offset.added_x = wrap_width;
  //trigger.teleport_offset.added_z = wrap_height;
  //trigger_indices[0] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = 0;
  //trigger.teleport_offset.added_z = wrap_height;
  //trigger_indices[1] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = -wrap_width;
  //trigger.teleport_offset.added_z = wrap_height;
  //trigger_indices[2] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = wrap_width;
  //trigger.teleport_offset.added_z = 0;
  //trigger_indices[3] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = -wrap_width;
  //trigger.teleport_offset.added_z = 0;
  //trigger_indices[4] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = wrap_width;
  //trigger.teleport_offset.added_z = -wrap_height;
  //trigger_indices[5] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = 0;
  //trigger.teleport_offset.added_z = -wrap_height;
  //trigger_indices[6] = compilation_context->addTrigger(&trigger);
  //trigger.teleport_offset.added_x = -wrap_width;
  //trigger.teleport_offset.added_z = -wrap_height;
  //trigger_indices[7] = compilation_context->addTrigger(&trigger);

  //for (int z = area->top; z < area->bottom; ++z) {
  //  for (int x = area->left; x < area->right; ++x) {
  //    MapLocationData* location = compilation_context->atOrNull(x, z);
  //    if (!location) continue;

  //    // Find the wrapping trigger type for this location
  //    bool top = z < border_trigger_size,
  //         left = x < border_trigger_size,
  //         right = x >= border_trigger_width,
  //         bottom = z >= border_trigger_height;
  //    Trigger::TriggerIndex trigger = Trigger::INVALID_TRIGGER_INDEX;
  //    if (top) {
  //      if (left) {
  //        trigger = trigger_indices[0];
  //      } else if (right) {
  //        trigger = trigger_indices[2];
  //      } else {
  //        trigger = trigger_indices[1];
  //      }
  //    } else if (bottom) {
  //      if (left) {
  //        trigger = trigger_indices[5];
  //      } else if (right) {
  //        trigger = trigger_indices[7];
  //      } else {
  //        trigger = trigger_indices[6];
  //      }
  //    } else if (left) {
  //      trigger = trigger_indices[3];
  //    } else if (right) {
  //      trigger = trigger_indices[4];
  //    }

  //    // copy the border left/right and top/bottom
  //    if (right) {
  //      int left = x - wrap_width;
  //      if (area->left <= left) {
  //        memcpy(compilation_context->atOrDefault(x, z),
  //               compilation_context->atOrDefault(left, z),
  //               sizeof(World::Tools::MapLocationData));
  //      }
  //    }
  //    if (bottom) {
  //      int top = z - wrap_height;
  //      if (area->top <= top) {
  //        memcpy(compilation_context->atOrDefault(x, z),
  //               compilation_context->atOrDefault(x, top),
  //               sizeof(World::Tools::MapLocationData));
  //      }
  //    }

  //    bool in_border_zone = x < border_zone_size ||
  //                          z < border_zone_size ||
  //                          x >= wrap_width ||
  //                          z >= wrap_height;
  //    if (in_border_zone) {
  //      location->this_layer.trigger = trigger;
  //      location->this_layer.zone = border_zone;
  //    }
  //  }
  //}
//}



}
}
}
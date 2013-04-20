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
#include "mapmask.h"
#include "common/safemacros.h"
#include <d3dx9.h>
#include <assert.h>
#include "mapcompilationcontext.h"
#include "maplocationdata.h"
#include "common/randf.h"


namespace Evidyon {
namespace World {
namespace Tools {


  
//----[  GetMapMaskInfo  ]-----------------------------------------------------
bool MapMask::GetMapMaskInfo(const char* file_name,
                             unsigned int* width,
                             unsigned int* height) {
  D3DXIMAGE_INFO maskInfo;
  CONFIRM(SUCCEEDED(D3DXGetImageInfoFromFile(file_name, &maskInfo))) else return false;
  *width = maskInfo.Width;
  *height = maskInfo.Height;
  return true;
}



//----[  LoadMapMaskData  ]----------------------------------------------------
bool MapMask::LoadMapMaskData(LPDIRECT3DDEVICE9 d3d_device,
                              const char* file_name,
                              unsigned int width,
                              unsigned int height,
                              D3DCOLOR* data_buffer) {
  LPDIRECT3DTEXTURE9 texture = NULL;
  HRESULT hr = D3DXCreateTextureFromFileEx(d3d_device,
                                           file_name,
                                           D3DX_DEFAULT, // width
                                           D3DX_DEFAULT, // height
                                           1, // mip levels
                                           0, // usage
                                           D3DFMT_X8R8G8B8, // full format
                                           D3DPOOL_SCRATCH,
                                           D3DX_FILTER_NONE,
                                           D3DX_FILTER_NONE,
                                           0, // no color key
                                           NULL,
                                           NULL,
                                          &texture);
  CONFIRM(SUCCEEDED(hr)) else return false;
  D3DLOCKED_RECT lr;
  CONFIRM(SUCCEEDED(texture->LockRect(0, &lr, NULL, D3DLOCK_READONLY))) else {
    SAFE_RELEASE(texture);
    return false;
  }

  // Copy data directly into the data buffer
  ptrdiff_t pitch_in_d3dcolors = lr.Pitch / sizeof(D3DCOLOR);
  for (unsigned int y = 0; y < height; ++y) {
    D3DCOLOR* src_bits = ((D3DCOLOR*)lr.pBits) + y * pitch_in_d3dcolors;
    for (unsigned int x = 0; x < width; ++x) {
      D3DCOLOR mask_color = 0xFF000000 | (*(src_bits + x));
      *data_buffer = mask_color;
      ++data_buffer;
    }
  }

  texture->UnlockRect(0);
  SAFE_RELEASE(texture);
  return true;
}


//----[  AcquireMapMask  ]-----------------------------------------------------
bool MapMask::AcquireMapMask(LPDIRECT3DDEVICE9 d3d_device,
                           const char* file_name,
                           unsigned int* width_out,
                           unsigned int* height_out,
                           D3DCOLOR** data_buffer) {
  unsigned int width, height;
  CONFIRM(GetMapMaskInfo(file_name,
                        &width,
                        &height)) else return false;

  D3DCOLOR* buffer = new D3DCOLOR[width*height];
  ASSERT(buffer != NULL) else return false;

  CONFIRM(LoadMapMaskData(d3d_device,
                          file_name,
                          width,
                          height,
                          buffer)) else {
    SAFE_DELETE_ARRAY(buffer);
    return false;
  }

  *width_out = width;
  *height_out = height;
  *data_buffer = buffer;
  return true;
}







//----[  MapMask  ]------------------------------------------------------------
MapMask::MapMask() {
  member("Source File", &source_file_);
  member("Colors", &color_data_);
  width_ = 0;
  height_ = 0;
  map_mask_ = NULL;
  modified_ = false;
}




//----[  MapMask  ]------------------------------------------------------------
MapMask::~MapMask() {
  destroy();
}



//----[  destroy  ]------------------------------------------------------------
void MapMask::destroy() {
  width_ = 0;
  height_ = 0;
  SAFE_DELETE_ARRAY(map_mask_);
  color_data_.clear();
  colors_.clear();
}





//----[  load  ]---------------------------------------------------------------
bool MapMask::load(LPDIRECT3DDEVICE9 d3d_device,
                   bool force_reload) {
  if (!map_mask_ || force_reload) {
    SAFE_DELETE_ARRAY(map_mask_);
    colors_.clear();

    modified_ = false;

    if (!AcquireMapMask(d3d_device,
                        source_file_.getValue().c_str(),
                        &width_,
                        &height_,
                        &map_mask_)) return false;
  }

  unsigned int size = width_*height_;
  for (unsigned int i = 0; i < size; ++i) {
    colors_.insert(map_mask_[i]);
  }

  updateColorData();

  // Success
  return true;
}





//----[  saveChangesToMask  ]--------------------------------------------------
void MapMask::saveChangesToMask(LPDIRECT3DDEVICE9 d3d_device) {
  if (!modified_) return;
  std::string source_file = source_file_.getValue();
  std::string extension = source_file.substr(source_file.find_last_of("."));
  if (source_file.empty()) return;
  D3DXIMAGE_FILEFORMAT iff;
  if (extension.compare(".bmp") == 0) {
    iff = D3DXIFF_BMP;
  } else if (extension.compare(".png") == 0) {
    iff = D3DXIFF_PNG;
  } else if (extension.compare(".tga") == 0) {
    iff = D3DXIFF_TGA;
  } else if (extension.compare(".dds") == 0) {
    iff = D3DXIFF_DDS;
  } else {
    char buffer[512];
    std::string new_source_file = source_file + ".png";
    sprintf_s(buffer, 512, "Invalid save file format for map mask; "\
                           "saving '%s' as PNG file '%s'",
                           source_file.c_str(),
                           new_source_file.c_str());
    MessageBox(NULL, buffer, "Warning: Changing Map Mask File", MB_OK);
    source_file.swap(new_source_file);
    iff = D3DXIFF_PNG;
    source_file_.setValue(new_source_file);
  }
  if (map_mask_) {
    LPDIRECT3DSURFACE9 mask_surface = NULL;
    HRESULT hr = d3d_device->CreateOffscreenPlainSurface(width_,
                                                         height_,
                                                         D3DFMT_X8R8G8B8,
                                                         D3DPOOL_SCRATCH,
                                                         &mask_surface,
                                                         NULL);
    if (SUCCEEDED(hr)) {
      D3DLOCKED_RECT lr;
      mask_surface->LockRect(&lr, NULL, 0);
      BYTE* bits = (BYTE*)lr.pBits;
      size_t row_bytes = sizeof(D3DCOLOR)*width_;
      for (unsigned int y = 0; y < height_; ++y) {
        memcpy(bits + (lr.Pitch*y), &map_mask_[y*width_], row_bytes);
      }
      mask_surface->UnlockRect();
      hr = D3DXSaveSurfaceToFile(source_file.c_str(),
                                 iff,
                                 mask_surface,
                                 NULL,
                                 NULL);
    }
    if (FAILED(hr)) {
      char buffer[512];
      sprintf_s(buffer, 512, "There was a problem "\
                             "saving map mask '%s' into '%s'",
                             getName().c_str(),
                             source_file.c_str());
      MessageBox(NULL, buffer, "Error Saving Map Mask", MB_OK);
    }

    SAFE_RELEASE(mask_surface);
  }
}








//----[  updateColorData  ]----------------------------------------------------
void MapMask::updateColorData() {
  // Make sure all mask colors are in the list
  for (Colors::iterator i = colors_.begin(); i != colors_.end(); ++i) {
    D3DCOLOR color = *i;
    if (!colorData(color)) {
      char name[64];
      sprintf_s(name,
                "Color (%i, %i, %i)",
                0xFF&(color>>16), // R
                0xFF&(color>>8),  // G
                0xFF&(color>>0)); // B
      MapMaskColorData* element
        = (MapMaskColorData*)color_data_.generateElement(name);
      element->getColor()->setD3DColor(color);
    }
  }

  // Make sure all list entries are in the mask
  const dc::dcGenericResource::Array& members = color_data_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end();) {
    const MapMaskColorData* element = (MapMaskColorData*)(*i);
    if (colors_.find(element->getD3DColor()) == colors_.end()) {
      color_data_.releaseElement(
        const_cast<MapMaskColorData*>(element)); // TODO: shouldn't have to const_cast
      i = members.begin();
    } else {
      ++i;
    }
  }
}





//
////----[  getColorNames  ]------------------------------------------------------
//const dc::dcList<dcx::dcxD3DColor>& MapMask::getColorNames() {
//  return colors_;
//}





//----[  colorName  ]----------------------------------------------------------
const MapMaskColorData* MapMask::colorData(D3DCOLOR color) const {
  const dc::dcGenericResource::Array& members = color_data_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end(); ++i) {
    const MapMaskColorData* element = (const MapMaskColorData*)(*i);
    if (element->getD3DColor() == color) return element;
  }
  return NULL; // color couldn't be found
}





//----[  getFirstLocation  ]---------------------------------------------------
bool MapMask::getFirstLocation(D3DCOLOR color,
                               int* x,
                               int* y) {
  if (!map_mask_) return false;
  if (colors_.find(color) == colors_.end()) return false;
  return getNextLocation(color, -1, 0, x, y);
}






//----[  getNextLocation  ]----------------------------------------------------
bool MapMask::getNextLocation(D3DCOLOR color,
                              int last_x,
                              int last_y,
                              int* x,
                              int* y) {
  if (!map_mask_) return false;
  ++last_x;
  for (; last_y < height_; ++last_y) {
    int last_y_base = last_y * width_;
    for (; last_x < width_; ++last_x) {
      if (map_mask_[last_y_base + last_x] == color) {
        *x = last_x;
        *y = last_y;
        return true;
      }
    }
    last_x = 0;
  }
  return false; // no more could be found
}





//----[  getFirstLocationInArea  ]---------------------------------------------
bool MapMask::getFirstLocationInArea(D3DCOLOR color,
                                     const RECT* area,
                                     int* x,
                                     int* y) {
  if (!map_mask_) return false;
  return getNextLocationInArea(color, area->left - 1, area->top, area, x, y);
}




//----[  getNextLocationInArea  ]----------------------------------------------
bool MapMask::getNextLocationInArea(D3DCOLOR color,
                                    int last_x,
                                    int last_y,
                                    const RECT* area,
                                    int* x,
                                    int* y) {
  if (!map_mask_) return false;
  ++last_x;
  for (; last_y < area->bottom; ++last_y) {
    int last_y_base = last_y * width_;
    for (; last_x < area->right; ++last_x) {
      if (map_mask_[last_y_base + last_x] == color) {
        *x = last_x;
        *y = last_y;
        return true;
      }
    }
    last_x = area->left;
  }
  return false; // no more could be found
}



//----[  expandDimensionsToContainMask  ]--------------------------------------
void MapMask::expandDimensionsToContainMask(unsigned int* width,
                                            unsigned int* height) {
  if (*width < width_) *width = width_;
  if (*height < height_) *height = height_;
}





//----[  setSourceFile  ]------------------------------------------------------
bool MapMask::setSourceFile(const char* source_file,
                            LPDIRECT3DDEVICE9 d3d_device) {
  source_file_.setValue(source_file);
  return load(d3d_device, true);
}



//----[  loadSourceFileTexture  ]----------------------------------------------
bool MapMask::loadSourceFileTexture(LPDIRECT3DDEVICE9 d3d_device,
                                    LPDIRECT3DTEXTURE9* d3d_texture) {
  if (map_mask_ == NULL) {
    return SUCCEEDED(D3DXCreateTextureFromFileEx(d3d_device,
                                                 source_file_.getValue().c_str(),
                                                 D3DX_DEFAULT,
                                                 D3DX_DEFAULT,
                                                 1,
                                                 0,
                                                 D3DFMT_X8R8G8B8,
                                                 D3DPOOL_MANAGED,
                                                 D3DX_DEFAULT,
                                                 D3DX_FILTER_POINT,
                                                 0, // no color key
                                                 NULL,
                                                 NULL,
                                                 d3d_texture));
  } else {
    // This texture exists in memory and may be different from what is
    // present on disk.  To make sure everything's correct, we'll create
    // the texture and copy data into it from RAM.
    LPDIRECT3DTEXTURE9 texture = NULL;
    HRESULT hr = D3DXCreateTexture(d3d_device,
                                   width_,
                                   height_,
                                   -1,
                                   0,
                                   D3DFMT_X8R8G8B8,
                                   D3DPOOL_MANAGED,
                                   &texture);
    if (FAILED(hr)) return false;
    D3DLOCKED_RECT lr;
    DWORD levels = texture->GetLevelCount();
    for (DWORD level = 0; level < levels; ++level) {
      texture->LockRect(level, &lr, NULL, 0);
      D3DCOLOR* dest_bits = (D3DCOLOR*)lr.pBits;
      DWORD level_height = height_>>level;
      DWORD level_width = width_>>level;
      DWORD pitch_in_D3DCOLORs = lr.Pitch / sizeof(D3DCOLOR);
      for (long y = 0; y < level_height; ++y) {
        unsigned int y_times_width = (y<<level)*level_width;
        for (long x = 0; x < level_width; ++x) {
          dest_bits[y*pitch_in_D3DCOLORs + x] = map_mask_[y_times_width + (x<<level)];
        }
      }
      texture->UnlockRect(level);
    }

    // Success!
    *d3d_texture = texture;
    return true;
  }
}



//----[  paint  ]--------------------------------------------------------------
D3DCOLOR MapMask::paint(int x, int y, D3DCOLOR color) {
  if (!map_mask_ || x < 0 || y < 0 || x >= width_ || y >= height_) return 0;
  unsigned int index = y * width_ + x;
  D3DCOLOR old_color = map_mask_[index];
  map_mask_[index] = 0xFF000000 | color;
  modified_ = true;
  return old_color;
}


//----[  compareTo  ]----------------------------------------------------------
int MapMask::compareTo(const dc::dcGenericResource* other) const {
  if (!other || getOwner()->getTypeID() != other->getOwner()->getTypeID()) {
     return dc::dcGenericResource::compareTo(other);
  } else {
    return priority_.compareTo(&((const MapMask*)other)->priority_);
  }
}



//----[  setPriority  ]--------------------------------------------------------
void MapMask::setPriority(int priority) {
  priority_.setValue(priority);
}



//----[  compile  ]------------------------------------------------------------
void MapMask::compile(MapCompilationContext* compilation_context) {
  const dc::dcGenericResource::Array& members = color_data_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end(); ++i) {
    const MapMaskColorData* element = (const MapMaskColorData*)(*i);
    D3DCOLOR color = element->getD3DColor();
    Navigability navigability;
    bool set_navigability = element->getNavigability(&navigability);
    int swarm_min_level, swarm_level_range;
    bool set_swarm_level
      = element->getSwarmLevel(&swarm_min_level, &swarm_level_range);
    Zone::ZoneIndex zone;
    bool set_zone = element->getZone(&zone);
    static const size_t MAX_SWARM_SPAWNS = 16;
    SwarmSpawn::Data swarm_spawns[MAX_SWARM_SPAWNS];
    size_t number_of_swarm_spawns
      = element->getSwarms(swarm_spawns, MAX_SWARM_SPAWNS);
    if (!set_navigability &&
        !set_swarm_level &&
        !set_zone &&
        !number_of_swarm_spawns) {
      continue; // no iteration
    }
    int x, y;
    if (!getFirstLocation(color, &x, &y)) continue;
    do {
      MapLocationData* location = compilation_context->atOrNull(x, y);
      assert(location);
      if (set_navigability) {
        location->this_layer.navigability = navigability;
      }
      if (set_swarm_level) {
        location->this_layer.spawn_intended_level
          = (rand() % swarm_level_range) + swarm_min_level;
      }
      if (set_zone) {
        location->this_layer.zone = zone;
      }
      for (size_t i = 0; i < number_of_swarm_spawns; ++i) {
        if (randf() >= swarm_spawns[i].chance_of_appearing) continue;
        size_t left_to_pick = swarm_spawns[i].number_to_choose;
        while (left_to_pick-- > 0) {
          if (location->this_layer.number_of_spawns
                >= location->NUMBER_OF_SPAWNS) {
            break;
          }

          location->this_layer.spawn
            [location->this_layer.number_of_spawns++]
              = swarm_spawns[i].lifeform_ai
                [rand()%SwarmSpawn::Data::NUMBER_OF_LIFEFORM_AI];
        }
      }
    } while (getNextLocation(color, x, y, &x, &y));
  }
}




//----[  compile  ]------------------------------------------------------------
void MapMask::compile(MapCompilationContext* compilation_context,
                      const RECT* area) {
  const dc::dcGenericResource::Array& members = color_data_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
       i != members.end(); ++i) {
    const MapMaskColorData* element = (const MapMaskColorData*)(*i);
    D3DCOLOR color = element->getD3DColor();
    Navigability navigability;
    bool set_navigability = element->getNavigability(&navigability);
    int swarm_min_level, swarm_level_range;
    bool set_swarm_level
      = element->getSwarmLevel(&swarm_min_level, &swarm_level_range);
    Zone::ZoneIndex zone;
    bool set_zone = element->getZone(&zone);
    static const size_t MAX_SWARM_SPAWNS = 16;
    SwarmSpawn::Data swarm_spawns[MAX_SWARM_SPAWNS];
    size_t number_of_swarm_spawns
      = element->getSwarms(swarm_spawns, MAX_SWARM_SPAWNS);
    if (!set_navigability &&
        !set_swarm_level &&
        !set_zone &&
        !number_of_swarm_spawns) {
      continue; // no iteration
    }
    int x, y;
    if (!getFirstLocationInArea(color, area, &x, &y)) continue;
    do {
      MapLocationData* location = compilation_context->atOrNull(x, y);
      assert(location);
      if (set_navigability) {
        location->this_layer.navigability = navigability;
      }
      if (set_swarm_level) {
        location->this_layer.spawn_intended_level
          = (rand() % swarm_level_range) + swarm_min_level;
      }
      if (set_zone) {
        location->this_layer.zone = zone;
      }
      for (size_t i = 0; i < number_of_swarm_spawns; ++i) {
        if (randf() >= swarm_spawns[i].chance_of_appearing) continue;
        size_t left_to_pick = swarm_spawns[i].number_to_choose;
        while (left_to_pick-- > 0) {
          if (location->this_layer.number_of_spawns
                >= location->NUMBER_OF_SPAWNS) {
            break;
          }

          location->this_layer.spawn
            [location->this_layer.number_of_spawns++]
              = swarm_spawns[i].lifeform_ai
                [rand()%SwarmSpawn::Data::NUMBER_OF_LIFEFORM_AI];
        }
      }
    } while (getNextLocationInArea(color, x, y, area, &x, &y));
  }
}



}
}
}
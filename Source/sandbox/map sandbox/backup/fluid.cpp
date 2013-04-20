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
#include "fluid.h"
#include "map.h"
#include "../shared/evidyond3drenderstatemanager.h"


namespace Evidyon {
namespace Map {

static const FLOAT FLUID_HEIGHT = -0.25f;


//----[  Fluid  ]---------------------------------------------------------------
Fluid::Fluid() {
}



//----[  update  ]-------------------------------------------------------------
void Fluid::update(MapManager* map,
                   int center_x,
                   int center_y,
                   const TileVisibility* visibility,
                   bool visible_areas_valid[2],
                   const VisibleTilesRect::Area new_visible_areas[2]) {

  bool fluid_changed_or_is_visible = false;
  if (visible_areas_valid[0]) {
    fluid_changed_or_is_visible = updateCachedTextures(false, map, &new_visible_areas[0]);
  }
  if (visible_areas_valid[1]) {
    fluid_changed_or_is_visible |= updateCachedTextures(fluid_changed_or_is_visible,
                                                        map,
                                                        &new_visible_areas[1]);
  }

  // For much of the map there is no fluid at all.  If this is the case, there is
  // no need to lock the buffers, update colors or do any other processing.
  if (fluid_changed_or_is_visible) {
    int first_visible_column = center_x - VISIBLE_HORIZONTAL_EXTENT;
    int first_visible_row = center_y - VISIBLE_VERTICAL_EXTENT;

    // Rewrite the fluid's colors based on which can be seen
    TerrainVertex* vertices = NULL;
    if (lockVB(first_visible_column,
               first_visible_row,
               visibility,
               &vertices)) {
      if (visible_areas_valid[0]) writeVertices(&new_visible_areas[0], vertices);
      if (visible_areas_valid[1]) writeVertices(&new_visible_areas[1], vertices);
      unlockVB();
    }

    // Write the batches
    updateIB(first_visible_column, first_visible_row, visibility);
  }
}






//----[  writeVertices  ]------------------------------------------------------
// This method is a heavily simplified version of Land::writeVertices, since
// neither the height of the fluid nor the texture coordinates ever change.
void Fluid::writeVertices(const VisibleTilesRect::Area* area,
                         TerrainVertex* vertices) {
  int row_less_1 = area->top - 1, row_add_1;
  float row_bottom = 1.0f * row_less_1 ;
  for (int row  = area->top;
           row <= area->bottom;
           row = row_add_1) {
    row_add_1 = row + 1;
    size_t quad_y_base = MAP_TO_RANGE(row, HEIGHT) * WIDTH * VERTICES_PER_QUAD;
    float row_top = row_bottom;
    row_bottom = 1.0f * row;
    float row_center = row_top + 0.5f;
    int col = area->left;
    int col_less_1 = col - 1, col_add_1 = col + 1;
    float col_right = 1.0f * col_less_1;
    do {
      size_t quad_x = MAP_TO_RANGE(col, WIDTH) * VERTICES_PER_QUAD;
      float col_left = col_right;
      float col_center = col_left + 0.5f;
      col_right = 1.0f * col;

      TerrainVertex* quad_vertices = vertices + quad_y_base + quad_x;

      quad_vertices[0].x = col_left;
      quad_vertices[0].z = row_top;
      quad_vertices[1].x = col_right;
      quad_vertices[1].z = row_top;
      quad_vertices[2].x = col_center;
      quad_vertices[2].z = row_center;
      quad_vertices[3].x = col_left;
      quad_vertices[3].z = row_bottom;
      quad_vertices[4].x = col_right;
      quad_vertices[4].z = row_bottom;

      if (col < area->right) {
        // get the next set of column heights
        col = col_add_1;
        col_add_1++;
      } else {
        // finished iterating through the columns
        break;
      }
    } while (true);

    row_less_1 = row;
  }
}





//----[  updateCachedTextures  ]-----------------------------------------------
bool Fluid::updateCachedTextures(bool already_found_fluid,
                                 const MapManager* map, 
                                 const VisibleTilesRect::Area* area) {
  TextureIndex* quad_textures = getCachedQuadTextures();

  // If fluid was found in another cache update, don't bother searching
  // or keeping track of changes during the cache update.
  if (already_found_fluid) {
    for (int row  = area->top;
             row <= area->bottom;
           ++row) {
      size_t y_base = MAP_TO_RANGE(row, HEIGHT) * WIDTH;
      for (int col  = area->left;
               col <= area->right;
             ++col) {
        size_t x = MAP_TO_RANGE(col, WIDTH);
        quad_textures[y_base + x] = map->fluidTexture(col, row);
      }
    }
  } else {
    bool changed = false;
    for (int row  = area->top;
             row <= area->bottom;
           ++row) {
      size_t y_base = MAP_TO_RANGE(row, HEIGHT) * WIDTH;
      for (int col  = area->left;
               col <= area->right;
             ++col) {
        size_t x = MAP_TO_RANGE(col, WIDTH);
        TextureIndex current_texture = quad_textures[y_base + x];
        TextureIndex new_texture = map->fluidTexture(col, row);
        quad_textures[y_base + x] = new_texture;
        changed |= current_texture != new_texture;
      }
    }

    // When nothing changes, search the complete list of cached textures
    // for any element that is valid
    if (changed) return true;
    for (int i = 0; i < VISIBLE_TILES; ++i, ++quad_textures) {
      if (*quad_textures != INVALID_TEXTURE_INDEX) return true;
    }
    
    // No fluid visible anywhere
    return false;
  }

  // There is fluid visible
  return true;
}




//----[  initializeVertexBuffer  ]---------------------------------------------
void Fluid::initializeVertexBuffer(TerrainVertex* vertices) const {
  for (int q = 0; q < VISIBLE_TILES; ++q) {
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices[1].u = 1.0f;
    vertices[1].v = 0.0f;
    vertices[2].u = 0.5f;
    vertices[2].v = 0.5f;
    vertices[3].u = 0.0f;
    vertices[3].v = 1.0f;
    vertices[4].u = 1.0f;
    vertices[4].v = 1.0f;
    for (int i = 0; i < VERTICES_PER_QUAD; ++i) {
      vertices->y = FLUID_HEIGHT;
      vertices->nx = 0.0f;
      vertices->ny = 1.0f;
      vertices->nz = 0.0f;
      ++vertices;
    }
  }
}


}
}
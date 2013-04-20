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
#include "land.h"
#include "map.h"
#include "../shared/evidyond3drenderstatemanager.h"


namespace Evidyon {
namespace Map {




//----[  Land  ]---------------------------------------------------------------
Land::Land() {
}



//----[  update  ]-------------------------------------------------------------
void Land::update(MapManager* map,
            int center_x,
            int center_y,
            const TileVisibility* visibility,
            bool visible_areas_valid[2],
            const VisibleTilesRect::Area new_visible_areas[2]) {
  VisibleTilesRect::Area new_cached_height_area[2];
  cached_height_visible_tiles_.moveTo(center_x,
                                      center_y,
                                      CACHED_HEIGHT_HORIZONTAL_EXTENT,
                                      CACHED_HEIGHT_VERTICAL_EXTENT,
                                      &new_cached_height_area[0],
                                      &new_cached_height_area[1]);

  updateCachedHeights(map, &new_cached_height_area[0]);
  updateCachedHeights(map, &new_cached_height_area[1]);

  int first_visible_column = center_x - VISIBLE_HORIZONTAL_EXTENT;
  int first_visible_row = center_y - VISIBLE_VERTICAL_EXTENT;

  TerrainVertex* vertices = NULL;
  if (lockVB(first_visible_column,
             first_visible_row,
             visibility,
             &vertices)) {
    if (visible_areas_valid[0]) {
      updateCachedTextures(map, &new_visible_areas[0]);
      writeVertices(&new_visible_areas[0], vertices);
    }
    if (visible_areas_valid[1]) { 
      updateCachedTextures(map, &new_visible_areas[1]);
      writeVertices(&new_visible_areas[1], vertices);
    }
    unlockVB();
  }

  // Write the batches
  updateIB(first_visible_column, first_visible_row, visibility);
}



//----[  updateCachedHeights  ]------------------------------------------------
void Land::updateCachedHeights(const MapManager* map,
                               const VisibleTilesRect::Area* area) {
  for (int row  = area->top;
           row <= area->bottom;
         ++row) {
    size_t cache_y_base = MAP_TO_RANGE(row, CACHED_HEIGHTS_HEIGHT) * CACHED_HEIGHTS_WIDTH;
    for (int col  = area->left;
             col <= area->right;
           ++col) {
      size_t cache_x = MAP_TO_RANGE(col, CACHED_HEIGHTS_WIDTH);
      cached_heights_[cache_y_base + cache_x] = map->terrainHeight(col, row);
    }
  }
}




//----[  writeVertices  ]------------------------------------------------------
void Land::writeVertices(const VisibleTilesRect::Area* area,
                         TerrainVertex* vertices) {
  int row_less_1 = area->top - 1, row_add_1;
  float row_bottom = 1.0f * row_less_1 ;
  for (int row  = area->top;
           row <= area->bottom;
           row = row_add_1) {
    row_add_1 = row + 1;
    size_t y_base = MAP_TO_RANGE(row, HEIGHT) * WIDTH;
    float row_top = row_bottom;
    row_bottom = 1.0f * row;
    float row_center = row_top + 0.5f;

    // Nearby terrain heights.  These are outside the column loop so that
    // we only have to get three new heights per column instead of 9.
    // 0 1 2
    // 3 4 5
    // 6 7 8
    int col = area->left;
    int col_less_1 = col - 1, col_add_1 = col + 1;
    float h[9] = {
      cachedHeight(col_less_1, row_less_1),
      cachedHeight(col,        row_less_1),
      cachedHeight(col_add_1,  row_less_1),
      cachedHeight(col_less_1, row),
      cachedHeight(col,        row),
      cachedHeight(col_add_1,  row),
      cachedHeight(col_less_1, row_add_1),
      cachedHeight(col,        row_add_1),
      cachedHeight(col_add_1,  row_add_1),
    };
    float col_right = 1.0f * col_less_1;

    do {
      size_t x = MAP_TO_RANGE(col, WIDTH);
      float col_left = col_right;
      float col_center = col_left + 0.5f;
      col_right = 1.0f * col;

#define AVERAGE4(a,b,c,d) (a+b+c+d) / 4.0f
//#define INTERPOLATE(a,b,c,d) AVERAGE4(a,b,c,d)
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define INTERPOLATE(a,b,c,d) MAX(MAX(a,b),MAX(c,d))

      TerrainVertex* quad_vertices = vertices + (y_base + x) * VERTICES_PER_QUAD;

      // this is necessary otherwise we read from write-only memory
      // when building the quad's center vertex height
      const float corner_heights[4] = {
        INTERPOLATE(h[0], h[1], h[3], h[4]),
        INTERPOLATE(h[1], h[2], h[4], h[5]),
        INTERPOLATE(h[3], h[4], h[6], h[7]),
        INTERPOLATE(h[4], h[5], h[7], h[8])
      };
      float center_height = AVERAGE4(corner_heights[0],
                                     corner_heights[1],
                                     corner_heights[2],
                                     corner_heights[3]);

      quad_vertices[0].x = col_left;
      quad_vertices[0].y = corner_heights[0];
      quad_vertices[0].z = row_top;
      quad_vertices[1].x = col_right;
      quad_vertices[1].y = corner_heights[1];
      quad_vertices[1].z = row_top;
      quad_vertices[3].x = col_left;
      quad_vertices[3].y = corner_heights[2];
      quad_vertices[3].z = row_bottom;
      quad_vertices[4].x = col_right;
      quad_vertices[4].y = corner_heights[3],
      quad_vertices[4].z = row_bottom;
      quad_vertices[2].x = col_center;
      quad_vertices[2].y = center_height;
      quad_vertices[2].z = row_center;

      writeQuadTextureCoordinates(quad_vertices);

      if (col < area->right) {
        // get the next set of column heights
        col = col_add_1;
        col_add_1++;
        h[0] = h[1]; h[1] = h[2]; h[2] = cachedHeight(col_add_1, row_less_1);
        h[3] = h[4]; h[4] = h[5]; h[5] = cachedHeight(col_add_1, row);
        h[6] = h[7]; h[7] = h[8]; h[8] = cachedHeight(col_add_1, row_add_1);
      } else {
        // finished iterating through the columns
        break;
      }
    } while (true);

    row_less_1 = row;
  }
}




//----[  writeQuadTextureCoordinates  ]----------------------------------------
void Land::writeQuadTextureCoordinates(TerrainVertex* quad_vertices) {
  quad_vertices[0].u = 0.0f;
  quad_vertices[0].v = 0.0f;
  quad_vertices[1].u = 1.0f;
  quad_vertices[1].v = 0.0f;
  //quad_vertices[2].u = 0.5f; // these are set when the buffer is created
  //quad_vertices[2].v = 0.5f;
  quad_vertices[3].u = 0.0f;
  quad_vertices[3].v = 1.0f;
  quad_vertices[4].u = 1.0f;
  quad_vertices[4].v = 1.0f;
}




//----[  cachedHeight  ]-------------------------------------------------------
float Land::cachedHeight(int x, int y) {
  size_t cache_y_base = MAP_TO_RANGE(y, CACHED_HEIGHTS_HEIGHT);
  size_t cache_x = MAP_TO_RANGE(x, CACHED_HEIGHTS_WIDTH);
  return cached_heights_[cache_y_base * CACHED_HEIGHTS_WIDTH + cache_x];
}



//----[  updateCachedTextures  ]-----------------------------------------------
void Land::updateCachedTextures(const MapManager* map, 
                                const VisibleTilesRect::Area* area) {
  TextureIndex* quad_textures = getCachedQuadTextures();
  for (int row  = area->top;
           row <= area->bottom;
         ++row) {
    size_t y_base = MAP_TO_RANGE(row, HEIGHT) * WIDTH;
    for (int col  = area->left;
             col <= area->right;
           ++col) {
      size_t x = MAP_TO_RANGE(col, WIDTH);
      quad_textures[y_base + x] = map->terrainTexture(col, row);
    }
  }
}



//----[  initializeVertexBuffer  ]---------------------------------------------
void Land::initializeVertexBuffer(TerrainVertex* vertices) const {
  for (int i = 0; i < VERTICES_PER_QUAD * VISIBLE_TILES; ++i, ++vertices) {
    vertices->nx = 0.0f;
    vertices->ny = 1.0f;
    vertices->nz = 0.0f;
    if ((i % VERTICES_PER_QUAD) == 2) {
      vertices->u = 0.5f;
      vertices->v = 0.5f;
    }
  }
}


}
}
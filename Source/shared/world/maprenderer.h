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
#ifndef __MAPRENDERER_H__
#define __MAPRENDERER_H__
#pragma once



#include <d3d9.h>
#include "mapgeometryvertex.h"
#include "mapgeometryindex.h"
#include "maptile_data.h"
#include "maptile_fluiddata.h"
#include "maptile_terraindata.h"
#include "maptile_walldata.h"
#include "maptilecorner_data.h"
#include "shared/texture/textureindex.h"
#include "mapvisibilityrect.h"
#include <set>
#include "shared/scenery/sceneryindex.h"
#include "locationvisualdata.h"

#if defined(DEBUG)||defined(_DEBUG)
#define DEBUG_EVIDYON_MAP_RENDERER
#endif

namespace Evidyon {
namespace World {


static const int VISIBLE_HORIZONTAL_EXTENT = 8;
static const int VISIBLE_VERTICAL_EXTENT = 8 + 3;
static const int VISIBLE_WIDTH = 2*VISIBLE_HORIZONTAL_EXTENT+1;
static const int VISIBLE_HEIGHT = 2*VISIBLE_VERTICAL_EXTENT+1;
static const int VISIBLE_TILES = VISIBLE_WIDTH * VISIBLE_HEIGHT;

static const float VISIBLE_RADIUS = VISIBLE_VERTICAL_EXTENT;
static const float VISIBLE_RADIUS_SQ = VISIBLE_RADIUS*VISIBLE_RADIUS;

static const int CACHED_HORIZONTAL_EXTENT
  = VISIBLE_HORIZONTAL_EXTENT + 1;
static const int CACHED_VERTICAL_EXTENT
  = VISIBLE_VERTICAL_EXTENT + 1;
static const int CACHED_WIDTH = 2*CACHED_HORIZONTAL_EXTENT + 1;
static const int CACHED_HEIGHT = 2*CACHED_VERTICAL_EXTENT + 1;
static const int CACHED_TILES = CACHED_WIDTH * CACHED_HEIGHT;

static const int CACHED_CORNERS_HORIZONTAL_EXTENT
  = CACHED_HORIZONTAL_EXTENT;
static const int CACHED_CORNERS_VERTICAL_EXTENT
  = CACHED_VERTICAL_EXTENT;
static const int CACHED_CORNERS_WIDTH = CACHED_WIDTH;
static const int CACHED_CORNERS_HEIGHT = CACHED_HEIGHT;
static const int CACHED_CORNERS = CACHED_TILES;

}
}


#include "maptiletyperenderingdata.h"

namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Scenery {
class SceneryRenderer;
struct SceneryInstance;
}
namespace SkinnedMesh {
class SkinnedMeshRenderer;
struct SkinnedMeshInstance;
}
namespace World {
class MapRendererDataSource;
}
}

namespace Evidyon {
namespace World {

static const int VERTICES_PER_QUAD = 5;
static const int TRIANGLES_PER_QUAD = 4;


//----[  MapRenderer  ]--------------------------------------------------------
class MapRenderer {
  static const int MAX_QUADS_PER_TILE = 6;
  static const int MAX_VISIBLE_QUADS = VISIBLE_TILES * MAX_QUADS_PER_TILE;

public:
  MapRenderer();

  // Initializes the structure
  bool create(LPDIRECT3DDEVICE9 d3d_device,
              Scenery::SceneryRenderer* scenery_renderer,
              SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer);

  // Frees resources used by this structure
  void destroy();

  // Erases all state-specific information from the renderer.  This should be
  // called before changing maps so that none of the old map's data is
  // accidentally preserved.
  void reset();

  // Recenters this renderer around the given coordinates.  The map data
  // provides information that the renderer pulls from.
  void update(MapRendererDataSource* map_data,
              int center_x,
              int center_y);

  //// Obtains the visibility of a location on the map.  Any coordinates are
  //// valid.  Values are valid only after update() has been called.
  //Visibility getVisibility(int x, int y);

  // Draws all map geometry with the given texture index using lighting.
  bool render(Texture::TextureIndex texture_index,
              EvidyonD3DRenderStateManager* renderstate_manager,
              float x,
              float y,
              float light_distance);


private:

  // Locks the dynamic geometry buffer so that data can be inserted into it.
  // If there isn't enough room in the buffer to draw the given number of
  // quads, this method will flush the buffer and lock the buffer with the
  // D3DLOCK_DISCARD flag.  Otherwise, it asks for D3DLOCK_NOOVERWRITE.  If
  // this isn't the first lock that was called since a flush(), the lock is
  // very efficient--this class maintains the state of the VB pointer.
  bool lock(unsigned int quads,
            MapGeometryVertex** vertices,
            EvidyonD3DRenderStateManager* renderstate_manager);

  // If this renderer had created geometry to be rendered with this texture,
  // flush() will unlock the buffer and issue a draw call for it.
  bool flush(EvidyonD3DRenderStateManager* renderstate_manager);

  // Resets all internal information without performing any checks.
  void zero();

  // Initializes the internal vertex buffer
  bool createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Initializes the internal index buffer
  bool createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Updates the tiles in the given region with data from the map data source.
  void updateAreaData(MapRendererDataSource* map_data,
                      const MapVisibilityRect::Area* area);

  // This method computes terrain heights.
  void computePreVisibility(const MapVisibilityRect::Area* area);

  // Based on the viewer's current position, this method determines which areas
  // of the map are visible and which aren't, then sets the map tiles'
  // attributes accordingly.
  void computeVisibility(int x, int y);

  // Determines of the viewer has line-of-sight to the given coordinate
  bool raytraceVisibility(int x0, int y0, int x1, int y1);

  // Called during computeVisibiity to propagate a VISIBILITY_NORMAL to all
  // areas that the player is able to see.  This causes all enclosed and blocked
  // areas on the map to remain invisible.
  void recursiveSetNormalVisibility(int x, int y, std::set<std::pair<int,int>>* visited);

  // After visibility has been computed, this method synchronizes the vertex
  // colors based on their visibility.
  void computePostVisibility(MapRendererDataSource* map_data);


private:

  // Device used to create the renderer
  LPDIRECT3DDEVICE9 d3d_device_;

  // Geometry buffers.  The index buffer is static and is filled with enough
  // indices to render a full VB of 5-vertex quads (4 corners + center vertex).
  // The vertex buffer is dynamic/write-only, and is filled with data as
  // needed during rendering.
  LPDIRECT3DVERTEXBUFFER9 vb_;
  LPDIRECT3DINDEXBUFFER9 ib_;

  MapGeometryVertex* locked_vertices_;
  bool next_vb_lock_discard_;
  UINT vb_lock_vertices_offset_;
  UINT vb_quads_remaining_;
  UINT vb_quads_consumed_;

  MapTileTypeRenderingData<MapTile_TerrainData> terrain_rendering_data_;
  MapTileTypeRenderingData<MapTile_FluidData> fluid_rendering_data_;
  MapTileTypeRenderingData<MapTile_WallData> wall_rendering_data_;

  MapVisibilityRect map_tiles_visibility_;

  // This cache holds all visible tiles and a ring of one tile around the
  // entire visible set.  This ring's visibility is always set to invisible;
  // however, its other information is correct so when it moves into the
  // visible set no new lookups need to be made.
  MapVisibilityRect cached_map_tiles_visibility_;
  MapTile_Data cached_map_tiles_[CACHED_TILES];

  // The corners are stored for each intersection of 4 quads.  Since
  // the quads wrap around the edges in the cache, there are exactly the same
  // number of corners as there are tiles.
  MapTileCorner_Data cached_corners_[CACHED_CORNERS];

  // These structures are used to draw objects on the map
  Scenery::SceneryRenderer* scenery_renderer_;
  Scenery::SceneryInstance* scenery_instances_[CACHED_TILES];

  // These structures draw skinned meshes on the map
  SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer_;
  SkinnedMesh::SkinnedMeshInstance* skinned_mesh_instances_[CACHED_TILES];
};

}
}


#endif
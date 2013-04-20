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
#ifndef __EVIDYONMAPRENDERER_H__
#define __EVIDYONMAPRENDERER_H__

#include <d3d9.h>
#include "evidyonmap.h"
#include "../shared/evidyontextureindextype.h"
#include "visibletilesrect.h"
#include <set>
#include "../shared/evidyonsceneryindextype.h"
#include "shared/world/locationvisualdata.h"

#if defined(DEBUG)||defined(_DEBUG)
#define DEBUG_EVIDYON_MAP_RENDERER
#endif


int EvidyonMapTile_TerrainData_Sorter(const void* p1, const void* p2);
int EvidyonMapTile_FluidData_Sorter(const void* p1, const void* p2);
int EvidyonMapTile_WallData_Sorter(const void* p1, const void* p2);


namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Scenery {
struct SceneryInstance;
class EvidyonSceneryRenderer;
}

namespace Map {

  
//----[  MapVertex  ]----------------------------------------------------------
struct MapVertex {
  float x, y, z;
  float nx, ny, nz;
  D3DCOLOR diffuse;
  float u, v;
};
#define D3DFVF_EVIDYON_MAPVERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1)




//----[  MapIndex  ]-----------------------------------------------------------
typedef unsigned short MapIndex;
#define D3DFMT_EVIDYON_MAPINDEX (D3DFMT_INDEX16)


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



static const int VERTICES_PER_QUAD = 5;
static const int TRIANGLES_PER_QUAD = 4;



//----[  TileVisibility  ]-----------------------------------------------------
enum TileVisibility {

  // This tile can't be seen at all because it's in a room behind a wall.  The
  // player should be completely unaware that this tile is even present.
  TILEVISIBILITY_INVISIBLE,

  // This tile was made invisible, but it should be rendered as black instead
  // of not rendered.  This is useful if terrain is occluding, for example.
  TILEVISIBILITY_HIDDEN,

  // This tile is behind something that occludes the direct view of the player.
  // These squares are colored slightly darker to show that there might be
  // something hiding there!
  TILEVISIBILITY_OCCLUDED,

  // This tile is completely visible.
  TILEVISIBILITY_NORMAL,
};



//----[  EvidyonMapTileCorner_Data  ]------------------------------------------
struct EvidyonMapTileCorner_Data {

  // This is computed from the surrounding tiles' terrain heights
  float terrain_height;

  // This color is based on the surrounding tiles' visibility
  D3DCOLOR vertex_color;

  // This color is based on both the surrounding tiles' visibility and the
  // height of the vertex.  This allows pits to be black.
  D3DCOLOR terrain_color;
};

  
//----[  EvidyonMapTile_Data  ]------------------------------------------------
struct EvidyonMapTile_Data {
  Evidyon::World::LocationVisualData visual_data;

  // ---------------------------------------------------------------
  // This data is recomputed by the renderer and is not valid when
  // this structure is passed to the map data source.

  // Wherever the tile is in the world
  float left, top;

  // Determined by the player's location
  TileVisibility visibility;

  //  0
  // 3 1
  //  2
  EvidyonMapTile_Data* neighbors[4];

  // 0 1
  // 2 3
  EvidyonMapTileCorner_Data* corners[4];

  // average of 4 corners
  float terrain_center_vertex_height;
  D3DCOLOR terrain_center_vertex_color;


  typedef int (*SorterType)(const void*, const void*);
};


//----[  EvidyonMapTile_TerrainData  ]-----------------------------------------
struct EvidyonMapTile_TerrainData {
  static const EvidyonMapTile_Data::SorterType Sorter;
  const EvidyonMapTile_Data* data;
  inline TextureIndex texture() { return data->visual_data.terrain_texture; }
};



//----[  EvidyonMapTile_FluidData  ]-------------------------------------------
struct EvidyonMapTile_FluidData {
  static const EvidyonMapTile_Data::SorterType Sorter;
  const EvidyonMapTile_Data* data;
  inline TextureIndex texture() { return data->visual_data.fluid_texture; }
};



//----[  EvidyonMapTile_WallData  ]--------------------------------------------
struct EvidyonMapTile_WallData {
  static const EvidyonMapTile_Data::SorterType Sorter;
  const EvidyonMapTile_Data* data;
  inline TextureIndex texture() { return data->visual_data.wall_texture; }
};



//----[  MapTileTypeRenderingData  ]-------------------------------------------
// This class is employed by the main map renderer to assist in rendering each
// unique type of element that can be present on a given map tile.  These
// elements are generated on-the-fly and change frequently with the map, so
// this class assists in optimizing them for rendering.
template <typename DataType> class MapTileTypeRenderingData {
public:

  //----[  MapTileTypeRenderingData  ]-----------------------------------------
  MapTileTypeRenderingData(const EvidyonMapTile_Data* data) {
    for (int i = 0; i < CACHED_TILES; ++i) {
      rendering_data_[i].data = &data[i];
    }
    reset();
  }



  //----[  getRenderingData  ]-------------------------------------------------
  // Obtains the internal rendering data buffer so that its pointers can be
  // properly initialized.  This method should be called during the constructor
  // of the owner class, since the actual pointer reference should never need
  // to be changed--just the data in the buffers should be modified.
  DataType* getRenderingData() {
    return rendering_data_;
  }

  //----[  sortAndReset  ]-----------------------------------------------------
  void sortAndReset() {

    // Sort the rendering data.  The method defined in the data type should
    // sort the pointers first by comparing visibility (putting invisible
    // entries at the end) then by ascending texture index.
    qsort(rendering_data_,
          CACHED_TILES,
          sizeof(DataType),
          DataType::Sorter);

    // Count the number of entries in the buffer that both have a valid texture
    // and are not invisible.
    DataType* iterator = rendering_data_;
    const DataType* end = iterator + CACHED_TILES;
    while (iterator != end &&
           iterator->texture() != INVALID_TEXTURE_INDEX &&
           iterator->data->visibility != TILEVISIBILITY_INVISIBLE) {
      ++iterator;
    }
    number_of_renderable_entries_ = iterator - rendering_data_;

    // Reset the rendering state
    reset();
  }

  //----[  reset  ]------------------------------------------------------------
  // Resets the rendering state to the start of the buffer
  void reset() {
    next_texture_index_ = number_of_renderable_entries_ > 0 ?
        rendering_data_[0].texture() : INVALID_TEXTURE_INDEX;
    next_renderable_entry_ = 0;
  }

  //----[  next  ]-------------------------------------------------------------
  // Gets the next entry that belongs to the texture index, or returns 'false'
  // if no more visible entries with that texture exist.  What to do with the
  // entry is entirely up to the owner class.
  bool next(TextureIndex texture_index, DataType** entry) {
    if (next_texture_index_ != texture_index) return false;
    size_t next_renderable_entry = next_renderable_entry_;
    *entry = &rendering_data_[next_renderable_entry];
    ++next_renderable_entry;
    next_texture_index_
      = next_renderable_entry < number_of_renderable_entries_ ?
          rendering_data_[next_renderable_entry].texture()
        :
          INVALID_TEXTURE_INDEX;
    next_renderable_entry_ = next_renderable_entry;
    return true;
  }

private:

  // Whatever kind of rendering data this class holds.  The pointers in the
  // elements here must be initialized manually by the owner class.
  DataType rendering_data_[CACHED_TILES];

  // Information used by next() during rendering.
  TextureIndex next_texture_index_;
  size_t next_renderable_entry_;

  // Stores the number of renderable entries so that entries that are marked
  // as invisible but have a valid texture entry are not rendered.
  size_t number_of_renderable_entries_;
};



//----[  EvidyonMapData  ]-----------------------------------------------------
class EvidyonMapData {
public:
  virtual void updateMapTileData(int x, int y, Evidyon::World::LocationVisualData* data) = 0;
};


//----[  EvidyonMapRenderer  ]-------------------------------------------------
class EvidyonMapRenderer {
  static const int MAX_QUADS_PER_TILE = 6;
  static const int MAX_VISIBLE_QUADS = VISIBLE_TILES * MAX_QUADS_PER_TILE;

public:
  EvidyonMapRenderer();

  // Initializes the structure
  bool create(LPDIRECT3DDEVICE9 d3d_device,
              Scenery::EvidyonSceneryRenderer* scenery_renderer);

  // Frees resources used by this structure
  void destroy();

  // Erases all state-specific information from the renderer.  This should be
  // called before changing maps so that none of the old map's data is
  // accidentally preserved.
  void reset();

  // Recenters this renderer around the given coordinates.  The map data
  // provides information that the renderer pulls from.
  void update(EvidyonMapData* map_data,
              int center_x,
              int center_y);

  // Draws all map geometry with the given texture index
  bool render(TextureIndex texture_index,
              EvidyonD3DRenderStateManager* renderstate_manager);


private:

  // Locks the dynamic geometry buffer so that data can be inserted into it.
  // If there isn't enough room in the buffer to draw the given number of
  // quads, this method will flush the buffer and lock the buffer with the
  // D3DLOCK_DISCARD flag.  Otherwise, it asks for D3DLOCK_NOOVERWRITE.  If
  // this isn't the first lock that was called since a flush(), the lock is
  // very efficient--this class maintains the state of the VB pointer.
  bool lock(unsigned int quads,
            MapVertex** vertices,
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
  void updateAreaData(EvidyonMapData* map_data,
                      const VisibleTilesRect::Area* area);

  // This method computes terrain heights.
  void computePreVisibility(const VisibleTilesRect::Area* area);

  // Based on the viewer's current position, this method determines which areas
  // of the map are visible and which aren't, then sets the map tiles'
  // attributes accordingly.
  void computeVisibility(int x, int y);

  // Determines of the viewer has line-of-sight to the given coordinate
  bool raytraceVisibility(int x0, int y0, int x1, int y1);

  // Called during computeVisibiity to propagate a TILEVISIBILITY_NORMAL to all
  // areas that the player is able to see.  This causes all enclosed and blocked
  // areas on the map to remain invisible.
  void recursiveSetNormalVisibility(int x, int y, std::set<std::pair<int,int>>* visited);

  // After visibility has been computed, this method synchronizes the vertex
  // colors based on their visibility.
  void computePostVisibility(EvidyonMapData* map_data);


private:

  // Device used to create the renderer
  LPDIRECT3DDEVICE9 d3d_device_;

  // Geometry buffers.  The index buffer is static and is filled with enough
  // indices to render a full VB of 5-vertex quads (4 corners + center vertex).
  // The vertex buffer is dynamic/write-only, and is filled with data as
  // needed during rendering.
  LPDIRECT3DVERTEXBUFFER9 vb_;
  LPDIRECT3DINDEXBUFFER9 ib_;

  MapVertex* locked_vertices_;
  bool next_vb_lock_discard_;
  UINT vb_lock_vertices_offset_;
  UINT vb_quads_remaining_;
  UINT vb_quads_consumed_;

  MapTileTypeRenderingData<EvidyonMapTile_TerrainData> terrain_rendering_data_;
  MapTileTypeRenderingData<EvidyonMapTile_FluidData> fluid_rendering_data_;
  MapTileTypeRenderingData<EvidyonMapTile_WallData> wall_rendering_data_;

  VisibleTilesRect map_tiles_visibility_;

  // This cache holds all visible tiles and a ring of one tile around the
  // entire visible set.  This ring's visibility is always set to invisible;
  // however, its other information is correct so when it moves into the
  // visible set no new lookups need to be made.
  VisibleTilesRect cached_map_tiles_visibility_;
  EvidyonMapTile_Data cached_map_tiles_[CACHED_TILES];

  // The corners are stored for each intersection of 4 quads.  Since
  // the quads wrap around the edges in the cache, there are exactly the same
  // number of corners as there are tiles.
  EvidyonMapTileCorner_Data cached_corners_[CACHED_CORNERS];

  // These structures are used to draw objects on the map
  Scenery::EvidyonSceneryRenderer* scenery_renderer_;
  Scenery::SceneryInstance* scenery_instances_[CACHED_TILES];
};

}
}

#endif

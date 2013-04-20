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
#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <d3d9.h>
#include "evidyonmap.h"
#include "visibletilesrect.h"
#include "../shared/evidyontextureindextype.h"


namespace Evidyon {
class EvidyonD3DRenderStateManager;


namespace Map {

class MapManager;




struct TerrainVertex {
  float x, y, z;
  float nx, ny, nz;
  D3DCOLOR diffuse;
  float u, v;
};

typedef unsigned short TerrainIndex;

#define D3DFVF_TERRAINVERTEX  (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFMT_TERRAININDEX (D3DFMT_INDEX16)






class TerrainQuadBuffer {
public:

  struct TextureIndexQuadPair {
    TextureIndex texture;
    int quad;
  };

  struct RenderingBatch {
    UINT minimum_vertex_index;
    UINT number_of_vertices;
    UINT start_index;
    UINT triangles;
    TextureIndex texture;
  };



public:
  TerrainQuadBuffer();

  // Initializes the structure
  bool create(LPDIRECT3DDEVICE9 d3d_device);

  // Frees resources used by this structure
  void destroy();

  // Draws the geometry to the screen.  The state manager is used to move the
  // texture into the device if any geometry is going to be rendered with it.
  bool render(LPDIRECT3DDEVICE9 d3d_device,
              TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);

protected:

  // Locks the vertex buffer so that the vertices can be overwritten.
  // This method will also write out the vertex colors, since changing
  // the vertices implies that the visible region has changed
  bool lockVB(int first_visible_column,
              int first_visible_row,
              const TileVisibility* visibility,
              TerrainVertex** vertices);

  // Unlocks the vertex buffer after vertices have been updated
  void unlockVB();

  // Locks the index buffer and updates both the triangles in the IB
  // and the render-batches based on the cached quad textures.
  void updateIB(int first_visible_column,
                int first_visible_row,
                const TileVisibility* visibility);

  // Allows editing of the cached quad textures
  TextureIndex* getCachedQuadTextures();


private:

  // Creates vertex colors for the map based on the visibility of
  // the tiles.
  void writeVertexColors(int first_visible_column,
                         int first_visible_row,
                         const TileVisibility* visibility,
                         TerrainVertex* vertices);

  // Initializes the internal vertex buffer
  bool createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Initializes the internal index buffer
  bool createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Rebuilds the triangle indices for the map based on the current set
  // of quad textures and the visibility map.  The visibility parameter should
  // be a WIDTH x HEIGHT array.
  void writeQuadIndices(const TextureIndexQuadPair sorted_pairs[VISIBLE_TILES],
                        size_t number_of_unique_textures,
                        TerrainIndex* indices);

  // Obtains a list of texture->quad index pairs sorted first by
  // texture index then by quad number in ascending order.  The
  // list is terminated when a quad-index is found that's less than zero.
  void getSortedQuadTextures(int first_visible_column,
                             int first_visible_row,
                             const TileVisibility* visibility,
                             TextureIndexQuadPair sorted_pairs[VISIBLE_TILES],
                             size_t* number_of_unique_textures);

  // Resets the number of rendering batches and ensures the batch array has
  // the requested capacity.
  void beginBuildingBatches(size_t number_of_batches);

  // Returns the next rendering batch and increments the number of batches.
  RenderingBatch* nextBatch();

private:

  // Allows the implementing class to do a one-time write of the entire vertex
  // buffer to set up its persistent state.
  virtual void initializeVertexBuffer(TerrainVertex* vertices) const = 0;

private:

  // Saved texture-indices for every tile currently in the rendering buffer.
  TextureIndex cached_quad_textures_[VISIBLE_TILES];

  // Geometry buffers
  LPDIRECT3DVERTEXBUFFER9 vb_;
  LPDIRECT3DINDEXBUFFER9 ib_;

  // Controls how textures are applied
  RenderingBatch* rendering_batches_;
  size_t number_of_rendering_batches_;
  size_t rendering_batches_capacity_;
  size_t next_batch_to_render_;
};



}
}



#endif
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
#include "terrain.h"
#include "map.h"
#include "../shared/evidyond3drenderstatemanager.h"


namespace Evidyon {
namespace Map {



//----[  TerrainQuadBuffer  ]--------------------------------------------------
TerrainQuadBuffer::TerrainQuadBuffer() {
  vb_ = NULL;
  ib_ = NULL;
  rendering_batches_ = NULL;
  number_of_rendering_batches_ = 0;
  rendering_batches_capacity_ = 0;
  next_batch_to_render_ = 0;
}




//----[  create  ]-------------------------------------------------------------
bool TerrainQuadBuffer::create(LPDIRECT3DDEVICE9 d3d_device) {
  if (!createIndexBuffer(d3d_device) ||
      !createVertexBuffer(d3d_device)) {
    destroy();
    return false;
  }

  return true;
}




//----[  destroy  ]------------------------------------------------------------
void TerrainQuadBuffer::destroy() {
  if (vb_ != NULL) vb_->Release();
  if (ib_ != NULL) ib_->Release();
  vb_ = NULL;
  ib_ = NULL;
}







//----[  render  ]-------------------------------------------------------------
bool TerrainQuadBuffer::render(LPDIRECT3DDEVICE9 d3d_device,
                               TextureIndex texture,
                               EvidyonD3DRenderStateManager* renderstate_manager) {
  if (!rendering_batches_) return true;

  if (next_batch_to_render_ >= number_of_rendering_batches_) {
    next_batch_to_render_ = 0;
  }

  int batch = next_batch_to_render_;
  TextureIndex next_batch_texture = rendering_batches_[batch].texture;
  if (texture != next_batch_texture) return true;
  next_batch_to_render_ = batch + 1;

  // Move this texture into the device
  renderstate_manager->setGeometryTexture(texture);

  // Initialize the device
  d3d_device->SetIndices(ib_);
  d3d_device->SetStreamSource(0, vb_, 0, sizeof(TerrainVertex));
  d3d_device->SetFVF(D3DFVF_TERRAINVERTEX);
  if (FAILED(d3d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                              0,
                                              rendering_batches_[batch].minimum_vertex_index,
                                              rendering_batches_[batch].number_of_vertices,
                                              rendering_batches_[batch].start_index,
                                              rendering_batches_[batch].triangles))) {
    return false;
  }

  return true;
}




//----[  lockVB  ]-------------------------------------------------------------
bool TerrainQuadBuffer::lockVB(int first_visible_column,
                               int first_visible_row,
                               const TileVisibility* visibility,
                               TerrainVertex** vertices) {
  TerrainVertex* internal_vertices;
  if (FAILED(vb_->Lock(0, 0, (LPVOID*)&internal_vertices, 0))) return false;
  writeVertexColors(first_visible_column,
                    first_visible_row,
                    visibility,
                    internal_vertices);
  *vertices = internal_vertices;
  return true;
}



//----[  unlockVB  ]-----------------------------------------------------------
void TerrainQuadBuffer::unlockVB() {
  vb_->Unlock();
}




//----[  updateIB  ]-----------------------------------------------------------
void TerrainQuadBuffer::updateIB(int first_visible_column,
                                 int first_visible_row,
                                 const TileVisibility* visibility) {
  TerrainIndex* indices = NULL;
  if (SUCCEEDED(ib_->Lock(0, 0, (LPVOID*)&indices, D3DLOCK_DISCARD))) {
    TextureIndexQuadPair sorted_pairs[VISIBLE_TILES];
    size_t number_of_unique_textures;
    getSortedQuadTextures(first_visible_column,
                          first_visible_row,
                          visibility,
                          sorted_pairs,
                          &number_of_unique_textures);
    writeQuadIndices(sorted_pairs,
                     number_of_unique_textures,
                     indices);
    ib_->Unlock();
  }
}



//----[  getCachedQuadTextures  ]----------------------------------------------
TextureIndex* TerrainQuadBuffer::getCachedQuadTextures() {
  return cached_quad_textures_;
}



//----[  writeVertexColors  ]--------------------------------------------------
void TerrainQuadBuffer::writeVertexColors(int first_visible_column,
                                          int first_visible_row,
                                          const TileVisibility* visibility,
                                          TerrainVertex* vertices) {
#define INVISIBLE_COLOR D3DCOLOR_ARGB(0,0,0,0)
#define OCCLUDED_COLOR  D3DCOLOR_XRGB(128,128,128)
#define NORMAL_COLOR    D3DCOLOR_XRGB(255,255,255)

  static const int VERTICES_WIDE = WIDTH + 1;
  static const int VERTICES_HIGH = HEIGHT + 1;
  static const int NUMBER_OF_VERTICES = VERTICES_WIDE * VERTICES_HIGH;

  D3DCOLOR vertex_colors[NUMBER_OF_VERTICES];
  D3DCOLOR* next_color = vertex_colors;
  for (int y = 0; y < VERTICES_HIGH; ++y) {
    int y_above = y-1;
    bool top_y_on_edge = y == 0;
    bool bottom_y_on_edge = y == HEIGHT;
    for (int x = 0; x < VERTICES_WIDE; ++x) {
      int x_left = x - 1;
      bool left_x_on_edge = x == 0;
      bool right_x_on_edge = x == WIDTH;

      // visibility of the 4 tiles around the vertex
      TileVisibility neighbor_visibility[4] = {
        (left_x_on_edge || top_y_on_edge) ? TILEVISIBILITY_INVISIBLE : visibility[y_above*WIDTH+x_left],
        (right_x_on_edge ||top_y_on_edge) ? TILEVISIBILITY_INVISIBLE : visibility[y_above*WIDTH+x],
        (left_x_on_edge||bottom_y_on_edge) ? TILEVISIBILITY_INVISIBLE : visibility[y*WIDTH+x_left],
        (right_x_on_edge||bottom_y_on_edge) ? TILEVISIBILITY_INVISIBLE : visibility[y*WIDTH+x],
      };

      // if any neighbors are invisible, this vertex should be invisible
      if (/*neighbor_visibility[0] == TILEVISIBILITY_INVISIBLE || 
          neighbor_visibility[1] == TILEVISIBILITY_INVISIBLE || 
          neighbor_visibility[2] == TILEVISIBILITY_INVISIBLE || 
          neighbor_visibility[3] == TILEVISIBILITY_INVISIBLE ||*/
          neighbor_visibility[0] == TILEVISIBILITY_OCCLUDED || 
          neighbor_visibility[1] == TILEVISIBILITY_OCCLUDED || 
          neighbor_visibility[2] == TILEVISIBILITY_OCCLUDED || 
          neighbor_visibility[3] == TILEVISIBILITY_OCCLUDED) {
        *(next_color++) = OCCLUDED_COLOR;
      } else {
        *(next_color++) = NORMAL_COLOR;
      }
    }
  }


  D3DCOLOR* corner_vertex_colors[4] = {
    vertex_colors + 0,
    vertex_colors + 1,
    vertex_colors + 0 + VERTICES_WIDE,
    vertex_colors + 1 + VERTICES_WIDE,
  };

  static const D3DCOLOR VISIBILITY_COLORS[] = {
    INVISIBLE_COLOR,
    OCCLUDED_COLOR,
    NORMAL_COLOR,
  };

  int mapped_top  = MAP_TO_RANGE(first_visible_row, HEIGHT);
  int mapped_left = MAP_TO_RANGE(first_visible_column, WIDTH);

  for (int y = 0; y < HEIGHT; ++y) {
    size_t vertex_y_base = ((y+mapped_top) % HEIGHT) * WIDTH * VERTICES_PER_QUAD;
    for (int x = 0; x < WIDTH; ++x) {
      size_t vertex_x = ((x + mapped_left) % WIDTH) * VERTICES_PER_QUAD;
      TerrainVertex* quad_vertices = vertices + (vertex_y_base + vertex_x);
      const D3DCOLOR corner_colors[] = {
        *(corner_vertex_colors[0]++),
        *(corner_vertex_colors[1]++),
        *(corner_vertex_colors[2]++),
        *(corner_vertex_colors[3]++),
      };
      D3DCOLOR center_color;
      if (corner_colors[0] == corner_colors[1] &&
          corner_colors[1] == corner_colors[2] &&
          corner_colors[2] == corner_colors[3]) {
        center_color = corner_colors[0];
      } else {
#define AVERAGEBYTE4(a,b,c,d)  (((a)&0xFF)+((b)&0xFF)+((c)&0xFF)+((d)&0xFF))>>2
        center_color =
          D3DCOLOR_ARGB(AVERAGEBYTE4(corner_colors[0]>>24, corner_colors[1]>>24, corner_colors[2]>>24, corner_colors[3]>>24),
                        AVERAGEBYTE4(corner_colors[0]>>16, corner_colors[1]>>16, corner_colors[2]>>16, corner_colors[3]>>16),
                        AVERAGEBYTE4(corner_colors[0]>>8, corner_colors[1]>>8, corner_colors[2]>>8, corner_colors[3]>>8),
                        AVERAGEBYTE4(corner_colors[0]>>0, corner_colors[1]>>0, corner_colors[2]>>0, corner_colors[3]>>0));
                      
      }
      quad_vertices[0].diffuse = corner_colors[0];
      quad_vertices[1].diffuse = corner_colors[1];
      quad_vertices[2].diffuse = center_color;//VISIBILITY_COLORS[*(visibility++)];
      quad_vertices[3].diffuse = corner_colors[2];
      quad_vertices[4].diffuse = corner_colors[3];
    }
    corner_vertex_colors[0]++;
    corner_vertex_colors[1]++;
    corner_vertex_colors[2]++;
    corner_vertex_colors[3]++;
  }
}


//----[  createVertexBuffer  ]-------------------------------------------------
bool TerrainQuadBuffer::createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateVertexBuffer(
      sizeof(TerrainVertex) * VERTICES_PER_QUAD * VISIBLE_TILES,
      D3DUSAGE_WRITEONLY,
      D3DFVF_TERRAINVERTEX,
      D3DPOOL_MANAGED,
      &vb_,
      NULL))) return false;

  TerrainVertex* vertices = NULL;
  if (SUCCEEDED(vb_->Lock(0, 0, (LPVOID*)&vertices, 0))) {
    initializeVertexBuffer(vertices);
    vb_->Unlock();
  }

  return true;
}



//----[  createIndexBuffer  ]--------------------------------------------------
bool TerrainQuadBuffer::createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateIndexBuffer(
        sizeof(TerrainIndex) * TRIANGLES_PER_QUAD * 3 * VISIBLE_TILES,
        D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
        D3DFMT_TERRAININDEX,
        D3DPOOL_DEFAULT,
        &ib_,
        NULL))) return false;

  //TerrainIndex* indices;
  //if (FAILED(ib_->Lock(0, 0, (LPVOID*)&indices, 0))) return false;

  //// 2 0 1
  //// 2 1 4
  //// 2 4 3
  //// 2 3 0
  //TerrainIndex base_index = 0;
  //for (int quad = 0; quad < WIDTH * HEIGHT; ++quad, base_index += VERTICES_PER_QUAD) {
  //  const TerrainIndex center_index = base_index + 2;
  //  const TerrainIndex offsets[] = { base_index + 1, base_index + 4, base_index + 3 };
  //  *(indices++) = center_index; *(indices++) = base_index; *(indices++) = offsets[0];
  //  *(indices++) = center_index; *(indices++) = offsets[0]; *(indices++) = offsets[1];
  //  *(indices++) = center_index; *(indices++) = offsets[1]; *(indices++) = offsets[2];
  //  *(indices++) = center_index; *(indices++) = offsets[2]; *(indices++) = base_index;
  //}

  //ib_->Unlock();

  return true;
}






//----[  writeQuadIndices  ]---------------------------------------------------
void TerrainQuadBuffer::writeQuadIndices(const TextureIndexQuadPair sorted_pairs[VISIBLE_TILES],
                                         size_t number_of_unique_textures,
                                         TerrainIndex* indices) {
  TextureIndex current_texture = INVALID_TEXTURE_INDEX;

  beginBuildingBatches(number_of_unique_textures);

  RenderingBatch* current_batch = NULL;
  int first_tile_in_batch = 0;
  int tile, quad_index;
  for (tile = 0; (tile < VISIBLE_TILES) &&
                 (quad_index = sorted_pairs[tile].quad) >= 0; ++tile) {
    TextureIndex texture = sorted_pairs[tile].texture;
    TerrainIndex base_index = quad_index * VERTICES_PER_QUAD;

    // Create a new batch if the texture has changed
    if (texture != current_texture) {
      current_texture = texture;

      // for the first iteration, this code acts on 'dummy_batch'
      int quads_in_batch = tile - first_tile_in_batch;
      if (quads_in_batch > 0) {
        current_batch->number_of_vertices =
          (sorted_pairs[tile - 1].quad * VERTICES_PER_QUAD + VERTICES_PER_QUAD) - current_batch->minimum_vertex_index;
        current_batch->triangles = quads_in_batch * TRIANGLES_PER_QUAD;
      }

      current_batch = nextBatch();
      current_batch->minimum_vertex_index = base_index;
      current_batch->start_index = tile * TRIANGLES_PER_QUAD * 3;
      current_batch->texture = texture;
      first_tile_in_batch = tile;
    }

    // 2 0 1
    // 2 1 4
    // 2 4 3
    // 2 3 0
    const TerrainIndex center_index = base_index + 2;
    const TerrainIndex offsets[] = { base_index + 1, base_index + 4, base_index + 3 };
    *(indices++) = center_index; *(indices++) = base_index; *(indices++) = offsets[0];
    *(indices++) = center_index; *(indices++) = offsets[0]; *(indices++) = offsets[1];
    *(indices++) = center_index; *(indices++) = offsets[1]; *(indices++) = offsets[2];
    *(indices++) = center_index; *(indices++) = offsets[2]; *(indices++) = base_index;
  }

  // Push the last batch
  UINT quads_in_batch = tile - first_tile_in_batch;
  if (quads_in_batch > 0) {
    current_batch->number_of_vertices =
      (sorted_pairs[tile-1].quad * VERTICES_PER_QUAD + VERTICES_PER_QUAD) - current_batch->minimum_vertex_index;
    current_batch->triangles = (tile - first_tile_in_batch) * TRIANGLES_PER_QUAD;
  } else {
    // The last batch is invalid, so prevent it from being rendered
    --number_of_rendering_batches_;
  }
}





//----[  CompareTextureIndexQuadPair  ]----------------------------------------
int CompareTextureIndexQuadPair(const void* p1, const void* p2) {
  const TerrainQuadBuffer::TextureIndexQuadPair* a = (const TerrainQuadBuffer::TextureIndexQuadPair*)p1;
  const TerrainQuadBuffer::TextureIndexQuadPair* b = (const TerrainQuadBuffer::TextureIndexQuadPair*)p2;
  if (a->quad < 0) return +1; // invalid (invisible) quads to end
  if (b->quad < 0) return -1;
  if (a->texture > b->texture) return +1; // sort by ascending texture
  if (a->texture < b->texture) return -1;
  return a->quad - b->quad; // sort by ascending quad
}


//----[  getSortedQuadTextures  ]----------------------------------------------
void TerrainQuadBuffer::getSortedQuadTextures(int first_visible_column,
                                              int first_visible_row,
                                              const TileVisibility* visibility,
                                              TextureIndexQuadPair sorted_pairs[VISIBLE_TILES],
                                              size_t* number_of_unique_textures) {

  int mapped_top  = MAP_TO_RANGE(first_visible_row, HEIGHT);
  int mapped_left = MAP_TO_RANGE(first_visible_column, WIDTH);

  const TileVisibility* next_visibility = visibility;
  TextureIndexQuadPair* next_pair = sorted_pairs;
  for (int y = 0; y < HEIGHT; ++y) {
    int y_base = ((y+mapped_top) % HEIGHT) * WIDTH;
    for (int x = 0; x < WIDTH; ++x) {
      int quad_index = y_base + ((x + mapped_left) % WIDTH);
      TextureIndex texture = cached_quad_textures_[quad_index];
      next_pair->quad =
        ((texture == INVALID_TEXTURE_INDEX) ||
         ((*(next_visibility)) == TILEVISIBILITY_INVISIBLE)) ? -1 : quad_index;
      next_pair->texture = texture;
      ++next_visibility;
      ++next_pair;
    }
  }

  qsort(sorted_pairs,
        VISIBLE_TILES,
        sizeof(TextureIndexQuadPair),
        CompareTextureIndexQuadPair);

  // count the unique textures so that we can build batches
  TextureIndex last_texture_index = sorted_pairs[0].texture;
  size_t unique_texture_count = 1;
  for (int i = 0; i < VISIBLE_TILES; ++i) {
    TextureIndex this_texture_index = sorted_pairs[i].texture;
    if (last_texture_index != this_texture_index) {
      last_texture_index = this_texture_index;
      ++unique_texture_count;
    }
  }
  *number_of_unique_textures = unique_texture_count;
}




//----[  beginBuildingBatches  ]-----------------------------------------------
void TerrainQuadBuffer::beginBuildingBatches(size_t number_of_batches) {
  number_of_rendering_batches_ = 0;
  if (!rendering_batches_ || rendering_batches_capacity_ < number_of_batches) {
    delete[] rendering_batches_;
    rendering_batches_ = new RenderingBatch[number_of_batches];
    rendering_batches_capacity_ = number_of_batches;
  }
}


//----[  nextBatch  ]----------------------------------------------------------
TerrainQuadBuffer::RenderingBatch* TerrainQuadBuffer::nextBatch() {
  return &rendering_batches_[number_of_rendering_batches_++];
}













}
}
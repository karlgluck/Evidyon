//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#include "evidyonmaprenderer.h"
#include "../common/safemacros.h"
#include <assert.h>
#include "../shared/evidyond3drenderstatemanager.h"

#define COLOR_A(c) (0xFF&((c)>>24))
#define COLOR_R(c) (0xFF&((c)>>16))
#define COLOR_G(c) (0xFF&((c)>>8))
#define COLOR_B(c) (0xFF&((c)>>0))
#define INTERPOLATE_COLOR2(a,b) \
  D3DCOLOR_XRGB((COLOR_R(a)+COLOR_R(b))>>1,\
                (COLOR_G(a)+COLOR_G(b))>>1,\
                (COLOR_B(a)+COLOR_B(b))>>1)
#define INTERPOLATE_COLOR(a,b,c,d) \
  D3DCOLOR_XRGB((COLOR_R(a)+COLOR_R(b)+COLOR_R(c)+COLOR_R(d))>>2,\
                (COLOR_G(a)+COLOR_G(b)+COLOR_G(c)+COLOR_G(d))>>2,\
                (COLOR_B(a)+COLOR_B(b)+COLOR_B(c)+COLOR_B(d))>>2)


static const float FLUID_HEIGHT = -0.25f;


//----[  EvidyonMapTile_TerrainData_Sorter  ]----------------------------------
int EvidyonMapTile_TerrainData_Sorter(const void* p1, const void* p2) {
  using namespace Evidyon::Map;
  const EvidyonMapTile_TerrainData* a = (const EvidyonMapTile_TerrainData*)p1;
  const EvidyonMapTile_Data* a_data = a->data;
  const EvidyonMapTile_TerrainData* b = (const EvidyonMapTile_TerrainData*)p2;
  const EvidyonMapTile_Data* b_data = b->data;
  if (a_data->visibility == TILEVISIBILITY_INVISIBLE) return +1; // invalid (invisible) quads to end
  if (b_data->visibility == TILEVISIBILITY_INVISIBLE) return -1; // invalid (invisible) quads to end
  Evidyon::TextureIndex a_texture = a_data->terrain_texture;
  Evidyon::TextureIndex b_texture = b_data->terrain_texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  return 0; // return a->texture_rotation - b->texture_rotation;
}


//----[  EvidyonMapTile_FluidData_Sorter  ]------------------------------------
int EvidyonMapTile_FluidData_Sorter(const void* p1, const void* p2) {
  using namespace Evidyon::Map;
  const EvidyonMapTile_FluidData* a = (const EvidyonMapTile_FluidData*)p1;
  const EvidyonMapTile_Data* a_data = a->data;
  const EvidyonMapTile_FluidData* b = (const EvidyonMapTile_FluidData*)p2;
  const EvidyonMapTile_Data* b_data = b->data;
  if (a_data->visibility == TILEVISIBILITY_INVISIBLE) return +1; // invalid (invisible) quads to end
  if (b_data->visibility == TILEVISIBILITY_INVISIBLE) return -1; // invalid (invisible) quads to end
  Evidyon::TextureIndex a_texture = a_data->fluid_texture;
  Evidyon::TextureIndex b_texture = b_data->fluid_texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  return 0;
}




//----[  EvidyonMapTile_WallData_Sorter  ]-------------------------------------
int EvidyonMapTile_WallData_Sorter(const void* p1, const void* p2){
  using namespace Evidyon::Map;
  const EvidyonMapTile_WallData* a = (const EvidyonMapTile_WallData*)p1;
  const EvidyonMapTile_Data* a_data = a->data;
  const EvidyonMapTile_WallData* b = (const EvidyonMapTile_WallData*)p2;
  const EvidyonMapTile_Data* b_data = b->data;
  if (a_data->visibility == TILEVISIBILITY_INVISIBLE) return +1; // invalid (invisible) quads to end
  if (b_data->visibility == TILEVISIBILITY_INVISIBLE) return -1; // invalid (invisible) quads to end
  Evidyon::TextureIndex a_texture = a_data->wall_texture;
  Evidyon::TextureIndex b_texture = b_data->wall_texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  return 0;
}


namespace Evidyon {
namespace Map {


const EvidyonMapTile_Data::SorterType EvidyonMapTile_TerrainData::Sorter
  = EvidyonMapTile_TerrainData_Sorter;
const EvidyonMapTile_Data::SorterType EvidyonMapTile_FluidData::Sorter
  = EvidyonMapTile_FluidData_Sorter;
const EvidyonMapTile_Data::SorterType EvidyonMapTile_WallData::Sorter
  = EvidyonMapTile_WallData_Sorter;

  
//----[  EvidyonMapRenderer  ]-------------------------------------------------
EvidyonMapRenderer::EvidyonMapRenderer() :
    terrain_rendering_data_(cached_map_tiles_),
    fluid_rendering_data_(cached_map_tiles_),
    wall_rendering_data_(cached_map_tiles_) {
  memset(cached_map_tiles_, 0, sizeof(cached_map_tiles_));
  zero();
  d3d_device_ = NULL;
  vb_ = NULL;
  ib_ = NULL;
}




//----[  create  ]-------------------------------------------------------------
bool EvidyonMapRenderer::create(LPDIRECT3DDEVICE9 d3d_device) {
  if (!d3d_device || vb_ || ib_) return false;
  if (!createVertexBuffer(d3d_device) || !createIndexBuffer(d3d_device)) {
    destroy();
    return false;
  }
  (d3d_device_ = d3d_device)->AddRef();
  reset();
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void EvidyonMapRenderer::destroy() {
  SAFE_RELEASE(vb_);
  SAFE_RELEASE(ib_);
  SAFE_RELEASE(d3d_device_);
}



//----[  reset  ]--------------------------------------------------------------
void EvidyonMapRenderer::reset() {
  cached_map_tiles_visibility_.center(-MAXIMUM_DIMENSION,
                                      -MAXIMUM_DIMENSION,
                                      CACHED_HORIZONTAL_EXTENT,
                                      CACHED_VERTICAL_EXTENT);
  map_tiles_visibility_.center(-MAXIMUM_DIMENSION,
                               -MAXIMUM_DIMENSION,
                               VISIBLE_HORIZONTAL_EXTENT,
                               VISIBLE_VERTICAL_EXTENT);
}



//----[  update  ]-------------------------------------------------------------
void EvidyonMapRenderer::update(
    EvidyonMapData* map_data,
    int center_x,
    int center_y) {

  /*cached_map_tiles_visibility_.center(-MAXIMUM_DIMENSION,
                                      -MAXIMUM_DIMENSION,
                                      CACHED_HORIZONTAL_EXTENT,
                                      CACHED_VERTICAL_EXTENT);*/
  VisibleTilesRect::Area newly_visible[2];
  cached_map_tiles_visibility_.moveTo(
    center_x,
    center_y,
    CACHED_HORIZONTAL_EXTENT,
    CACHED_VERTICAL_EXTENT,
    &newly_visible[0], &newly_visible[1]);

  bool changed[2] = { false, false };

  if (newly_visible[0].valid()) {
    updateAreaData(map_data, &newly_visible[0]);
    changed[0] = true;
  }
  if (newly_visible[1].valid()) {
    updateAreaData(map_data, &newly_visible[1]);
    changed[1] = true;
  }

  if (changed[0] || changed[1]) {

    // The visibility region that we actually need to rewrite the vertices for
    // is contained within the cached region.  This check is necessary because
    // the border tiles are not completely valid--their interpolated positions
    // contain values that are left over from wrapping on the other side of
    // the map.
    map_tiles_visibility_.moveTo(
      center_x,
      center_y,
      VISIBLE_HORIZONTAL_EXTENT,
      VISIBLE_VERTICAL_EXTENT,
      &newly_visible[0], &newly_visible[1]);

    if (changed[0]) computePreVisibility(&newly_visible[0]);
    if (changed[1]) computePreVisibility(&newly_visible[1]);
    computeVisibility(center_x, center_y);
    computePostVisibility(map_data);
    terrain_rendering_data_.sortAndReset();
    fluid_rendering_data_.sortAndReset();
    wall_rendering_data_.sortAndReset();
  } else {
    terrain_rendering_data_.reset();
    fluid_rendering_data_.reset();
    wall_rendering_data_.reset();
  }
}





//----[  render  ]-------------------------------------------------------------
bool EvidyonMapRenderer::render(
    TextureIndex texture_index,
    EvidyonD3DRenderStateManager* renderstate_manager) {

  {
    EvidyonMapTile_TerrainData* terrain_data;
    while (terrain_rendering_data_.next(texture_index, &terrain_data)) {
      renderstate_manager->setGeometryTexture(texture_index);
      const EvidyonMapTile_Data* data = terrain_data->data;

      MapVertex* vertices;
      if (!lock(1, &vertices, renderstate_manager)) return false;

      vertices[0].nx = 0.0f; vertices[0].ny = 1.0f; vertices[0].nz = 0.0f;
      vertices[1].nx = 0.0f; vertices[1].ny = 1.0f; vertices[1].nz = 0.0f;
      vertices[2].nx = 0.0f; vertices[2].ny = 1.0f; vertices[2].nz = 0.0f;
      vertices[3].nx = 0.0f; vertices[3].ny = 1.0f; vertices[3].nz = 0.0f;
      vertices[4].nx = 0.0f; vertices[4].ny = 1.0f; vertices[4].nz = 0.0f;

      float corner_heights[4] = {
        data->corners[0]->terrain_height,
        data->corners[1]->terrain_height,
        data->corners[2]->terrain_height,
        data->corners[3]->terrain_height,
      };
      //float center_height = corner_heights[0] + corner_heights[1] + corner_heights[2] + corner_heights[3];//data->terrain_center_vertex_height;
      //center_height /= 4;
      float center_height = data->terrain_center_vertex_height;

      float col_left = data->left;
      float col_center = data->left + 0.5f;
      float col_right = col_left + 1.0f;
      float row_top  = data->top;
      float row_center = data->top + 0.5f;
      float row_bottom = row_top + 1.0f;

      vertices[0].x = col_left;
      vertices[0].y = corner_heights[0];
      vertices[0].z = row_top;
      vertices[1].x = col_right;
      vertices[1].y = corner_heights[1];
      vertices[1].z = row_top;
      vertices[3].x = col_left;
      vertices[3].y = corner_heights[2];
      vertices[3].z = row_bottom;
      vertices[4].x = col_right;
      vertices[4].y = corner_heights[3],
      vertices[4].z = row_bottom;
      vertices[2].x = col_center;
      vertices[2].y = center_height;
      vertices[2].z = row_center;

      vertices[0].diffuse = data->corners[0]->terrain_color;
      vertices[1].diffuse = data->corners[1]->terrain_color;
      vertices[2].diffuse = data->terrain_center_vertex_color;
      vertices[3].diffuse = data->corners[2]->terrain_color;
      vertices[4].diffuse = data->corners[3]->terrain_color;

      vertices[2].u = 0.5f; vertices[2].v = 0.5f;
      switch (data->terrain_texture_rotation) {
        case 0:
          vertices[0].u = 0.0f; vertices[0].v = 0.0f;
          vertices[1].u = 1.0f; vertices[1].v = 0.0f;
          vertices[4].u = 1.0f; vertices[4].v = 1.0f;
          vertices[3].u = 0.0f; vertices[3].v = 1.0f;
          break;
        case 1:
          vertices[1].u = 0.0f; vertices[1].v = 0.0f;
          vertices[4].u = 1.0f; vertices[4].v = 0.0f;
          vertices[3].u = 1.0f; vertices[3].v = 1.0f;
          vertices[0].u = 0.0f; vertices[0].v = 1.0f;
          break;
        case 2:
          vertices[4].u = 0.0f; vertices[4].v = 0.0f;
          vertices[3].u = 1.0f; vertices[3].v = 0.0f;
          vertices[0].u = 1.0f; vertices[0].v = 1.0f;
          vertices[1].u = 0.0f; vertices[1].v = 1.0f;
          break;
        case 3:
          vertices[3].u = 0.0f; vertices[3].v = 0.0f;
          vertices[0].u = 1.0f; vertices[0].v = 0.0f;
          vertices[1].u = 1.0f; vertices[1].v = 1.0f;
          vertices[4].u = 0.0f; vertices[4].v = 1.0f;
          break;
      }
    }
  }
  
  {
    EvidyonMapTile_FluidData* fluid_data;
    while (fluid_rendering_data_.next(texture_index, &fluid_data)) {
      renderstate_manager->setGeometryTexture(texture_index);
      const EvidyonMapTile_Data* data = fluid_data->data;

      MapVertex* vertices;
      if (!lock(1, &vertices, renderstate_manager)) return false;

      vertices[0].nx = 0.0f; vertices[0].ny = 1.0f; vertices[0].nz = 0.0f;
      vertices[1].nx = 0.0f; vertices[1].ny = 1.0f; vertices[1].nz = 0.0f;
      vertices[2].nx = 0.0f; vertices[2].ny = 1.0f; vertices[2].nz = 0.0f;
      vertices[3].nx = 0.0f; vertices[3].ny = 1.0f; vertices[3].nz = 0.0f;
      vertices[4].nx = 0.0f; vertices[4].ny = 1.0f; vertices[4].nz = 0.0f;

      float col_left = data->left;
      float col_center = data->left + 0.5f;
      float col_right = col_left + 1.0f;
      float row_top  = data->top;
      float row_center = data->top + 0.5f;
      float row_bottom = row_top + 1.0f;

      vertices[0].x = col_left;
      vertices[0].y = FLUID_HEIGHT;
      vertices[0].z = row_top;
      vertices[1].x = col_right;
      vertices[1].y = FLUID_HEIGHT;
      vertices[1].z = row_top;
      vertices[3].x = col_left;
      vertices[3].y = FLUID_HEIGHT;
      vertices[3].z = row_bottom;
      vertices[4].x = col_right;
      vertices[4].y = FLUID_HEIGHT,
      vertices[4].z = row_bottom;
      vertices[2].x = col_center;
      vertices[2].y = FLUID_HEIGHT;
      vertices[2].z = row_center;

      vertices[0].diffuse = data->corners[0]->vertex_color;
      vertices[1].diffuse = data->corners[1]->vertex_color;
      vertices[2].diffuse =
        INTERPOLATE_COLOR(data->corners[0]->vertex_color,
                          data->corners[1]->vertex_color,
                          data->corners[2]->vertex_color,
                          data->corners[3]->vertex_color);
      vertices[3].diffuse = data->corners[2]->vertex_color;
      vertices[4].diffuse = data->corners[3]->vertex_color;

      vertices[0].u = 0.0f; vertices[0].v = 0.0f;
      vertices[1].u = 1.0f; vertices[1].v = 0.0f;
      vertices[4].u = 1.0f; vertices[4].v = 1.0f;
      vertices[3].u = 0.0f; vertices[3].v = 1.0f;
      vertices[2].u = 0.5f; vertices[2].v = 0.5f;
    }
  }
  {
    EvidyonMapTile_WallData* wall_data;
    while (wall_rendering_data_.next(texture_index, &wall_data)) {
      renderstate_manager->setGeometryTexture(texture_index);
      const EvidyonMapTile_Data* data = wall_data->data;


      MapVertex* vertices;
      if (!lock(5, &vertices, renderstate_manager)) return false;

      for (int quad = 0; quad < 5; ++quad) {

        vertices[0].nx = 0.0f; vertices[0].ny = 1.0f; vertices[0].nz = 0.0f;
        vertices[1].nx = 0.0f; vertices[1].ny = 1.0f; vertices[1].nz = 0.0f;
        vertices[2].nx = 0.0f; vertices[2].ny = 1.0f; vertices[2].nz = 0.0f;
        vertices[3].nx = 0.0f; vertices[3].ny = 1.0f; vertices[3].nz = 0.0f;
        vertices[4].nx = 0.0f; vertices[4].ny = 1.0f; vertices[4].nz = 0.0f;

        float col_left = data->left;
        float col_center = data->left + 0.5f;
        float col_right = col_left + 1.0f;
        float row_top  = data->top;
        float row_center = data->top + 0.5f;
        float row_bottom = row_top + 1.0f;

        float height, ground = 0.0f;
        switch (data->wall_type) {
          case WALLTYPE_FLOOR:  ground = -3.0f; height = 0.0f; break;
          case WALLTYPE_HIGH:   height = 2.0f; break;
          case WALLTYPE_LOW:    height = .75f; break;
          case WALLTYPE_MEDIUM: height = 1.25f; break;
        }

#define H(v) { vertices[v].y = height;  }
#define Hf(v) { vertices[v].y = (ground+height)/2.0f;  if (data->wall_type==WALLTYPE_FLOOR) vertices[v].diffuse = D3DCOLOR_XRGB(0,0,0); }
#define Lo(v) { vertices[v].y = ground; if (data->wall_type==WALLTYPE_FLOOR) vertices[v].diffuse = D3DCOLOR_XRGB(0,0,0); }
#define TL(v) { vertices[v].x = col_left; vertices[v].z=row_top; vertices[v].diffuse = data->corners[0]->vertex_color; }
#define TR(v) { vertices[v].x = col_right; vertices[v].z=row_top; vertices[v].diffuse = data->corners[1]->vertex_color; }
#define BL(v) { vertices[v].x = col_left; vertices[v].z=row_bottom; vertices[v].diffuse = data->corners[2]->vertex_color; }
#define BR(v) { vertices[v].x = col_right; vertices[v].z=row_bottom; vertices[v].diffuse = data->corners[3]->vertex_color; }
#define C(v) { vertices[v].x = col_center; vertices[v].z=row_center; vertices[v].diffuse = data->terrain_center_vertex_color; }
#define T(v) { vertices[v].x = col_center; vertices[v].z=row_top; vertices[v].diffuse = INTERPOLATE_COLOR2(data->corners[0]->vertex_color,data->corners[1]->vertex_color); }
#define R(v) { vertices[v].x = col_right; vertices[v].z=row_center; vertices[v].diffuse = INTERPOLATE_COLOR2(data->corners[1]->vertex_color,data->corners[3]->vertex_color); }
#define B(v) { vertices[v].x = col_center; vertices[v].z=row_bottom; vertices[v].diffuse = INTERPOLATE_COLOR2(data->corners[2]->vertex_color,data->corners[3]->vertex_color); }
#define L(v) { vertices[v].x = col_left; vertices[v].z=row_center; vertices[v].diffuse = INTERPOLATE_COLOR2(data->corners[0]->vertex_color,data->corners[2]->vertex_color); }

        switch (quad) {
        case 0:
          TL(0) TR(1) BL(3) C(2) BR(4)
          H(0) H(1) H(2) H(3) H(4)
          break;
        case 1:
          TL(0) TR(1) T(2) TL(3) TL(4)
          Lo(0) Lo(1) Hf(2) H(3) H(4)
          break;
        case 2:
          TR(0) BR(1) R(2) TR(3) BR(4)
          Lo(0) Lo(1) Hf(2) H(3) H(4)
          break;
        case 3:
          BR(0) BL(1) B(2) BR(3) BL(4)
          Lo(0) Lo(1) Hf(2) H(3) H(4)
          break;
        case 4:
          BL(0) TL(1) L(2) BL(3) TL(4)
          Lo(0) Lo(1) Hf(2) H(3) H(4)
          break;
        }


        vertices[0].u = 0.0f; vertices[0].v = 0.0f;
        vertices[1].u = 1.0f; vertices[1].v = 0.0f;
        vertices[2].u = 0.5f; vertices[2].v = 0.5f;
        vertices[3].u = 0.0f; vertices[3].v = 1.0f;
        vertices[4].u = 1.0f; vertices[4].v = 1.0f;

        vertices += VERTICES_PER_QUAD;
      }
    }
  }

  // Draw any geometry that hasn't been flushed yet
  return flush(renderstate_manager);
}










//----[  lock  ]---------------------------------------------------------------
bool EvidyonMapRenderer::lock(
    unsigned int quads,
    MapVertex** vertices,
    EvidyonD3DRenderStateManager* renderstate_manager) {

  assert(quads < MAX_VISIBLE_QUADS);
  assert(vertices);

  // Discard the whole buffer if the flag is set
  if (next_vb_lock_discard_) {
    assert(!locked_vertices_);
    HRESULT hr;
    hr = vb_->Lock(0,
                   0,
                   (void**)(&locked_vertices_),
                   D3DLOCK_DISCARD);
    if (FAILED(hr)) return false;
    vb_lock_vertices_offset_ = 0;
    next_vb_lock_discard_ = false;
    vb_quads_remaining_ = MAX_VISIBLE_QUADS;
    vb_quads_consumed_ = 0;
  }

  // Do we have room to lock?  If so, return a pointer.
  if (quads <= vb_quads_remaining_) {
    if (!locked_vertices_) {
      HRESULT hr;
      hr = vb_->Lock(vb_lock_vertices_offset_ * sizeof(MapVertex),
                     vb_quads_remaining_ * VERTICES_PER_QUAD * sizeof(MapVertex),
                     (void**)(&locked_vertices_),
                     D3DLOCK_NOOVERWRITE);
      if (FAILED(hr)) return false;                
    }
    *vertices = locked_vertices_;
    vb_quads_remaining_ -= quads;
    vb_quads_consumed_ += quads;
    locked_vertices_ += quads * VERTICES_PER_QUAD;
    return true;
  } else {
    if (!flush(renderstate_manager)) return false;
    next_vb_lock_discard_ = true;
    return lock(quads, vertices, renderstate_manager);
  }
}






//----[  flush  ]--------------------------------------------------------------
bool EvidyonMapRenderer::flush(
    EvidyonD3DRenderStateManager* renderstate_manager) {

  // there is only stuff to render if the buffer was locked
  if (!locked_vertices_) return true;
  locked_vertices_ = NULL;
  vb_->Unlock();

  // Set the geometry buffers into the device
  if (!renderstate_manager->setStream0GeometryBuffers(
        vb_,
        D3DFVF_EVIDYON_MAPVERTEX,
        sizeof(MapVertex),
        ib_)) {
    return false;
  }

  // Draw triangles
  HRESULT hr;
  UINT vertices_consumed = vb_quads_consumed_ * VERTICES_PER_QUAD;
  hr = d3d_device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                         vb_lock_vertices_offset_,
                                         0,
                                         vertices_consumed,
                                         0,
                                         vb_quads_consumed_*TRIANGLES_PER_QUAD);

  // Update the state
  if (SUCCEEDED(hr)) {
    vb_lock_vertices_offset_ += vertices_consumed;
    vb_quads_consumed_ = 0;
    return true;
  } else {
    next_vb_lock_discard_ = true;
    return false;
  }
}



//----[  zero  ]---------------------------------------------------------------
void EvidyonMapRenderer::zero() {
  d3d_device_ = NULL;
  vb_ = NULL;
  ib_ = NULL;

  locked_vertices_ = NULL;
  next_vb_lock_discard_ = true;
  vb_lock_vertices_offset_ = 0;
  vb_quads_remaining_ = 0;
  vb_quads_consumed_ = 0;

  // Create neighbor references
  for (int y = 0; y < CACHED_HEIGHT; ++y) {
    int y_above = y == 0 ? CACHED_HEIGHT - 1 : y-1;
    int y_below = y + 1 == CACHED_HEIGHT ? 0 : y+1;
    for (int x = 0; x < CACHED_WIDTH; ++x) {
      int x_west = x == 0 ? CACHED_WIDTH - 1 : x-1;
      int x_east = x + 1 == CACHED_WIDTH ? 0 : x+1;
#define GET(x, y) &cached_map_tiles_[(y)*CACHED_WIDTH+(x)];
      EvidyonMapTile_Data* cached_map_tile = GET(x,y);
      cached_map_tile->neighbors[0] = GET(x,y_above);
      cached_map_tile->neighbors[1] = GET(x_east,y);
      cached_map_tile->neighbors[2] = GET(x,y_below);
      cached_map_tile->neighbors[3] = GET(x_west,y);
#undef GET
    }
  }

  // Initialize corner height references
  for (int y = 0; y < CACHED_HEIGHT; ++y) {
    int y_below = y + 1 == CACHED_HEIGHT ? 0 : y+1;
    for (int x = 0; x < CACHED_WIDTH; ++x) {
      int x_east = x + 1 == CACHED_WIDTH ? 0 : x+1;
      EvidyonMapTile_Data* cached_map_tile
          = &cached_map_tiles_[(y)*CACHED_WIDTH+(x)];
#define GET(x, y) &cached_corners_[(y)*CACHED_CORNERS_WIDTH+(x)];
      cached_map_tile->corners[0] = GET(x,y);
      cached_map_tile->corners[1] = GET(x_east,y);
      cached_map_tile->corners[2] = GET(x,y_below);
      cached_map_tile->corners[3] = GET(x_east,y_below);
#undef GET
    }
  }

  // Reset other misc. data
  reset();
}



//----[  createVertexBuffer  ]-------------------------------------------------
bool EvidyonMapRenderer::createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateVertexBuffer(
      sizeof(MapVertex) * VERTICES_PER_QUAD * MAX_VISIBLE_QUADS,
      D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
      D3DFVF_EVIDYON_MAPVERTEX,
      D3DPOOL_DEFAULT,
      &vb_,
      NULL))) return false;

  return true;
}



//----[  createIndexBuffer  ]--------------------------------------------------
bool EvidyonMapRenderer::createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateIndexBuffer(
        sizeof(MapIndex) * 3 * TRIANGLES_PER_QUAD * MAX_VISIBLE_QUADS,
        D3DUSAGE_WRITEONLY,
        D3DFMT_EVIDYON_MAPINDEX,
        D3DPOOL_DEFAULT,
        &ib_,
        NULL))) return false;

  MapIndex* indices;
  if (FAILED(ib_->Lock(0, 0, (LPVOID*)&indices, 0))) return false;

  // 2 0 1
  // 2 1 4
  // 2 4 3
  // 2 3 0
  MapIndex base_index = 0;
  for (int quad = 0; quad < MAX_VISIBLE_QUADS; ++quad, base_index += VERTICES_PER_QUAD) {
    const MapIndex center_index = base_index + 2;
    const MapIndex offsets[] = { base_index + 1, base_index + 4, base_index + 3 };
    *(indices++) = center_index; *(indices++) = base_index; *(indices++) = offsets[0];
    *(indices++) = center_index; *(indices++) = offsets[0]; *(indices++) = offsets[1];
    *(indices++) = center_index; *(indices++) = offsets[1]; *(indices++) = offsets[2];
    *(indices++) = center_index; *(indices++) = offsets[2]; *(indices++) = base_index;
  }

  ib_->Unlock();

  return true;
}





//----[  updateAreaData  ]-----------------------------------------------------
void EvidyonMapRenderer::updateAreaData(EvidyonMapData* map_data,
                                        const VisibleTilesRect::Area* area) {
  for (int row = area->top; row <= area->bottom; ++row) {
    size_t y_base = MAP_TO_RANGE(row, CACHED_HEIGHT) * CACHED_WIDTH;
    for (int col = area->left; col <= area->right; ++col) {
      size_t x = MAP_TO_RANGE(col, CACHED_WIDTH);
      EvidyonMapTile_Data* data = &cached_map_tiles_[y_base+x];
      map_data->updateMapTileData(col, row, data);
    }
  }
}




//----[  computePreVisibility  ]-----------------------------------------------
void EvidyonMapRenderer::computePreVisibility(
    const VisibleTilesRect::Area* area) {

  for (int row = area->top; row <= area->bottom; ++row) {
    size_t y_base = MAP_TO_RANGE(row, CACHED_HEIGHT) * CACHED_WIDTH;
    for (int col = area->left; col <= area->right; ++col) {
      size_t x = MAP_TO_RANGE(col, CACHED_WIDTH);
      EvidyonMapTile_Data* data = &cached_map_tiles_[y_base+x];

      float col_left = col * 1.0f;
      float row_top = row * 1.0f;
      data->left = col_left;
      data->top = row_top;

      // 0 1 2
      // 3 4 5
      // 6 7 8
      const float h[9] = {
        data->neighbors[0]->neighbors[3]->terrain_height,
        data->neighbors[0]->terrain_height,
        data->neighbors[0]->neighbors[1]->terrain_height,
        data->neighbors[3]->terrain_height,
        data->terrain_height,
        data->neighbors[1]->terrain_height,
        data->neighbors[2]->neighbors[3]->terrain_height,
        data->neighbors[2]->terrain_height,
        data->neighbors[2]->neighbors[1]->terrain_height,
      };

#define AVERAGE4(a,b,c,d) (a+b+c+d) / 4.0f
#define AVERAGE3(a,b,c) (a+b+c) / 3.0f
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MIN3(a,b,c) (((a)<(b)?((a)<(c)?(a):(c)):((b)<(c)?(b):(c))))
#define MAX3(a,b,c) (((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c))))
#define MAX4(a,b,c,d) MAX(MAX(a,b),MAX(b,c))
#define INTERPOLATE(a,b,c,d) MAX(MAX(a,b),MAX(c,d))
#define INTERPOLATE1(a,b,c,d) MAX(AVERAGE3(a,b,c),d)
#define INTERPOLATE2(a,b,c,d) MAX(AVERAGE3(a,b,d),c)
#define INTERPOLATE3(a,b,c,d) MAX(AVERAGE3(a,c,d),b)
#define INTERPOLATE4(a,b,c,d) MAX(AVERAGE3(b,c,d),a)

      // always update the top-left
      //if (col == area->left && row == area->top) {
        data->corners[0]->terrain_height = INTERPOLATE(h[0], h[1], h[3], h[4]);
      //}

      // update the top-right if this is the first row
      //if (row == area->top) {
        data->corners[1]->terrain_height = INTERPOLATE(h[1], h[2], h[4], h[5]);
      //}

      // update the bottom-left if this is the first column
      //if (col == area->left) {
        data->corners[2]->terrain_height = INTERPOLATE(h[3], h[4], h[6], h[7]);
      //}

      // always update the bottom-right
      data->corners[3]->terrain_height = INTERPOLATE(h[4], h[5], h[7], h[8]);

#define GREATER3(a,b,c) (a>h[4]&&b>h[4]&&c>h[4])
      bool corner = GREATER3(h[0], h[1], h[3]) ||
                    GREATER3(h[1], h[2], h[5]) ||
                    GREATER3(h[3], h[6], h[7]) ||
                    GREATER3(h[5], h[7], h[8]);


      if (corner) {
        data->terrain_center_vertex_height =
          MAX4(data->corners[0]->terrain_height,
                   data->corners[1]->terrain_height,
                   data->corners[2]->terrain_height,
                   data->corners[3]->terrain_height);
      } else {
        data->terrain_center_vertex_height =
          AVERAGE4(data->corners[0]->terrain_height,
                   data->corners[1]->terrain_height,
                   data->corners[2]->terrain_height,
                   data->corners[3]->terrain_height);
      }

#undef AVERAGE4
#undef MAX
#undef MIN
#undef INTERPOLATE

    }
  }
}


//
//void raytrace(int x0, int y0, int x1, int y1)
//{
//    int dx = abs(x1 - x0);
//    int dy = abs(y1 - y0);
//    int x = x0;
//    int y = y0;
//    int n = 1 + dx + dy;
//    int x_inc = (x1 > x0) ? 1 : -1;
//    int y_inc = (y1 > y0) ? 1 : -1;
//    int error = dx - dy;
//    dx *= 2;
//    dy *= 2;
//
//    for (; n > 0; --n)
//    {
//        visit(x, y);
//
//        if (error > 0)
//        {
//            x += x_inc;
//            error -= dy;
//        }
//        else
//        {
//            y += y_inc;
//            error += dx;
//        }
//    }
//}

//----[  computeVisibility  ]--------------------------------------------------
void EvidyonMapRenderer::computeVisibility(int x, int y) {
  for (int i = 0; i < CACHED_TILES; ++i) {
    cached_map_tiles_[i].visibility = TILEVISIBILITY_HIDDEN;
  }

  std::set<std::pair<int,int>> visited;
  recursiveSetNormalVisibility(x, y, &visited);

  int left = cached_map_tiles_visibility_.left();
  int top = cached_map_tiles_visibility_.top();
  for (int row = top; row < top + CACHED_HEIGHT; row++) {
    size_t y_base = MAP_TO_RANGE(row, CACHED_HEIGHT) * CACHED_WIDTH;
    for (int col = left; col < left + CACHED_WIDTH; ++col) {
      size_t x_offset = MAP_TO_RANGE(col, CACHED_WIDTH);
      if ((row == top) || (row + 1 == (top + CACHED_HEIGHT)) ||
          (col == left) || (col + 1 == (left + CACHED_WIDTH))) {
        cached_map_tiles_[y_base+x_offset].visibility = TILEVISIBILITY_INVISIBLE;
      } else {
        if (cached_map_tiles_[y_base+x_offset].neighbors[0]->occluding &&
            cached_map_tiles_[y_base+x_offset].neighbors[1]->occluding &&
            cached_map_tiles_[y_base+x_offset].neighbors[2]->occluding &&
            cached_map_tiles_[y_base+x_offset].neighbors[3]->occluding) {
          cached_map_tiles_[y_base+x_offset].visibility = TILEVISIBILITY_HIDDEN;
        } else {
          //cached_map_tiles_[y_base+x_offset].visibility = TILEVISIBILITY_NORMAL;
          if (cached_map_tiles_[y_base+x_offset].terrain_center_vertex_height < -3.0f) {
            cached_map_tiles_[y_base+x_offset].visibility = TILEVISIBILITY_INVISIBLE;
          }/* else {
            cached_map_tiles_[y_base+x_offset].visibility =
              raytraceVisibility(x, y, col, row) ? TILEVISIBILITY_NORMAL : TILEVISIBILITY_OCCLUDED;
          }*/
          else if (cached_map_tiles_[y_base+x_offset].visibility == TILEVISIBILITY_NORMAL &&
                   raytraceVisibility(x,y,col,row) == false) {
            cached_map_tiles_[y_base+x_offset].visibility = TILEVISIBILITY_OCCLUDED;
          }
        }
      }
    }
  }
}




//----[  raytraceVisibility  ]-------------------------------------------------
bool EvidyonMapRenderer::raytraceVisibility(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int x = x0;
    int y = y0;
    int n = 1 + dx + dy;
    int x_inc = (x1 > x0) ? 1 : -1;
    int y_inc = (y1 > y0) ? 1 : -1;
    int error = dx - dy;
    dx *= 2;
    dy *= 2;

    for (; n > 0; --n)
    {
        if (cached_map_tiles_[MAP_TO_RANGE(x, CACHED_WIDTH)
              + MAP_TO_RANGE(y,CACHED_HEIGHT)*CACHED_WIDTH].occluding) {
            return false;
        }

        if (error > 0)
        {
            x += x_inc;
            error -= dy;
        }
        else
        {
            y += y_inc;
            error += dx;
        }
    }
    return true;
}



//----[  recursiveSetNormalVisibility  ]---------------------------------------
void EvidyonMapRenderer::recursiveSetNormalVisibility(
    int x,
    int y,
    std::set<std::pair<int,int>>* visited) {
  visited->insert(std::pair<int,int>(x,y));
  cached_map_tiles_[MAP_TO_RANGE(x, CACHED_WIDTH) + MAP_TO_RANGE(y,CACHED_HEIGHT)*CACHED_WIDTH].visibility = TILEVISIBILITY_NORMAL;
  if (cached_map_tiles_[MAP_TO_RANGE(x, CACHED_WIDTH) + MAP_TO_RANGE(y,CACHED_HEIGHT)*CACHED_WIDTH].occluding) return;
  bool nesw[4] = {
    y - 1 >= cached_map_tiles_visibility_.top(),
    x + 1 <= cached_map_tiles_visibility_.right(),
    y + 1 <= cached_map_tiles_visibility_.bottom(),
    x - 1 >= cached_map_tiles_visibility_.left(),
  };
  if (nesw[3]) {
    if (visited->find(std::pair<int,int>(x-1,y)) == visited->end()) {
      recursiveSetNormalVisibility(x-1,y,visited);
    }
  }
  if (nesw[1]) {
    if (visited->find(std::pair<int,int>(x+1,y)) == visited->end()) {
      recursiveSetNormalVisibility(x+1,y,visited);
    }
  }
  if (nesw[0]) {
    if (visited->find(std::pair<int,int>(x,y-1)) == visited->end()) {
      recursiveSetNormalVisibility(x,y-1,visited);
    }
  }
  if (nesw[2]) {
    if (visited->find(std::pair<int,int>(x,y+1)) == visited->end()) {
      recursiveSetNormalVisibility(x,y+1,visited);
    }
  }

  //// do corner processing
  //if (nesw[0] && nesw[1]) {
  //  if (visited->find(std::pair<int,int>(x+1,y-1)) == visited->end()) {
  //    recursiveSetNormalVisibility(x+1,y-1,visited);
  //  }
  //}
  //if (nesw[0] && nesw[3]) {
  //  if (visited->find(std::pair<int,int>(x-1,y-1)) == visited->end()) {
  //    recursiveSetNormalVisibility(x-1,y-1,visited);
  //  }
  //}
  //if (nesw[2] && nesw[1]) {
  //  if (visited->find(std::pair<int,int>(x+1,y+1)) == visited->end()) {
  //    recursiveSetNormalVisibility(x+1,y+1,visited);
  //  }
  //}
  //if (nesw[2] && nesw[3]) {
  //  if (visited->find(std::pair<int,int>(x-1,y+1)) == visited->end()) {
  //    recursiveSetNormalVisibility(x-1,y+1,visited);
  //  }
  //}
}


//----[  computePostVisibility  ]----------------------------------------------
void EvidyonMapRenderer::computePostVisibility(EvidyonMapData* map_data) {
  // iterate through all and set the vertex colors
  int left = cached_map_tiles_visibility_.left();
  int top = cached_map_tiles_visibility_.top();
  for (int row = top; row < top + CACHED_HEIGHT; row++) {
    size_t y_base = MAP_TO_RANGE(row, CACHED_HEIGHT) * CACHED_WIDTH;
    for (int col = left; col < left + CACHED_WIDTH; ++col) {
      size_t x = MAP_TO_RANGE(col, CACHED_WIDTH);
      EvidyonMapTile_Data* data = &cached_map_tiles_[y_base+x];
      TileVisibility h[9] = {
        data->neighbors[0]->neighbors[3]->visibility,
        data->neighbors[0]->visibility,
        data->neighbors[0]->neighbors[1]->visibility,
        data->neighbors[3]->visibility,
        data->visibility,
        data->neighbors[1]->visibility,
        data->neighbors[2]->neighbors[3]->visibility,
        data->neighbors[2]->visibility,
        data->neighbors[2]->neighbors[1]->visibility,
      };
      D3DCOLOR c[9];
      if (h[4] == TILEVISIBILITY_HIDDEN || h[4] == TILEVISIBILITY_INVISIBLE) {
        for (int i = 0; i < 9; ++i) {
          c[i] = D3DCOLOR_XRGB(0,0,0);
        }
        data->corners[0]->vertex_color = D3DCOLOR_XRGB(0,0,0);
        data->corners[1]->vertex_color = D3DCOLOR_XRGB(0,0,0);
        data->corners[2]->vertex_color = D3DCOLOR_XRGB(0,0,0);
        data->corners[3]->vertex_color = D3DCOLOR_XRGB(0,0,0);
      } else {
        int four[4][4] = {
          { 0, 1, 3, 4, },
          { 1, 2, 4, 5, },
          { 3, 4, 6, 7, },
          { 4, 5, 7, 8, },
        };
        for (int i = 0; i < 4; ++i) {
          bool invisible = false, hidden = false, occluded = false;
          for (int j = 0; j < 4; ++j) {
            if (h[four[i][j]] == TILEVISIBILITY_HIDDEN) {
              hidden = true;
            } else if (h[four[i][j]] == TILEVISIBILITY_INVISIBLE) {
              hidden = true;
            } else if (h[four[i][j]] == TILEVISIBILITY_OCCLUDED) {
              occluded = true;
            }
          }
          if (invisible || hidden) {
            data->corners[i]->vertex_color = D3DCOLOR_XRGB(0,0,0);
          } else if (occluded) {
            data->corners[i]->vertex_color = D3DCOLOR_XRGB(200,200,200);
          } else {
            data->corners[i]->vertex_color = D3DCOLOR_XRGB(255,255,255);
          }
        }
        /*
        for (int i = 0; i < 9; ++i) {
          switch (h[i]) {
            case TILEVISIBILITY_NORMAL:    c[i] = D3DCOLOR_XRGB(255,255,255); break;
            case TILEVISIBILITY_INVISIBLE:
            case TILEVISIBILITY_HIDDEN:    c[i] = D3DCOLOR_XRGB(0,0,0);     break;
            case TILEVISIBILITY_OCCLUDED:  c[i] = D3DCOLOR_XRGB(180,180,180);    break;
          }
        }*/
      }
      data->corners[0]->terrain_color = (data->corners[0]->terrain_height < -1.0f) ? D3DCOLOR_XRGB(0,0,0) : data->corners[0]->vertex_color;
      data->corners[1]->terrain_color = (data->corners[1]->terrain_height < -1.0f) ? D3DCOLOR_XRGB(0,0,0) : data->corners[1]->vertex_color;
      data->corners[2]->terrain_color = (data->corners[2]->terrain_height < -1.0f) ? D3DCOLOR_XRGB(0,0,0) : data->corners[2]->vertex_color;
      data->corners[3]->terrain_color = (data->corners[3]->terrain_height < -1.0f) ? D3DCOLOR_XRGB(0,0,0) : data->corners[3]->vertex_color;
      data->terrain_center_vertex_color = INTERPOLATE_COLOR(data->corners[0]->terrain_color,
                                                            data->corners[1]->terrain_color,
                                                            data->corners[2]->terrain_color,
                                                            data->corners[3]->terrain_color);
    }
  }
}


}
}
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
#ifndef __WALLS_H__
#define __WALLS_H__



/*

On recenter, rebuild the changes in the cached map data from *source* data?
i.e. build the complex transitions n' stuff so that the map format is simple.  Get
vertex heights for all vertices on the map, walls, etc.

Use a single VB/IB pair for all map rendering.

Use a VB/IB pair that contains portions of meshes using the same texture.  Sort
scenery by mesh type.  When texture comes up, find meshes using textures and
scenery that references meshes 

Use a texture loop for the terrain and one for the scenery, since it's unlikely
that they will share a texture (it's possible and that will cause slowdown, but
not having to switch the geometry buffers as frequently should make up for that)

SetFVF(geometry_fvf)
SetTransform(view)
SetTransform(projection)

// Node:  my_index, parent_index, active, child_list
// map_node = streamNode(dynamic_map_vb, dynamic_map_ib)
// texture_node = textureNode(map_node, d3d_texture)
// 
// when updating map, each quad has its own node
//    quad_texture_node = textureNode(map_node, quad.texture.d3d)
//    render_state_node = renderStateNode(quad_texture_node, quad.texture.state)
//    lock_streams_node = lockStreamsNode(render_state_node, discard)
//    

// map
SetTransform(null)
SetFVF
SetStreamSource(dynamic_map_vb)
SetIndices(dynamic_map_ib)
  SetTexture(texture)
    SetRenderState()
      Lock(dynamic_map_vb, discard)
      Lock(dynamic_map_ib, discard)
      // copy map vertices, indices
      Unlock(dynamic_map_vb)
      Unlock(dynamic_map_ib)
      DrawIndexedPrimitive(geometry)

// meshes
SetTexture(texture)
  SetRenderState()
    SetStreamSource(texture_vb)
    SetIndices(texture_ib)
      SetTransform(scenery_transform)
      DrawIndexedPrimtive(mesh)

// skeletal animated (dynamic) geometry
SetStreamSource(vb_for_K_bone_influences)
SetFVF
SetIndexedVertexBlendEnable
  SetTexture(texture)
    SetRenderState()
      SetTransform(bone_matrix_group(0).first ... bone_matrix__group(0).last)
      DrawIndexedPrimitive(bone_matrix_group(0))
      SetTransform(bone_matrix_group(1).first ... bone_matrix__group(1).last)
      DrawIndexedPrimitive(bone_matrix_group(1))
      ...
      SetTransform(bone_matrix_group(n).first ... bone_matrix__group(n).last)
      DrawIndexedPrimitive(bone_matrix_group(n))

// particle effects
SetStreamSource(dynamic_particles_vb)
SetFVF
SetTexture(texture)
  Lock(dynamic_particles_vb, discard)
  // write particles
  Unlock(dynamic_particles_vb)
  Render

// special effects (beam, lightning)
SetStreamSource(particlefx_vb)
SetIndices(particlefx_ib)
SetTexture(texture)
  SetRenderState()
    Lock(dynamic_map_vb, discard)
    Lock(dynamic_map_ib, discard)
    // copy special-effect vertices, indices
    Unlock(dynamic_map_vb)
    Unlock(dynamic_map_ib)
    DrawIndexedPrimitive(effects)
    

// gui
SetTexture(gui_texture)
SetFVF
SetStreamSource




EvidyonD3DSceneNode
  addTextureNode(texture, node)
  addStreamNode(vb, ib)
  addStateNode(state)
  addDrawNode(node);





*/









namespace Evidyon {
namespace Map {


// the tops of walls are drawn within the extended view area (+1 on all sides)
// the sides of walls are drawn within the inner view area
// the top of a wall is drawn if any of its sides are drawn
  //  -> a wall that is surrounded by taller walls draws the sides of the
  //     taller walls but not its own top

  // data: - the visibility for each tile
  //       - the kind of wall present there
  // rules:
  //  - a tile is responsible for drawing higher walls that border it
  //  - a tile always draws its own flat top
  //  - a wall takes its vertex colors from the visibility states of surrounding tiles




struct Wall {

  // 0 - none, 1 - low, 2 - medium, 3 - high,
  // 4 - high angled corner, 5 - high angled corner (2)
  // 6 - floorwall (center flat, negative walls),
  // 7 - angled floorwall
  unsigned dungeon_wall_type : 3; // center flat element

  // 0 - none, 1 - low, 2 - medium, 3 - high
  unsigned dungeon_wall_n : 2;
  unsigned dungeon_wall_e : 2;
  unsigned dungeon_wall_s : 2;
  unsigned dungeon_wall_w : 2;

  unsigned building_n : 1;
  unsigned building_e : 1;
  unsigned building_s : 1;
  unsigned building_w : 1;
  unsigned building_inner : 1;  // if this is a corner, this defines the connection method
};



#define COMP(a) ((~(a))&1)
#define DUNGEON_WALL_HIGH_ANGLED_CORNER(height,n_0_s_1,w_0_e_1)   (((height)<<(n_0_s_1==02*3*))|((height)<<

#define BUILDING_WALL_TYPE(direction)   (1<<(4-direction))
#define BUILDING_WALL_CORNER_TYPE(n_0_s_1,w_0_e_1,i)   ((COMP(n_0_s_1)<<4)|((w_0_e_1)<<3)|((n_0_s_1)<<2)|(COMPL(w_0_e_1)<<1)|(i)&1)


struct WallVertex {
  float x, y, z;
  float nx, ny, nz;
  D3DCOLOR diffuse;
  float u, v;
};

typedef unsigned short WallIndex;

#define D3DFVF_WALLVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFMT_WALLINDEX  (D3DFMT_INDEX16)


class Walls {
  static const int QUADS_PER_WALL_TILE = 4;
  static const int VERTICES_PER_WALL_TILE  = QUADS_PER_WALL_TILE * 4;
  static const int TRIANGLES_PER_WALL_TILE = QUADS_PER_WALL_TILE * 2;
  static const int INDICES_PER_WALL_TILE   = TRIANGLES_PER_WALL_TILE * 3;


public:
  Walls();

  // Initializes the wall renderer
  bool create(LPDIRECT3DDEVICE9 d3d_device);

  // Frees resources used by this renderer
  void destroy();

  // Synchronizes this renderer with the map
  void update(MapManager* map,
              int center_x,
              int center_y,
              const TileVisibility* visibility,
              bool visible_areas_valid[2],
              const VisibleTilesRect::Area new_visible_areas[2]);

  // Draws the geometry to the screen.  The state manager is used to move the
  // texture into the device if any geometry is going to be rendered with it.
  bool render(LPDIRECT3DDEVICE9 d3d_device,
              TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);

public:

private:

  // Initializes the internal vertex buffer
  bool createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Initializes the internal index buffer
  bool createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Generates geometry for the provided wall type.  It is assumed that each wall
  // will use the full number of allotted vertices; if this is not the case, it
  // should form degenerate triangles.
  void writeWallGeometry(const Wall* wall,
                         const TileVisibility neighbor_visibility[4],
                         WallIndex first_vertex_index,
                         WallVertex* vertices,
                         WallIndex* indices);

  // Updates the local wall types
  void updateCachedWalls(const MapManager* map, 
                         const VisibleTilesRect::Area* area);

  // Pulls the textures in the given area into the cache
  void updateCachedTextures(const MapManager* map, 
                            const VisibleTilesRect::Area* area);

  void update


private:
  LPDIRECT3DVERTEXBUFFER9 vb_;
  LPDIRECT3DINDEXBUFFER9 ib_;
};


}
}


#endif
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
#ifndef __SHARED_MESH_MESHRENDERER_H__
#define __SHARED_MESH_MESHRENDERER_H__
#pragma once



#include "meshgeometry.h"
#include <vector>
#include "meshindex.h"
#include "mesh.h" // get rid of this once MeshArray is not a std::vector

namespace Evidyon {
namespace Mesh {
struct Mesh;
}
}


namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Mesh {


  
//----[  MeshRenderer  ]-------------------------------------------------------
class MeshRenderer {
  typedef std::vector<Mesh> MeshArray;
  static const int BUFFER_TRIANGLE_CAPACITY = 8192; // max tris/model subset
  static const int VERTEX_BUFFER_CAPACITY = BUFFER_TRIANGLE_CAPACITY * 2;
  static const int INDEX_BUFFER_CAPACITY  = BUFFER_TRIANGLE_CAPACITY * 3;

public:
  MeshRenderer();

  // Initializes the renderer
  bool create(LPDIRECT3DDEVICE9 d3d_device);

  // Frees all allocated objects.  This will invoke delete[] on all of the
  // meshes' vertices, indices and subsets.
  void destroy();

  // Puts a new mesh into the renderer.  This method confirms that the provided
  // index is where the mesh is being added; if it isn't, 'false' is returned.
  bool addMesh(MeshIndex index, const Mesh& mesh);

  // Removes all meshes from the renderer
  void clearMeshes();

  // Copies data for the subset of the given mesh into the geometry buffers.
  // Each following renderInstance will then render that same mesh.
  // This method does not change any D3D device state.
  bool initializeMeshGeometry(MeshIndex mesh_index, MeshSubsetIndex subset_index);

  // Adds dynamic geometry to the rendering buffer.  renderDynamicGeometry must
  // then be called to actually draw an instance of this geometry.
  bool initializeDynamicGeometry(const GeometryVertex* vertices,
                                 unsigned int number_of_vertices,
                                 const GeometryIndex* indices,
                                 unsigned int number_of_indices);

  // Draws an instance of the geometry that was set up with either
  // initializeMeshGeometry or initializeDynamicGeometry.  This does not
  // change the current texture state, but will modify the device state as
  // necessary to draw the geometry.
  bool renderInstance(EvidyonD3DRenderStateManager* renderstate_manager);

  // Returns the number of subsets in a given mesh.  This is used by the
  // scenery renderer.
  MeshSubsetIndex getNumberOfMeshSubsets(MeshIndex mesh_index);

private:

  // Resets the contents of the renderer
  void zero();

  // Initializes the internal vertex buffer
  bool createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Initializes the internal index buffer
  bool createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device);

  // Locks the buffers so that geometry can be added.  This method only fails
  // if more geometry than the buffers can hold is requested.
  bool lock(unsigned int number_of_vertices,
            unsigned int number_of_indices,
            GeometryVertex** vertices,
            GeometryIndex** indices);

  // Frees the lock on the buffers so that they can be rendered.  Only call
  // this after a successful lock().
  void unlock();


private:

  // Device used to create the renderer
  LPDIRECT3DDEVICE9 d3d_device_;

  // All of the meshes in the renderer
  MeshArray meshes_;

  // Dynamic vertex and index buffers for rendering geometry.  When an instance
  // is initialized, the geometry is copied from the mesh array into the
  // buffers.  Each rendering call simply re-draws the current geometry.
  LPDIRECT3DVERTEXBUFFER9 vb_;
  LPDIRECT3DINDEXBUFFER9 ib_;

  // State information for rendering instances
  UINT vertices_offset_, indices_offset_;
  UINT vb_vertices_remaining_, ib_indices_remaining_;
  UINT last_num_vertices_, last_num_indices_, last_num_triangles_;
};

}
}

#endif
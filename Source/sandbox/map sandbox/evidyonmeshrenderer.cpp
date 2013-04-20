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
#include "evidyonmeshrenderer.h"
#include <assert.h>
#include "../shared/evidyond3drenderstatemanager.h"
#include "../common/safemacros.h"

namespace Evidyon {

namespace Meshes {

  
//----[  EvidyonMeshRenderer  ]------------------------------------------------
EvidyonMeshRenderer::EvidyonMeshRenderer() {
  zero();
}





//----[  create  ]-------------------------------------------------------------
bool EvidyonMeshRenderer::create(LPDIRECT3DDEVICE9 d3d_device) {
  if (!d3d_device || vb_ || ib_) return false;
  zero();
  if (!createVertexBuffer(d3d_device) || !createIndexBuffer(d3d_device)) {
    destroy();
    return false;
  }
  (d3d_device_ = d3d_device)->AddRef();
  return true;
}






//----[  destroy  ]------------------------------------------------------------
void EvidyonMeshRenderer::destroy() {
  for (MeshArray::iterator i = meshes_.begin(); i != meshes_.end(); ++i) {
    Mesh& mesh = *i;
    for (unsigned int i = 0; i < mesh.number_of_subsets; ++i) {
      SAFE_DELETE_ARRAY(mesh.subsets[i].vertices);
      SAFE_DELETE_ARRAY(mesh.subsets[i].indices);
    }
    SAFE_DELETE_ARRAY(mesh.subsets);
  }

  SAFE_RELEASE(vb_);
  SAFE_RELEASE(ib_);
  SAFE_RELEASE(d3d_device_);

  zero();
}






//----[  addMesh  ]------------------------------------------------------------
bool EvidyonMeshRenderer::addMesh(MeshIndex index, const Mesh& mesh) {
  if (meshes_.size() != index) return false;
  meshes_.push_back(mesh);
  return true;
}







//----[  initializeInstance  ]-------------------------------------------------
bool EvidyonMeshRenderer::initializeInstance(MeshIndex mesh_index,
                                             MeshSubsetIndex subset_index) {
  if (mesh_index >= meshes_.size()) return false;
  const Mesh& mesh = meshes_.at(mesh_index);
  if (subset_index >= mesh.number_of_subsets) return false;
  const Mesh::Subset* subset = &mesh.subsets[subset_index];

  unsigned int number_of_vertices = subset->number_of_vertices;
  unsigned int number_of_indices = subset->number_of_indices;
  GeometryVertex* vertices;
  GeometryIndex* indices;
  if (lock(subset->number_of_vertices,
           subset->number_of_indices,
           &vertices,
           &indices)) {
    memcpy(vertices, subset->vertices, sizeof(GeometryVertex)*number_of_vertices);
    memcpy(indices, subset->indices, sizeof(GeometryIndex)*number_of_indices);
    unlock();
    return true;
  } else {
    return false;
  }
}







//----[  renderInstance  ]-----------------------------------------------------
bool EvidyonMeshRenderer::renderInstance(
    EvidyonD3DRenderStateManager* renderstate_manager) {

  if (!renderstate_manager->setStream0GeometryBuffers(
      vb_,
      D3DFVF_EVIDYON_GEOMETRYVERTEX,
      sizeof(GeometryVertex),
      ib_)) {
    return false;
  }

  HRESULT hr;
  hr = d3d_device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                         vertices_offset_,
                                         0,
                                         last_num_vertices_,
                                         indices_offset_,
                                         last_num_triangles_);
  return SUCCEEDED(hr);
}





//----[  getNumberOfMeshSubsets  ]---------------------------------------------
MeshSubsetIndex EvidyonMeshRenderer::getNumberOfMeshSubsets(
    MeshIndex mesh_index) {
  return (mesh_index >= meshes_.size())
    ? 0 : meshes_.at(mesh_index).number_of_subsets;
}



//----[  zero  ]---------------------------------------------------------------
void EvidyonMeshRenderer::zero() {
  meshes_.clear();
  d3d_device_ = NULL;
  vb_ = NULL;
  ib_ = NULL;
  vertices_offset_ = 0;
  indices_offset_ = 0;
  vb_vertices_remaining_ = 0;
  ib_indices_remaining_ = 0;
  last_num_vertices_ = 0;
  last_num_indices_ = 0;
  last_num_triangles_ = 0;
}


//----[  createVertexBuffer  ]-------------------------------------------------
bool EvidyonMeshRenderer::createVertexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateVertexBuffer(
      sizeof(GeometryVertex) * VERTEX_BUFFER_CAPACITY,
      D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
      D3DFVF_EVIDYON_GEOMETRYVERTEX,
      D3DPOOL_DEFAULT,
      &vb_,
      NULL))) return false;
  return true;
}



//----[  createIndexBuffer  ]--------------------------------------------------
bool EvidyonMeshRenderer::createIndexBuffer(LPDIRECT3DDEVICE9 d3d_device) {
  if (FAILED(d3d_device->CreateIndexBuffer(
        sizeof(GeometryIndex) * INDEX_BUFFER_CAPACITY,
        D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
        D3DFMT_EVIDYON_GEOMETRYINDEX,
        D3DPOOL_DEFAULT,
        &ib_,
        NULL))) return false;
  return true;
}





//----[  lock  ]---------------------------------------------------------------
// The locking procedure for each buffer delays the offset advance into the
// buffer until the next call to lock.  This is essential for the geometry
// to be rendered correctly.
bool EvidyonMeshRenderer::lock(unsigned int number_of_vertices,
                               unsigned int number_of_indices,
                               GeometryVertex** vertices,
                               GeometryIndex** indices) {
  assert(number_of_vertices);
  assert(number_of_indices);
  assert(vertices);
  assert(indices);

  // Lock the vertex buffer
  if (number_of_vertices > vb_vertices_remaining_) {
    HRESULT hr;
    hr = vb_->Lock(0,
                   0,
                   (void**)vertices,
                   D3DLOCK_DISCARD);
    if (FAILED(hr)) return false;
    vertices_offset_ = 0;
    vb_vertices_remaining_ = VERTEX_BUFFER_CAPACITY;
  } else {
    vertices_offset_ += last_num_vertices_; // advance from the last lock
    HRESULT hr;
    hr = vb_->Lock(vertices_offset_ * sizeof(GeometryVertex),
                   vb_vertices_remaining_ * sizeof(GeometryVertex),
                   (void**)vertices,
                   D3DLOCK_NOOVERWRITE);
    if (FAILED(hr)) return false;
  }
  vb_vertices_remaining_ -= number_of_vertices;
  last_num_vertices_ = number_of_vertices;

  // Lock the index buffer
  if (number_of_indices > ib_indices_remaining_) {
    HRESULT hr;
    hr = ib_->Lock(0,
                   0,
                   (void**)indices,
                   D3DLOCK_DISCARD);
    if (FAILED(hr)) {
      vb_->Unlock();
      return false;
    }
    indices_offset_ = 0;
    ib_indices_remaining_ = INDEX_BUFFER_CAPACITY;
  } else {
    indices_offset_ += last_num_indices_; // advance from the last lock
    HRESULT hr;
    hr = ib_->Lock(indices_offset_ * sizeof(GeometryIndex),
                   ib_indices_remaining_ * sizeof(GeometryIndex),
                   (void**)indices,
                   D3DLOCK_NOOVERWRITE);
    if (FAILED(hr)) return false;
  }
  ib_indices_remaining_ -= number_of_indices;
  last_num_indices_ = number_of_indices;
  last_num_triangles_ = number_of_indices / 3;
  return true;
}






//----[  unlock  ]-------------------------------------------------------------
void EvidyonMeshRenderer::unlock() {
  vb_->Unlock();
  ib_->Unlock();
}



}
}
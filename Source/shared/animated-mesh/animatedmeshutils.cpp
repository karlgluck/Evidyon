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
#include "animatedmeshdata.h"
#include "renderableanimatedmesh.h"
#include "animatedmeshanimation.h"
#include "unseenskinnedmesh.h"
#include "animatedmeshutils.h"
#include "common/safemacros.h"
#include <assert.h>
#include <set>
#include "common/serializer/bufferserializer.h"

namespace Evidyon {
namespace AnimatedMesh {



 


//----[  AnimatedMeshUtil_InterpretUnseenSkinnedMesh  ]------------------------
// Algorithm:
//  1 Copy frames
//  2 Copy bones
//  3 For each texture group:
//    31 Condense influence groups
//    32 Sort by bone
//    33 Write out vertices/indices
bool AnimatedMeshUtil_InterpretUnseenSkinnedMesh(
    const UnseenSkinnedMesh* input_source_mesh,
    AnimatedMeshData* output_animated_mesh_data) {
  assert(input_source_mesh);
  assert(output_animated_mesh_data);

  //TODO: erase frames with no dependent bones
  //std::set<unsigned int> referenced_frames;

  // Copy frames
  size_t number_of_frames = input_source_mesh->info.numberOfFrames;
  output_animated_mesh_data->frames =
    new AnimatedMeshData::Frame[number_of_frames];
  output_animated_mesh_data->number_of_frames = number_of_frames;
  for (size_t i = 0; i < number_of_frames; ++i) {
    output_animated_mesh_data->frames[i].parent_frame_index
      = input_source_mesh->frameHierarchy[i].parentFrameIndex;
    output_animated_mesh_data->frames[i].offset
      = input_source_mesh->frameHierarchy[i].offset;
  }

  // Copy bones
  size_t number_of_bones = input_source_mesh->info.numberOfBones;
  output_animated_mesh_data->bones =
    new AnimatedMeshData::Bone[number_of_bones];
  output_animated_mesh_data->number_of_bones = number_of_bones;
  for (size_t i = 0; i < number_of_bones; ++i) {
    output_animated_mesh_data->bones[i].frame_index 
      = input_source_mesh->skeleton[i].frameIndex;
    output_animated_mesh_data->bones[i].inverse_offset 
      = input_source_mesh->skeleton[i].inverseOffset;
  }

  // Copy texture groups
  size_t number_of_texture_groups
    = input_source_mesh->info.numberOfTextureGroups;
  output_animated_mesh_data->texture_groups =
    new AnimatedMeshData::TextureGroup[number_of_texture_groups];
  output_animated_mesh_data->number_of_texture_groups
    = number_of_texture_groups;
  for (size_t texture_group = 0;
              texture_group < number_of_texture_groups;
            ++texture_group) {
    UnseenSkinnedMesh::TextureGroup* source_texture_group
      = &input_source_mesh->textureGroups[texture_group];
    size_t number_of_influence_groups
      = source_texture_group->info.numberOfVertexInfluenceGroups;

    // Total the number of vertices and indices
    size_t number_of_group_vertices = 0;
    size_t number_of_group_indices = 0;
    for (size_t i = 0; i < number_of_influence_groups; ++i) {
      UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influence_group
        = &source_texture_group->vertexInfluenceGroups[i];
      number_of_group_vertices += influence_group->info.numberOfVertices;
      number_of_group_indices += influence_group->info.numberOfTriangles * 3;
    }

    // Can't have more than the max WORD vertices
    assert(number_of_group_vertices < 65536);

    // Allocate buffers
    AnimatedGeometryVertex* group_vertices
      = new AnimatedGeometryVertex[number_of_group_vertices];
    AnimatedGeometryIndex* group_indices
      = new AnimatedGeometryIndex[number_of_group_indices];

    // Copy each influence group into the buffer
    size_t current_vertex_index = 0, current_index_index = 0;
    for (size_t influence_group_index = 0;
                influence_group_index < number_of_influence_groups;
              ++influence_group_index) {
      UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influence_group
        = &source_texture_group->vertexInfluenceGroups[influence_group_index];
      AnimatedGeometryIndex influence_group_first_vertex_index
        = current_vertex_index;
      switch (influence_group->info.numberOfVertexInfluences) {
      default: assert(false);
      case 0: {
        assert(influence_group->info.d3dVertexFormat ==
               (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1));
        Vertex0Weights* influence_group_vertices
          = (Vertex0Weights*)influence_group->vertices;
        for (size_t i = 0; i < influence_group->info.numberOfVertices; ++i) {
          AnimatedGeometryVertex* vertex = &group_vertices[current_vertex_index++];
          Vertex0Weights* source_vertex = &influence_group_vertices[i];
          vertex->x = source_vertex->x;
          vertex->y = source_vertex->y;
          vertex->z = source_vertex->z;
          vertex->weights[0] = 1.0f;
          vertex->weights[1] = 0.0f;
          vertex->weights[2] = 0.0f;
          vertex->packed_matrix_indices = 0; // all 4 matrices are zero
          vertex->nx = source_vertex->nx;
          vertex->ny = source_vertex->ny;
          vertex->nz = source_vertex->nz;
          vertex->u = source_vertex->u;
          vertex->v = source_vertex->v;
        }
        } break;
      case 2: {
        assert(influence_group->info.d3dVertexFormat ==
               (D3DFVF_XYZB2|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1));
        Vertex2Weights* influence_group_vertices
          = (Vertex2Weights*)influence_group->vertices;
        for (size_t i = 0; i < influence_group->info.numberOfVertices; ++i) {
          AnimatedGeometryVertex* vertex = &group_vertices[current_vertex_index++];
          Vertex2Weights* source_vertex = &influence_group_vertices[i];
          vertex->x = source_vertex->x;
          vertex->y = source_vertex->y;
          vertex->z = source_vertex->z;
          vertex->weights[0] = source_vertex->weights[0];
          assert(vertex->weights[0] < 1.00001f);
          vertex->weights[1] = 1.0f - vertex->weights[0]; // inverse of 0
          vertex->weights[2] = 0.0f;                      // no weight
          unsigned long matrix_index_to_duplicate
            = (source_vertex->matrix_indices) & 0xFF;
          vertex->packed_matrix_indices
            = source_vertex->matrix_indices
              | matrix_index_to_duplicate << 24
              | matrix_index_to_duplicate << 16;
          vertex->nx = source_vertex->nx;
          vertex->ny = source_vertex->ny;
          vertex->nz = source_vertex->nz;
          vertex->u = source_vertex->u;
          vertex->v = source_vertex->v;
        }
        } break;
      case 3: {
        assert(influence_group->info.d3dVertexFormat ==
               (D3DFVF_XYZB3|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1));
        Vertex3Weights* influence_group_vertices
          = (Vertex3Weights*)influence_group->vertices;
        for (size_t i = 0; i < influence_group->info.numberOfVertices; ++i) {
          AnimatedGeometryVertex* vertex = &group_vertices[current_vertex_index++];
          Vertex3Weights* source_vertex = &influence_group_vertices[i];
          vertex->x = source_vertex->x;
          vertex->y = source_vertex->y;
          vertex->z = source_vertex->z;
          vertex->weights[0] = source_vertex->weights[0];
          vertex->weights[1] = source_vertex->weights[1];
          float sum_of_weights = vertex->weights[0] + vertex->weights[1];
          assert(sum_of_weights < 1.00001f);
          vertex->weights[2] = 1.0f - sum_of_weights;
          unsigned long matrix_index_to_duplicate
            = (source_vertex->matrix_indices) & 0xFF;
          vertex->packed_matrix_indices
            = source_vertex->matrix_indices
              | matrix_index_to_duplicate << 24;
          vertex->nx = source_vertex->nx;
          vertex->ny = source_vertex->ny;
          vertex->nz = source_vertex->nz;
          vertex->u = source_vertex->u;
          vertex->v = source_vertex->v;
        }
        } break;
      case 4: {
        assert(influence_group->info.d3dVertexFormat ==
               (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1));
        Vertex4Weights* influence_group_vertices
          = (Vertex4Weights*)influence_group->vertices;
        for (size_t i = 0; i < influence_group->info.numberOfVertices; ++i) {
          AnimatedGeometryVertex* vertex = &group_vertices[current_vertex_index++];
          Vertex4Weights* source_vertex = &influence_group_vertices[i];
          vertex->x = source_vertex->x;
          vertex->y = source_vertex->y;
          vertex->z = source_vertex->z;
          vertex->weights[0] = source_vertex->weights[0];
          vertex->weights[1] = source_vertex->weights[1];
          vertex->weights[2] = source_vertex->weights[2];
          assert(vertex->weights[0] +
                 vertex->weights[1] +
                 vertex->weights[2] < 1.00001f);
          vertex->packed_matrix_indices = source_vertex->matrix_indices;
          vertex->nx = source_vertex->nx;
          vertex->ny = source_vertex->ny;
          vertex->nz = source_vertex->nz;
          vertex->u = source_vertex->u;
          vertex->v = source_vertex->v;
        }
        } break;
      }

      // Copy all indices, adjusted to offset from the new base vertex index
      assert(influence_group->info.indexSizeInBytes == 2);
      WORD* influence_group_indices = (WORD*)influence_group->indices;
      size_t number_of_influence_group_indices
        = influence_group->info.numberOfTriangles*3;
      for (size_t i = 0; i < number_of_influence_group_indices; ++i) {
        group_indices[current_index_index++]
         = influence_group_first_vertex_index + influence_group_indices[i];
      }
    }

    // TODO: remap to sort by bone reference

    // Save group data
    AnimatedMeshData::TextureGroup* output_texture_group =
      &output_animated_mesh_data->texture_groups[texture_group];
    output_texture_group->vertices = group_vertices;
    output_texture_group->number_of_vertices = number_of_group_vertices;
    output_texture_group->indices = group_indices;
    output_texture_group->number_of_indices = number_of_group_indices;
  }

  // Hooray! Everything worked!
  return true;
}


//----[  AnimatedMeshUtil_DeallocateAnimatedMeshData  ]------------------------
void AnimatedMeshUtil_DeallocateAnimatedMeshData(
    AnimatedMeshData* animated_mesh_data) {
  assert(animated_mesh_data);
  SAFE_DELETE_ARRAY(animated_mesh_data->bones);
  SAFE_DELETE_ARRAY(animated_mesh_data->frames);
  for (size_t i = 0; i < animated_mesh_data->number_of_texture_groups; ++i) {
    AnimatedGeometryVertex* vertices
      = animated_mesh_data->texture_groups[i].vertices;
    SAFE_DELETE_ARRAY(vertices);
    AnimatedGeometryIndex* indices
      = animated_mesh_data->texture_groups[i].indices;
    SAFE_DELETE_ARRAY(indices);
  }
  SAFE_DELETE_ARRAY(animated_mesh_data->texture_groups);

  animated_mesh_data->number_of_bones = 0;
  animated_mesh_data->number_of_frames = 0;
  animated_mesh_data->number_of_texture_groups = 0;
}




//----[  AnimatedMeshUtil_CopyDataFramesToRenderableFrames  ]------------------
void AnimatedMeshUtil_CopyDataFramesToRenderableFrames(
    AnimatedMeshData::Frame* data_frames,
    size_t number_of_frames,
    RenderableAnimatedMesh::Frame* renderable_frames) {
  for (size_t i = 0; i < number_of_frames; ++i) {
    memcpy(&renderable_frames[i].offset,
           &data_frames[i].offset,
           sizeof(D3DXMATRIX));
    unsigned int parent_frame_index = data_frames[i].parent_frame_index;
    renderable_frames[i].parent_frame_index = parent_frame_index;

    // Only the root's parent frame should be invalid, and it should be set
    // to a sentinel value
    assert((parent_frame_index < i) ||
           ((parent_frame_index == 0xFFFFFFFF) && i == 0));
  }
}



//----[  AnimatedMeshUtil_CopyDataBonesToRenderableBones  ]--------------------
void AnimatedMeshUtil_CopyDataBonesToRenderableBones(
    AnimatedMeshData::Bone* data_bones,
    size_t number_of_bones,
    RenderableAnimatedMesh::Frame* renderable_frames,
    size_t number_of_frames,
    RenderableAnimatedMesh::Bone* renderable_bones) {
  for (size_t i = 0; i < number_of_bones; ++i) {
    memcpy(&renderable_bones[i].inverse_offset,
           &data_bones[i].inverse_offset,
           sizeof(D3DXMATRIX));
    
    // No bone should have an invalid source frame
    unsigned int frame_index
      = data_bones[i].frame_index;
    assert(frame_index < number_of_frames);

    renderable_bones[i].frame_index = frame_index;
  }
}



//----[  AnimatedMeshUtil_CreateTextureGroupSubset  ]--------------------------
bool AnimatedMeshUtil_CreateTextureGroupSubset(
    LPDIRECT3DDEVICE9 d3d_device,
    AnimatedGeometryVertex* vertices,
    size_t number_of_vertices,
    AnimatedGeometryIndex* indices,
    size_t number_of_indices,
    const std::set<unsigned int>& subset_vertex_indices,
    const std::set<unsigned int>& subset_bone_indices,
    unsigned int subset_first_index_index,
    unsigned int next_subset_first_index_index,
    RenderableAnimatedMesh::TextureGroupSubset* texture_group_subset,
    unsigned int* bone_to_subset_matrix,            // temporary buffer
    unsigned int* source_vertex_to_subset_vertex) { // temporary buffer
  typedef std::set<unsigned int>::const_iterator UnsignedIntSetIter;

  LPDIRECT3DVERTEXBUFFER9 vb = NULL;
  LPDIRECT3DINDEXBUFFER9 ib = NULL;

  size_t number_of_subset_vertices = subset_vertex_indices.size();
  size_t number_of_subset_indices
    = next_subset_first_index_index - subset_first_index_index;

  // Allocate the D3D geometry buffers
  if (FAILED(d3d_device->CreateVertexBuffer(
      sizeof(AnimatedGeometryVertex) * number_of_subset_vertices, // byte size
      D3DUSAGE_WRITEONLY,
      D3DFVF_ANIMATEDGEOMETRYVERTEX,
      D3DPOOL_MANAGED,
      &vb,
      NULL))) {
    return false;
  }
  if (FAILED(d3d_device->CreateIndexBuffer(
      sizeof(AnimatedGeometryIndex) * number_of_subset_indices, // byte size
      D3DUSAGE_WRITEONLY,
      D3DFMT_ANIMATEDGEOMETRYINDEX,
      D3DPOOL_MANAGED,
      &ib,
      NULL))) {
    SAFE_RELEASE(vb);
    return false;
  }

  // Fill the bone index to subset matrix index table
  unsigned int subset_matrix_index = 0;
  size_t number_of_subset_matrices = subset_bone_indices.size();
  unsigned int* matrix_bone_indices 
    = new unsigned int[number_of_subset_matrices];
  for (UnsignedIntSetIter i = subset_bone_indices.begin();
                          i != subset_bone_indices.end();
                        ++i, ++subset_matrix_index) {
    unsigned int bone_index = *i;
    bone_to_subset_matrix[bone_index] = subset_matrix_index;
    matrix_bone_indices[subset_matrix_index] = bone_index;
  }

  AnimatedGeometryVertex* subset_vertices
    = new AnimatedGeometryVertex[number_of_subset_vertices];
  AnimatedGeometryIndex* subset_indices
    = new AnimatedGeometryIndex[number_of_subset_indices];

  // Copy vertices and generate the vertex index translation table
  unsigned int subset_vertex_index = 0;
  for (UnsignedIntSetIter i = subset_vertex_indices.begin();
                          i != subset_vertex_indices.end();
                        ++i, ++subset_vertex_index) {
    size_t source_vertex_index = *i;
    AnimatedGeometryVertex* target_vertex
      = subset_vertices + subset_vertex_index;
    memcpy(target_vertex,
           vertices + source_vertex_index,
           sizeof(AnimatedGeometryVertex));

    // Translate bone indices to the subset-local matrix indices
    target_vertex->matrix_indices[0]
      = bone_to_subset_matrix[target_vertex->matrix_indices[0]];
    target_vertex->matrix_indices[1]
      = bone_to_subset_matrix[target_vertex->matrix_indices[1]];
    target_vertex->matrix_indices[2]
      = bone_to_subset_matrix[target_vertex->matrix_indices[2]];
    target_vertex->matrix_indices[3]
      = bone_to_subset_matrix[target_vertex->matrix_indices[3]];

    // Build the index-translation buffer
    source_vertex_to_subset_vertex[source_vertex_index]
      = subset_vertex_index;
  }

  // Copy translated indices for this subset from the source buffer
  unsigned int subset_index_index = 0;
  for (unsigned int source_index_index = subset_first_index_index;
                    source_index_index < next_subset_first_index_index;
                  ++source_index_index, ++subset_index_index) {
    subset_indices[subset_index_index] =
      source_vertex_to_subset_vertex[indices[source_index_index]];
  }



  assert(0 == (number_of_subset_indices % 3)); // number of faces is an integer
  size_t number_of_subset_faces = number_of_subset_indices / 3;
#if 0
  { // Optimize the subset faces using the D3DX method
    AnimatedGeometryIndex* face_optimized_subset_indices
      = new AnimatedGeometryIndex[number_of_subset_indices];
    DWORD* face_remap = new DWORD[number_of_subset_faces];
    if (SUCCEEDED(D3DXOptimizeFaces(
        subset_indices,
        number_of_subset_faces,
        number_of_subset_vertices,
        D3DFMT_ANIMATEDGEOMETRYINDEX == D3DFMT_INDEX32,
        face_remap))) {

      // Copy indices by face remap
      for (size_t face = 0; face < number_of_subset_faces; ++face) {
        AnimatedGeometryIndex* source_face
          = subset_indices + face_remap[face]*3;
        AnimatedGeometryIndex* dest_face
          = face_optimized_subset_indices + face*3;
        *(dest_face++) = *(source_face++);
        *(dest_face++) = *(source_face++);
        *(dest_face++) = *(source_face++);
      }

      SAFE_DELETE_ARRAY(subset_indices);
      subset_indices = face_optimized_subset_indices;
    } else {
      assert(false); // did something really wrong...
      SAFE_DELETE_ARRAY(face_optimized_subset_indices);
    }
    SAFE_DELETE_ARRAY(face_remap);
  }

  { // Do a vertex-cache reordering
    DWORD* vertex_remap = new DWORD[number_of_subset_vertices];
    AnimatedGeometryVertex* vertex_optimized_subset_vertices
      = new AnimatedGeometryVertex[number_of_subset_vertices];
    if (SUCCEEDED(D3DXOptimizeVertices(
        subset_indices,
        number_of_subset_faces,
        number_of_subset_vertices,
        D3DFMT_ANIMATEDGEOMETRYINDEX == D3DFMT_INDEX32,
        vertex_remap))) {

      // Copy vertices by vertex remap
      for (size_t vertex = 0; vertex < number_of_subset_vertices; ++vertex) {
        AnimatedGeometryVertex* source_vertex
          = subset_vertices + vertex_remap[vertex];
        AnimatedGeometryVertex* dest_vertex
          = vertex_optimized_subset_vertices + vertex;
        memcpy(dest_vertex, source_vertex, sizeof(AnimatedGeometryVertex));
      }

      SAFE_DELETE_ARRAY(subset_vertices);
      subset_vertices = vertex_optimized_subset_vertices;
    } else {
      assert(false); // this usually shouldn't happen
      SAFE_DELETE_ARRAY(vertex_optimized_subset_vertices);
    }
    SAFE_DELETE_ARRAY(vertex_remap);
  }
#endif

  // Copy data into the buffers
  AnimatedGeometryVertex* vb_vertices = NULL;
  AnimatedGeometryIndex* ib_indices = NULL;
  if (FAILED(vb->Lock(0, 0, (void**)&vb_vertices, 0)) ||
      FAILED(ib->Lock(0, 0, (void**)&ib_indices, 0))) {
    assert(false); // todo: handle gracefully
    SAFE_RELEASE(vb);
    SAFE_RELEASE(ib);
    SAFE_DELETE_ARRAY(subset_vertices);
    SAFE_DELETE_ARRAY(subset_indices);
  }

  memcpy(vb_vertices,
         subset_vertices,
         number_of_subset_vertices * sizeof(AnimatedGeometryVertex));
  memcpy(ib_indices,
         subset_indices,
         number_of_subset_indices * sizeof(AnimatedGeometryIndex));

  vb->Unlock();
  ib->Unlock();

  SAFE_DELETE_ARRAY(subset_vertices);
  SAFE_DELETE_ARRAY(subset_indices);

  // Save into the subset
  texture_group_subset->vb = vb;
  texture_group_subset->ib = ib;
  texture_group_subset->number_of_vertices = number_of_subset_vertices;
  assert((number_of_subset_indices % 3) == 0);
  texture_group_subset->number_of_triangles = number_of_subset_indices / 3;
  texture_group_subset->matrix_bone_indices = matrix_bone_indices;
  texture_group_subset->number_of_matrices = number_of_subset_matrices;

  // Success
  return true;
}



//----[  AnimatedMeshUtil_CreateRenderableAnimatedMesh  ]----------------------
// Translates source animated mesh data into an animated mesh that can be
// rendered with the given device.
bool AnimatedMeshUtil_CreateRenderableAnimatedMesh(
    LPDIRECT3DDEVICE9 d3d_device,
    const AnimatedMeshData* animated_mesh_data,
    RenderableAnimatedMesh* renderable_animated_mesh) {
  assert(d3d_device);
  assert(animated_mesh_data);
  assert(renderable_animated_mesh);
  assert(animated_mesh_data->frames);
  size_t number_of_frames = animated_mesh_data->number_of_frames;
  assert(number_of_frames > 0);
  assert(animated_mesh_data->bones);
  size_t number_of_bones = animated_mesh_data->number_of_bones;
  assert(number_of_bones > 0);
  assert(animated_mesh_data->texture_groups);
  size_t number_of_texture_groups
    = animated_mesh_data->number_of_texture_groups;
  assert(number_of_texture_groups > 0);

  // Allocate the core structures
  renderable_animated_mesh->frames
    = new RenderableAnimatedMesh::Frame[number_of_frames];
  renderable_animated_mesh->number_of_frames = number_of_frames;
  renderable_animated_mesh->bones
    = new RenderableAnimatedMesh::Bone[number_of_bones];
  renderable_animated_mesh->number_of_bones = number_of_bones;
  renderable_animated_mesh->texture_groups
    = new RenderableAnimatedMesh::TextureGroup[number_of_texture_groups];
  renderable_animated_mesh->number_of_texture_groups
    = number_of_texture_groups;

  if (!renderable_animated_mesh->frames ||
      !renderable_animated_mesh->bones ||
      !renderable_animated_mesh->texture_groups) {
    assert(false);
    AnimatedMeshUtil_DeallocateRenderableAnimatedMesh(
      renderable_animated_mesh);
    return false;
  }

  // Copy frame data
  AnimatedMeshUtil_CopyDataFramesToRenderableFrames(
    animated_mesh_data->frames,
    number_of_frames,
    renderable_animated_mesh->frames);

  // Copy bone data
  AnimatedMeshUtil_CopyDataBonesToRenderableBones(
    animated_mesh_data->bones,
    number_of_bones,
    renderable_animated_mesh->frames,
    number_of_frames,
    renderable_animated_mesh->bones);

  // Get the device's capabilities so that we can break up the mesh's texture
  // groups into chunks that use fewer than the maximum number of blend matrix
  // indices.
  D3DCAPS9 dev_caps;
  if (FAILED(d3d_device->GetDeviceCaps(&dev_caps))) {
    assert(false);
    return false;
  }

  // D3DCAPS9.MaxVertexBlendMatrixIndex (from the SDK docs):
  //  DWORD value that specifies the maximum matrix index that can be indexed
  //  into using the per-vertex indices. The number of matrices is
  //  MaxVertexBlendMatrixIndex + 1, which is the size of the matrix palette.
  //  If normals are present in the vertex data that needs to be blended for
  //  lighting, then the number of matrices is half the number specified by
  //  this capability flag. If MaxVertexBlendMatrixIndex is set to zero, the
  //  driver does not support indexed vertex blending. If this value is not
  //  zero then the valid range of indices is zero through
  //  MaxVertexBlendMatrixIndex.
  size_t max_simultaneous_bones = (dev_caps.MaxVertexBlendMatrixIndex+1)>>1;

  // Algorithm:
  //  NOTE: FOR ANY USEFUL RESULTS, THE VERTICES MUST BE REASONABLY WELL
  //  SORTED BY WHICH BONES THEY REFERENCE.
  //  Scan triples from the list of bone indices, adding each referenced
  //   vertex's bone-matrix indices to a set until doing so would cause the
  //   set to be larger than max_simultaneous_bones.
  //  Create a two lookup tables: a temporary one that turns original bone
  //   matrix index into subset matrix index (size is # of bones in mesh so
  //   that a straight array-lookup can be used) and its inverse.  The latter
  //   is saved as the matrix_bone_indices member of the subset, because it specifies
  //   which bone matrices are to be used for each world matrix when rendering
  //   this subset.
  //  Create a vertex remap from original vertex index to subset vertex index
  //  Allocate vertex and index buffers
  //  Copy vertices into the vertex buffer.  With the first (temporary)
  //   lookup table, translate the source's bone index references.
  //  Copy indices into the index buffer.
  //  Create a subset structure and add it to a temporary subset list
  //  Repeat above until all indices have been processed.
  //  Allocate the list of subsets for this texture group and save into the
  //    output structure.  Clear the temporary subset lists.
  unsigned int* bone_to_subset_matrix = new unsigned int[number_of_bones];
  static const unsigned int MAX_NUMBER_OF_SUBSETS = 64;
  RenderableAnimatedMesh::TextureGroupSubset subsets[MAX_NUMBER_OF_SUBSETS];
  size_t current_number_of_subsets = 0;
  for (size_t texture_group = 0;
              texture_group < number_of_texture_groups;
            ++texture_group) {
    AnimatedMeshData::TextureGroup* source_texture_group
      = &animated_mesh_data->texture_groups[texture_group];
    AnimatedGeometryVertex* vertices = source_texture_group->vertices;
    size_t number_of_vertices = source_texture_group->number_of_vertices;
    AnimatedGeometryIndex* indices = source_texture_group->indices;
    size_t number_of_indices = source_texture_group->number_of_indices;

    // translation table used per-subset
    unsigned int* source_vertex_to_subset_vertex
      = new unsigned int[number_of_vertices];
    unsigned int subset_first_index_index = 0;
    std::set<unsigned int> subset_bone_indices;
    std::set<unsigned int> subset_vertex_indices;
    for (size_t index = 0; index <= number_of_indices; index += 3) {
      bool terminating_group = index == number_of_indices;

      std::set<unsigned int> triangle_bone_indices;
      AnimatedGeometryIndex triangle_indices[3] = { -1, -1, -1 };
      if (!terminating_group) {
        triangle_indices[0] = indices[index+0];
        triangle_indices[1] = indices[index+1];
        triangle_indices[2] = indices[index+2];

        assert(triangle_indices[0] < number_of_vertices);
        assert(triangle_indices[1] < number_of_vertices);
        assert(triangle_indices[2] < number_of_vertices);

        AnimatedGeometryVertex* triangle_vertices[] = {
          &vertices[triangle_indices[0]],
          &vertices[triangle_indices[1]],
          &vertices[triangle_indices[2]],
        };
        triangle_bone_indices.insert(triangle_vertices[0]->matrix_indices[0]);
        triangle_bone_indices.insert(triangle_vertices[0]->matrix_indices[1]);
        triangle_bone_indices.insert(triangle_vertices[0]->matrix_indices[2]);
        triangle_bone_indices.insert(triangle_vertices[0]->matrix_indices[3]);
        triangle_bone_indices.insert(triangle_vertices[1]->matrix_indices[0]);
        triangle_bone_indices.insert(triangle_vertices[1]->matrix_indices[1]);
        triangle_bone_indices.insert(triangle_vertices[1]->matrix_indices[2]);
        triangle_bone_indices.insert(triangle_vertices[1]->matrix_indices[3]);
        triangle_bone_indices.insert(triangle_vertices[2]->matrix_indices[0]);
        triangle_bone_indices.insert(triangle_vertices[2]->matrix_indices[1]);
        triangle_bone_indices.insert(triangle_vertices[2]->matrix_indices[2]);
        triangle_bone_indices.insert(triangle_vertices[2]->matrix_indices[3]);
      }

      // Add bones from the subset being built
      std::set<unsigned int> combined_bone_indices;
      combined_bone_indices.insert(triangle_bone_indices.begin(),
                                   triangle_bone_indices.end());
      combined_bone_indices.insert(subset_bone_indices.begin(),
                                   subset_bone_indices.end());


      // Check conditions to close this subset
      if ((terminating_group && !combined_bone_indices.empty()) ||
          (combined_bone_indices.size() > max_simultaneous_bones)) {
        assert(current_number_of_subsets < MAX_NUMBER_OF_SUBSETS);

        if (!AnimatedMeshUtil_CreateTextureGroupSubset(
            d3d_device,
            vertices,
            number_of_vertices,
            indices,
            number_of_indices,
            subset_vertex_indices,
            subset_bone_indices,
            subset_first_index_index,
            index,                    // bound the indices in this subset here
            &subsets[current_number_of_subsets++],
            bone_to_subset_matrix,
            source_vertex_to_subset_vertex)) {
          assert(false); // TODO: clean up
        }

        // Initialize the next subset
        subset_first_index_index = index;
        subset_vertex_indices.clear();
        // This gets swapped into the subset bone indices below
        combined_bone_indices.swap(triangle_bone_indices);
      }

      // Add this triangle to the list
      subset_vertex_indices.insert(triangle_indices[0]);
      subset_vertex_indices.insert(triangle_indices[1]);
      subset_vertex_indices.insert(triangle_indices[2]);
      subset_bone_indices.swap(combined_bone_indices);
    }

    // Free this temporary buffer
    SAFE_DELETE_ARRAY(source_vertex_to_subset_vertex);

    // This texture group is done being split into subsets, so generate
    // the data for it.
    RenderableAnimatedMesh::TextureGroup* renderable_texture_group
      = &renderable_animated_mesh->texture_groups[texture_group];
    renderable_texture_group->subsets
      = new RenderableAnimatedMesh::TextureGroupSubset[current_number_of_subsets];
    memcpy(renderable_texture_group->subsets,
           subsets,
           sizeof(RenderableAnimatedMesh::TextureGroupSubset)
             * current_number_of_subsets);
    renderable_texture_group->number_of_subsets
      = current_number_of_subsets;

    // Set up for the next group
    current_number_of_subsets = 0;
  }
  SAFE_DELETE_ARRAY(bone_to_subset_matrix);

  return true;
}



//----[  AnimatedMeshUtil_DeallocateRenderableAnimatedMesh  ]------------------
void AnimatedMeshUtil_DeallocateRenderableAnimatedMesh(
    RenderableAnimatedMesh* renderable_animated_mesh) {
  assert(renderable_animated_mesh);
  SAFE_DELETE_ARRAY(renderable_animated_mesh->frames);
  SAFE_DELETE_ARRAY(renderable_animated_mesh->bones);

  for (size_t i = 0; i < renderable_animated_mesh->number_of_texture_groups; ++i) {
    RenderableAnimatedMesh::TextureGroup* texture_group
      = &renderable_animated_mesh->texture_groups[i];
    for (size_t j = 0; j < texture_group->number_of_subsets; ++j) {
      SAFE_RELEASE(texture_group->subsets[j].vb);
      SAFE_RELEASE(texture_group->subsets[j].ib);
      SAFE_DELETE_ARRAY(texture_group->subsets[j].matrix_bone_indices);
    }
    SAFE_DELETE_ARRAY(texture_group->subsets);
  }
  SAFE_DELETE_ARRAY(renderable_animated_mesh->texture_groups);

  renderable_animated_mesh->number_of_frames = 0;
  renderable_animated_mesh->number_of_bones = 0;
  renderable_animated_mesh->number_of_texture_groups = 0;
}


//----[  AnimatedMeshUtil_InterpretUnseenSkeletalAnimation  ]------------------
bool AnimatedMeshUtil_InterpretUnseenSkeletalAnimation(
    const UnseenSkeletalAnimation* input_source_animation,
    AnimatedMeshAnimation* output_animation) {
  assert(input_source_animation);
  assert(output_animation);

  // Copy frame values
  size_t number_of_total_frames
    = input_source_animation->info.numberOfSkeletalFrames;
  output_animation->number_of_total_frames = number_of_total_frames;
  size_t number_of_animated_frames
    = input_source_animation->info.numberOfAnimatedFrames;
  output_animation->number_of_animated_frames = number_of_animated_frames;

  // Create and copy the animated frame -> skeletal frame translation table
  output_animation->animated_frame_to_frame_table
    = new unsigned int[number_of_animated_frames];
  assert(output_animation->animated_frame_to_frame_table);
  if (!output_animation->animated_frame_to_frame_table) return false;
  for (size_t i = 0; i < number_of_animated_frames; ++i) {
    unsigned int frame_index
      = AnimatedMeshAnimation::INVALID_FRAME_TRANSFORM_INDEX;
    for (size_t j = 0; j < number_of_total_frames; ++j) {
      if (input_source_animation
           ->frameIndexToAnimatedFrameTransformTable[j] == i) {
        frame_index = j;
        break;
      }
    }
    assert(frame_index
           != AnimatedMeshAnimation::INVALID_FRAME_TRANSFORM_INDEX);
    output_animation->animated_frame_to_frame_table[i] = frame_index;
  }

  // Copy frame transforms.  This block allocates all of the transforms for
  // the animation, then decomposes the source matrix-keys into the scaling,
  // rotation and translation components used by the new animation engine.
  typedef AnimatedMeshAnimation::AnimatedFrameTransform FrameTransform;
  size_t number_of_keys
    = input_source_animation->info.numberOfKeyframes;
  output_animation->number_of_keys = number_of_keys;
  output_animation->animated_frame_transforms
    = new FrameTransform[number_of_keys * number_of_animated_frames];
  assert(output_animation->animated_frame_transforms);
  if (!output_animation->animated_frame_transforms) {
    SAFE_DELETE(output_animation->animated_frame_to_frame_table);
    return false;
  }
  assert(output_animation->animated_frame_transforms);
  for (size_t key_index = 0; key_index < number_of_keys; ++key_index) {
    FrameTransform* key_frame_transforms
      = output_animation->animated_frame_transforms
        + key_index * number_of_animated_frames;
    for (size_t frame_index = 0;
                frame_index < number_of_animated_frames;
              ++frame_index) {
      const D3DXMATRIX* source_matrix
        = &input_source_animation
            ->keyframes[key_index]
            .animatedFrameTransforms[frame_index];
      FrameTransform* frame_transform = key_frame_transforms + frame_index;
      D3DXMatrixDecompose(&frame_transform->scaling,
                          &frame_transform->rotation,
                          &frame_transform->translation,
                          source_matrix);
    }
  }

  return true;
}


//----[  AnimatedMeshUtil_DeallocateRenderableAnimatedMesh  ]------------------
void AnimatedMeshUtil_DeallocateAnimatedMeshAnimation(
    AnimatedMeshAnimation* animation) {
  SAFE_DELETE_ARRAY(animation->animated_frame_transforms);
  SAFE_DELETE_ARRAY(animation->animated_frame_to_frame_table);
  memset(animation, 0, sizeof(AnimatedMeshAnimation));
}




//----[  BufferSerializer  ]---------------------------------------------------
// Borrowed from the game file serializer
typedef BufferSerializer<
  size_t,                       // total number of frames
  Array<unsigned int, size_t>,  // animated frame to frame table (& # of frames)
  Array<Evidyon::AnimatedMesh::AnimatedMeshAnimation::AnimatedFrameTransform,
        size_t> // animated frame transforms
  // NOTE: # of keys = # of animated frame transforms / # of animated frames
  > AnimatedMeshAnimationBufferSerializer;


//----[  AnimatedMeshUtil_Serialize  ]-----------------------------------------
size_t AnimatedMeshUtil_Serialize(
    AnimatedMeshAnimation* animation_in,
    char* buffer,
    const char* buffer_end) {
  char* next = AnimatedMeshAnimationBufferSerializer::writeBuffer(
    buffer,
    buffer_end,
    animation_in->number_of_total_frames,
    &CopiedArray<unsigned int>(
      animation_in->animated_frame_to_frame_table,
      animation_in->number_of_animated_frames),
    &CopiedArray<Evidyon::AnimatedMesh::AnimatedMeshAnimation::AnimatedFrameTransform>(
      animation_in->animated_frame_transforms,
      animation_in->number_of_keys * animation_in->number_of_animated_frames));
  return next - buffer;
}



//----[  AnimatedMeshUtil_Deserialize  ]---------------------------------------
bool AnimatedMeshUtil_Deserialize(
        char* buffer,
        const char* buffer_end,
        Evidyon::AnimatedMesh::AnimatedMeshAnimation* animation_out) {

  using namespace Evidyon::AnimatedMesh;
  size_t number_of_total_frames;
  size_t number_of_animated_frames;
  size_t number_of_animated_frame_transforms;
  AnimatedMeshAnimation description;
  AnimatedMeshAnimationBufferSerializer::readBuffer(
    buffer,
    buffer_end,
    &number_of_total_frames,
    &PeekArray<unsigned int>(&number_of_animated_frames),
    &PeekArray<AnimatedMeshAnimation::AnimatedFrameTransform>(
      &number_of_animated_frame_transforms));

  unsigned int* animated_frame_to_frame_table 
    = new unsigned int[number_of_animated_frames];
  AnimatedMeshAnimation::AnimatedFrameTransform* animated_frame_transforms
    = new AnimatedMeshAnimation::AnimatedFrameTransform
            [number_of_animated_frame_transforms];

  const char* next = AnimatedMeshAnimationBufferSerializer::readBuffer(
    buffer,
    buffer_end,
    &number_of_total_frames,
    &CopiedArray<unsigned int>(animated_frame_to_frame_table,
                               number_of_animated_frames,
                               &description.number_of_animated_frames),
    &CopiedArray<AnimatedMeshAnimation::AnimatedFrameTransform>(
      animated_frame_transforms,
      number_of_animated_frame_transforms,
      &description.number_of_keys));

  description.number_of_keys /= number_of_animated_frames;
  description.number_of_total_frames = number_of_total_frames;
  description.animated_frame_to_frame_table = animated_frame_to_frame_table;
  description.animated_frame_transforms = animated_frame_transforms;

  memcpy(animation_out, &description, sizeof(description));

  return next != 0;
}


}
}
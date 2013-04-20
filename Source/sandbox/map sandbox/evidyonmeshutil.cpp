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
#include "evidyonmeshutil.h"
#include <d3dx9.h>
#include <assert.h>
#include "../common/safemacros.h"
#include <dc/debug>
#include <set>
#include <map>

namespace Evidyon {
namespace Meshes {


typedef std::map<GeometryIndex,GeometryIndex> XIndicesTable;
typedef XIndicesTable::value_type XIndicesEntry;
typedef XIndicesTable::const_iterator XIndicesIterator;
typedef std::pair<XIndicesTable::iterator,bool> XIndicesTableInsertResult;

struct Triangle
{
    GeometryIndex index[3];
};

typedef std::vector<Triangle> TriangleList;
typedef TriangleList::value_type TriangleEntry;
typedef TriangleList::const_iterator TriangleIterator;

bool EvidyonMeshUtil_CreateMeshFromD3DXMeshWorker(LPD3DXMESH d3dx_mesh,
                                                  LPDIRECT3DDEVICE9 d3d_device,
                                                  DWORD subsets,
                                                  float adjacency_epsilon,
                                                  Mesh* mesh) {
  assert(d3dx_mesh);
  assert(mesh);

  // Add a reference to the mesh to counteract freeing it at the end.  The
  // freeing is necessary so that we can safely swap in a different mesh
  // if the format doesn't match the geometry FVF.
  d3dx_mesh->AddRef();

  // Allocate subsets
  mesh->subsets = new Mesh::Subset[subsets];
  memset(mesh->subsets, 0, sizeof(Mesh::Subset)*subsets);
  mesh->number_of_subsets = subsets;

  // If this mesh isn't already in the correct format, have D3D do the grunt work of
  // converting it.
  bool generate_normals = false; // Whether or not normals need to be generated for this mesh
  if ((d3dx_mesh->GetFVF() != D3DFVF_EVIDYON_GEOMETRYVERTEX) ||
      (D3DFMT_EVIDYON_GEOMETRYINDEX == D3DFMT_INDEX32) && ((d3dx_mesh->GetOptions() & D3DXMESH_32BIT) == 0))
  {
    // Holds the mesh when its converted to the correct format
    LPD3DXMESH pTempD3DXMesh = NULL;

    // Duplicate the loaded mesh into the format
    bool is_32bit = D3DFMT_EVIDYON_GEOMETRYINDEX == D3DFMT_INDEX32;
    if (APP_ERROR(d3dx_mesh->CloneMeshFVF(
                        D3DXMESH_SYSTEMMEM | (is_32bit ? D3DXMESH_32BIT : 0),
                        D3DFVF_EVIDYON_GEOMETRYVERTEX, d3d_device, &pTempD3DXMesh))
                 ("XMesh couldn't convert the source geometry format")) {
      return false;
    }

    // Generate normals if they didn't exist
    generate_normals = ((d3dx_mesh->GetFVF()&D3DFVF_NORMAL)!=D3DFVF_NORMAL &&
                        (D3DFMT_EVIDYON_GEOMETRYINDEX&D3DFVF_NORMAL)!=D3DFVF_NORMAL);

    // Use this mesh instead
    d3dx_mesh->Release();
    d3dx_mesh = pTempD3DXMesh;
  }

  // The mesh must have its attributes sorted before it can be converted to single strips
  {
    // Allocate an adjacency buffer
    DWORD faces = d3dx_mesh->GetNumFaces();
    DWORD* pAdjacency = new DWORD[faces * 3];
    bool failed = false;

    if (APP_ERROR(FAILED(d3dx_mesh->GenerateAdjacency(adjacency_epsilon, pAdjacency)))("Unable to generate the mesh adjacency"))
      failed = true;

    { // Clean up "bowties" in the mesh that prevent lighting from being calculated correctly
      LPD3DXMESH cleaned_mesh = NULL;
      DWORD* cleaned_adjacency = new DWORD[faces * 3];
      LPD3DXBUFFER errors_and_warnings = NULL;
      if (!failed && APP_ERROR(FAILED(D3DXCleanMesh(D3DXCLEAN_BOWTIES,
                                                    d3dx_mesh,
                                                    pAdjacency,
                                                    &cleaned_mesh,
                                                    cleaned_adjacency,
                                                    &errors_and_warnings)))
                              ("Failed to clean mesh")) {
        failed = true;
        if (errors_and_warnings) {
          DEBUG_ERROR("Mesh cleaning error:  %s", (const char*)errors_and_warnings->GetBufferPointer());
        }
      }

      SAFE_RELEASE(errors_and_warnings);

      // If we successfully cleaned the mesh, use the new mesh and new set of
      // adjacencies.  Otherwise, just delete anything that was allocated and
      // keep the original.
      if (failed) {
        SAFE_DELETE_ARRAY(cleaned_adjacency);
        SAFE_RELEASE(cleaned_mesh);
      } else {
        SAFE_DELETE_ARRAY(pAdjacency);
        SAFE_RELEASE(d3dx_mesh)
        pAdjacency = cleaned_adjacency;
        d3dx_mesh = cleaned_mesh;
      }
    }

    // Compute mesh normals, if necessary
    if (!failed && generate_normals && APP_ERROR(FAILED(D3DXComputeNormals(d3dx_mesh, pAdjacency)))("Couldn't generate mesh normals")) {
      failed = true;
    }

    // Optimize the mesh
    if (!failed && APP_ERROR(FAILED(d3dx_mesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT,
                                                              pAdjacency,
                                                              NULL,
                                                              NULL,
                                                              NULL)))
                             ("Couldn't optimize mesh attributes")) {
	    failed = true;
    }

    // Get rid of the temporary adjacency buffer
    SAFE_DELETE_ARRAY(pAdjacency);

    // Return if there was an error
    if (failed) {
      SAFE_RELEASE(d3dx_mesh);
      return false;
    }
  }

  // Lock the vertex buffer
  GeometryVertex* pXVertices = NULL;
  if (APP_ERROR(d3dx_mesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pXVertices))("Couldn't lock source vertex buffer"))
  {
	  // Failure
    SAFE_RELEASE(d3dx_mesh);
	  return false;
  }

  // Iterate through all of the materials and copy vertex/index data, and assign material
  // information for the mesh.
  for (DWORD subset = 0; subset < subsets; subset++) {

    //TODO: D3DXOptimizeFaces, D3DXOptimizeVertices

    // Use D3DX to convert this subset into a nicely indexed form
    DWORD numStripIndices;
    LPDIRECT3DINDEXBUFFER9 pSubsetIB;
    if (APP_ERROR(D3DXConvertMeshSubsetToSingleStrip(d3dx_mesh,
                                                     subset,
                                                     D3DXMESH_SYSTEMMEM,
                                                     &pSubsetIB,
                                                     &numStripIndices))("Couldn't convert mesh subset into indexable strip"))
    {
      d3dx_mesh->UnlockVertexBuffer();
      SAFE_RELEASE(d3dx_mesh);
      return false;
    }

    D3DINDEXBUFFER_DESC desc;
    GeometryIndex* pXIndices = NULL;

    // Check the format of the indices and lock the strip index buffer
    if (APP_ERROR(pSubsetIB->GetDesc(&desc))("Couldn't get .X mesh IB desc") || (desc.Format != D3DFMT_EVIDYON_GEOMETRYINDEX) ||
        APP_ERROR(pSubsetIB->Lock(0, 0, (VOID**)&pXIndices, D3DLOCK_READONLY))("Unable to lock the .X index buffer"))
    {
      pSubsetIB->Release();
      d3dx_mesh->UnlockVertexBuffer();
      SAFE_RELEASE(d3dx_mesh);
      return false;
    }

    // This table pairs an index from the .X file to an index in the buffer that
    // holds the vertices for this subset
    XIndicesTable xIndicesTable;

    // For each of the indices in the strip, puts its vertex ID into the indices
    // table.  Use the counter to determine which vertex this is.
    {
      GeometryIndex vertexCounter = 0;
      for (DWORD e = 0; e < numStripIndices; ++e)
      {
        // Insert the entry [x-mesh index, subset index] into the table
        XIndicesTableInsertResult result = xIndicesTable.insert(XIndicesEntry(pXIndices[e], vertexCounter));

        // If the result was successful (this isn't a duplicated X-mesh index) increment the vertex counter
        if (result.second)
            vertexCounter++;
      }
    }

    // Grab the number of vertices this geometry uses
    DWORD numVertices = (DWORD)xIndicesTable.size();

    // This buffer holds all of the triangles in this subset
    TriangleList triangles;

    // This list keeps track of locations in the strip where the winding order changes.  This is necessary
    // because this next part will remove degenerate triangles from the list.
    std::set<size_t> windingChanges;

    // Generate the list of triangles from the strip provided
    for (DWORD t = 0; t < numStripIndices - 2; ++t)
    {
      // Build the triangle that will be added to the buffer
      Triangle tri = { pXIndices[t + 0], pXIndices[t + 2], pXIndices[t + 1] };

      // Convert the triangle into subset-indices by using the lookup table
      // we generated before.
      tri.index[0] = xIndicesTable.find(tri.index[0])->second;
      tri.index[1] = xIndicesTable.find(tri.index[1])->second;
      tri.index[2] = xIndicesTable.find(tri.index[2])->second;

      // Check to make sure this triangle isn't degenerate.  If it is, we can just skip
      // this triangle entirely to simplify the geometry.
      if (tri.index[0] == tri.index[1] || tri.index[1] == tri.index[2] || tri.index[0] == tri.index[2])
      {
        // Try to find the winding in the list
        std::set<size_t>::iterator currentWinding = windingChanges.find(triangles.size());

        // Add this to the winding change list, or remove the change if it's already there
        if (currentWinding != windingChanges.end())
            windingChanges.erase(currentWinding);
        else
            windingChanges.insert(triangles.size());

        // Don't insert a triangle here
        continue;
      }

      // Add this triangle to the list
      triangles.push_back(tri);
    }

    // Calculate the number of indices we need for the buffer
    DWORD numGeometryIndices = (DWORD)(triangles.size() * 3);

    GeometryVertex* subset_vertices = new GeometryVertex[numVertices];
    GeometryIndex* subset_indices = new GeometryIndex[numGeometryIndices];
    if (!subset_vertices || !subset_indices) {
      SAFE_DELETE_ARRAY(subset_vertices);
      SAFE_DELETE_ARRAY(subset_indices);
      pSubsetIB->Unlock();
      SAFE_RELEASE(pSubsetIB);
      d3dx_mesh->UnlockVertexBuffer();
      SAFE_RELEASE(d3dx_mesh);
      return false;
    }

    // Copy the vertices needed for this subset into the buffer
    for (XIndicesIterator i = xIndicesTable.begin(); i != xIndicesTable.end(); ++i)
    {
      GeometryVertex* pCurrentVertex = &subset_vertices[i->second];
      *pCurrentVertex = pXVertices[i->first];
      pCurrentVertex->diffuse = D3DCOLOR_XRGB(255,255,255);
    }

    // Copy triangles into the indices buffer
    DWORD index = 0;
    DWORD windingOrder = 0;
    for (TriangleIterator t = triangles.begin(); t != triangles.end(); ++t)
    {
      // Find this index in the winding list
      if (windingChanges.find(index / 3) != windingChanges.end())
          windingOrder = 1 - windingOrder;

      // Alternate the winding order so that everything shows up correctly
      if ((index / 3) % 2 == windingOrder)
      {
        subset_indices[index + 0] = t->index[0];
        subset_indices[index + 1] = t->index[1];
        subset_indices[index + 2] = t->index[2];
      }
      else
      {
        subset_indices[index + 0] = t->index[1];
        subset_indices[index + 1] = t->index[0];
        subset_indices[index + 2] = t->index[2];
      }

      // Increment the index counter
      index += 3;
    }

    // Unlock and delete strip index buffer
    pSubsetIB->Unlock();
    SAFE_RELEASE(pSubsetIB);

    mesh->subsets[subset].vertices = subset_vertices;
    mesh->subsets[subset].number_of_vertices = numVertices;
    mesh->subsets[subset].indices = subset_indices;
    mesh->subsets[subset].number_of_indices = numGeometryIndices;
  }

  // Done with the DirectX mesh
  d3dx_mesh->UnlockVertexBuffer();
  SAFE_RELEASE(d3dx_mesh);

  // Success
  return true;
}





//----[  EvidyonMeshUtil_CreateMeshFromD3DXMesh  ]-----------------------------
bool EvidyonMeshUtil_CreateMeshFromD3DXMesh(LPD3DXMESH d3dx_mesh,
                                            DWORD subsets,
                                            Mesh* mesh) {

  // Get the device and pass it to the worker method
  LPDIRECT3DDEVICE9 d3d_device = NULL;
  d3dx_mesh->GetDevice(&d3d_device);
  bool result = EvidyonMeshUtil_CreateMeshFromD3DXMeshWorker(d3dx_mesh,
                                                             d3d_device,
                                                             subsets,
                                                             0.001f, // adjacency epsilon
                                                             mesh);
  SAFE_RELEASE(d3d_device);

  // Free the mesh data if the mesh didn't completely convert
  if (!result) {
    for (MeshSubsetIndex i = 0; i < mesh->number_of_subsets; ++i) {
      SAFE_DELETE_ARRAY(mesh->subsets[i].vertices);
      SAFE_DELETE_ARRAY(mesh->subsets[i].indices);
    }
    SAFE_DELETE_ARRAY(mesh->subsets);
    mesh->number_of_subsets = 0;

    // Failed to load the mesh
    return false;
  }

  // Return the output of the operation
  return result;
}


}
}
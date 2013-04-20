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
#include "stdafx.h"
#include "mesh.h"
#include "xmesh.h"



/// Used when calculating adjacency values during the load method
#define ADJACENCY_EPSILON 0.001f

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


/*#define APP_ERROR(a, b) false
#define DEBUG_MSG(a, b)
#define DX_FAILED(a) (hr = FAILED(a))
*/

/// Resolve this class's implementation linker
dcClass<XMesh>::Implements<Mesh> implementsMesh;



//------------------------------------------------------------------------------------------------
// Name:  XMesh
// Desc:  Constructs the subset geometry for a D3DXMesh
//------------------------------------------------------------------------------------------------
bool XMesh::buildGeometryFromD3DXMesh(LPD3DXMESH d3dxMesh, SubsetGeometry* subsetGeometry, DWORD subsets)
{
    // Check parameters
    if (APP_ERROR(!d3dxMesh || !subsetGeometry)("Invalid parameter to XMesh::buildGeometryFromD3DXMesh"))
        return false;

    // Add a reference to the mesh to counteract freeing it at the end
    d3dxMesh->AddRef();

    // Get the device
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;
    d3dxMesh->GetDevice(&pd3dDevice);

    // If this mesh isn't already in the correct format, have D3D do the grunt work of
    // converting it.
    bool generate_normals = false; // Whether or not normals need to be generated for this mesh
    if ((d3dxMesh->GetFVF() != D3DFVF_GEOMETRYVERTEX) ||
        (D3DFMT_GEOMETRYINDEX == D3DFMT_INDEX32) && ((d3dxMesh->GetOptions() & D3DXMESH_32BIT) == 0))
    {
        // Holds the mesh when its converted to the correct format
        LPD3DXMESH pTemd3dxMesh = NULL;

        // Duplicate the loaded mesh into the format
        if (APP_ERROR(d3dxMesh->CloneMeshFVF(
                            D3DXMESH_SYSTEMMEM | ((D3DFMT_GEOMETRYINDEX == D3DFMT_INDEX32) ? D3DXMESH_32BIT : 0),
                            D3DFVF_GEOMETRYVERTEX, pd3dDevice, &pTemd3dxMesh))
                     ("XMesh couldn't convert the source geometry format")) {
            d3dxMesh->Release();
            pd3dDevice->Release();
			      return false;
        }

        // Generate normals if they didn't exist
        generate_normals = ((d3dxMesh->GetFVF()&D3DFVF_NORMAL)!=D3DFVF_NORMAL &&
                            (D3DFMT_GEOMETRYINDEX&D3DFVF_NORMAL)!=D3DFVF_NORMAL);

        // Use this mesh instead
        d3dxMesh->Release();
        d3dxMesh = pTemd3dxMesh;
    }

    // The mesh must have its attributes sorted before it can be converted to single strips
    {
        // Allocate an adjacency buffer
        DWORD faces = d3dxMesh->GetNumFaces();
        DWORD* pAdjacency = new DWORD[faces * 3];
		    bool failed = false;

        if (APP_ERROR(FAILED(d3dxMesh->GenerateAdjacency(ADJACENCY_EPSILON, pAdjacency)))("Unable to generate the mesh adjacency"))
          failed = true;

        { // Clean up "bowties" in the mesh that prevent lighting from being calculated correctly
          LPD3DXMESH cleaned_mesh = NULL;
          DWORD* cleaned_adjacency = new DWORD[faces * 3];
          LPD3DXBUFFER errors_and_warnings = NULL;
          if (!failed && APP_ERROR(FAILED(D3DXCleanMesh(D3DXCLEAN_BOWTIES,
                                                        d3dxMesh,
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
            SAFE_RELEASE(d3dxMesh)
            pAdjacency = cleaned_adjacency;
            d3dxMesh = cleaned_mesh;
          }
        }

        // Compute mesh normals, if necessary
        if (!failed && generate_normals && APP_ERROR(FAILED(D3DXComputeNormals(d3dxMesh, pAdjacency)))("Couldn't generate mesh normals")) {
          failed = true;
        }

        // Optimize the mesh
        if (!failed && APP_ERROR(FAILED(d3dxMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT,
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
          SAFE_RELEASE(d3dxMesh);
          SAFE_RELEASE(pd3dDevice);
          return false;
        }
    }

    // Lock the vertex buffer
    GeometryVertex* pXVertices = NULL;
    if (APP_ERROR(d3dxMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pXVertices))("Couldn't lock source vertex buffer"))
    {
		// Erase this mesh
        d3dxMesh->Release();
        pd3dDevice->Release();

		// Failure
		return false;
    }

    // Iterate through all of the materials and copy vertex/index data, and assign material
    // information for the mesh.
    for (DWORD subset = 0; subset < subsets; subset++)
    {
        // Use D3DX to convert this subset into a nicely indexed form
        DWORD numStripIndices;
        LPDIRECT3DINDEXBUFFER9 pSubsetIB;
        if (APP_ERROR(D3DXConvertMeshSubsetToSingleStrip(d3dxMesh, subset, D3DXMESH_SYSTEMMEM, &pSubsetIB, &numStripIndices))("Couldn't convert mesh subset into indexable strip"))
        {
            // Erase any geometry we made
            DeallocateGeometry(subsetGeometry);

            // Get rid of the mesh
            d3dxMesh->UnlockVertexBuffer();
            d3dxMesh->Release();

            // Free our device
            pd3dDevice->Release();

            // Return the error
            return false;
        }

        D3DINDEXBUFFER_DESC desc;
        GeometryIndex* pXIndices = NULL;

        // Check the format of the indices and lock the strip index buffer
        if (APP_ERROR(pSubsetIB->GetDesc(&desc))("Couldn't get .X mesh IB desc") || (desc.Format != D3DFMT_GEOMETRYINDEX) ||
            APP_ERROR(pSubsetIB->Lock(0, 0, (VOID**)&pXIndices, D3DLOCK_READONLY))("Unable to lock the .X index buffer"))
        {
            // Erase any geometry we made
            DeallocateGeometry(subsetGeometry);

            // Get rid of the mesh
            pSubsetIB->Release();
            d3dxMesh->UnlockVertexBuffer();
            d3dxMesh->Release();

            // Free our device
            pd3dDevice->Release();

            // Error!
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
            // CHANGED July 25, 2008:  the winding order is wrong here
            //Triangle tri = { pXIndices[t + 0], pXIndices[t + 1], pXIndices[t + 2] };
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

        // Allocate the destination geometry
        Geometry* pGeometry = NULL;
        if (APP_ERROR(AllocateGeometry(numVertices, numGeometryIndices, &pGeometry))("Couldn't allocate geometry"))
        {
            // Erase any geometry we made
            DeallocateGeometry(subsetGeometry);

            // Get rid of the mesh
            pSubsetIB->Unlock();
            pSubsetIB->Release();
            d3dxMesh->UnlockVertexBuffer();
            d3dxMesh->Release();

            // Free our device
            pd3dDevice->Release();

            // Error!
            return false;
        }

        // Copy the vertices needed for this subset into the buffer
        GeometryVertex* pVertices = pGeometry->pVertices;
        for (XIndicesIterator i = xIndicesTable.begin(); i != xIndicesTable.end(); ++i)
        {
            GeometryVertex* pCurrentVertex = &pVertices[i->second];
            *pCurrentVertex = pXVertices[i->first];

            // Modify the vertex location to make this a unit mesh sitting on the X-Z plane
            pCurrentVertex->x = pCurrentVertex->x;
            pCurrentVertex->y = pCurrentVertex->y;
            pCurrentVertex->z = pCurrentVertex->z;

            //pVertices[i->second].color = D3DCOLOR_XRGB(255,255,255);
            // todo: enable color?
        }

        // Copy triangles into the indices buffer
        DWORD index = 0;
        GeometryIndex* pIndices = pGeometry->pIndices;
        DWORD windingOrder = 0;
        for (TriangleIterator t = triangles.begin(); t != triangles.end(); ++t)
        {
            // Find this index in the winding list
            if (windingChanges.find(index / 3) != windingChanges.end())
                windingOrder = 1 - windingOrder;

            // Alternate the winding order so that everything shows up correctly
            if ((index / 3) % 2 == windingOrder)
            {
                pIndices[index + 0] = t->index[0];
                pIndices[index + 1] = t->index[1];
                pIndices[index + 2] = t->index[2];
            }
            else
            {
                pIndices[index + 0] = t->index[1];
                pIndices[index + 1] = t->index[0];
                pIndices[index + 2] = t->index[2];
            }

            // Increment the index counter
            index += 3;
        }

        // Unlock and delete strip index buffer
        pSubsetIB->Unlock();
        pSubsetIB->Release();

        // Store the buffers in the main array
        std::pair<SubsetGeometry::iterator,bool> result =
            subsetGeometry->insert(SubsetGeometry::value_type(subset, pGeometry));

        if (APP_ERROR(!result.second)("Couldn't insert subset geometry into main array for .X mesh"))
        {
            // Get rid of this geometry
            DeallocateGeometry(pGeometry);
            DeallocateGeometry(subsetGeometry);

            // Erase the mesh
            d3dxMesh->UnlockVertexBuffer();
            d3dxMesh->Release();

            // Free our device
            pd3dDevice->Release();

            // Return error
            return false;
        }

        //DEBUG_MSG("Subset %i has %i vertices %i indices (%i polygons)\n", subset, numVertices, numGeometryIndices, numGeometryIndices / 3);
    }

    // Done with the DirectX mesh.  This will not erase the outside mesh.
    d3dxMesh->UnlockVertexBuffer();
    d3dxMesh->Release();

    // Free the device reference
    pd3dDevice->Release();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  XMesh
// Desc:  Initializes this class with the given name
//------------------------------------------------------------------------------------------------
XMesh::XMesh()
{
    member("Source File", &mySourceFile);
}



//------------------------------------------------------------------------------------------------
// Name:  obtainSourceGeometry
// Desc:  Loads geometry from the source file into the output subset geometry
//------------------------------------------------------------------------------------------------
bool XMesh::obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const
{
    // Fail without a device or if something is wrong with the output pointer
    if (APP_ERROR(!pd3dDevice || !subsetGeometry)("Invalid paramter to obtainSourceGeometry") ||
		APP_ERROR(!subsetGeometry->empty())("Provided geometry subset for obtainSourceGeometry must be empty"))
		return false;

    // Check to make sure a file exists (if not, just exit)
    if (mySourceFile.getValue().empty()) return true;

    // Keeps track of how many subsets this mesh contains
    DWORD subsets = 0;

    // Stores the mesh that was loaded from the file
    LPD3DXMESH pXMesh = NULL;

    // Load the mesh from the specified file
    if (APP_ERROR(D3DXLoadMeshFromX(mySourceFile.getValue().c_str(), D3DXMESH_SYSTEMMEM, 
                                   pd3dDevice, NULL, NULL, NULL, &subsets, 
                                   &pXMesh))("XMesh couldn't load \"%s\"", mySourceFile.getValue().c_str()))
		return false;

    // Convert the mesh
    bool succeeded = buildGeometryFromD3DXMesh(pXMesh, subsetGeometry, subsets);

    // Release the mesh
    pXMesh->Release();

    // Return an error code if an error occurred
    if (APP_ERROR(!succeeded)("Unable to build geometry for mesh from \"%s\"", mySourceFile.getValue().c_str()))
        return false;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string XMesh::staticTypeName()
{
    return "DirectX (.X) Mesh";
}

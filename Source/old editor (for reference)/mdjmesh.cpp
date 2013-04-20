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
#include "mdjmesh.h"
#include <set>


// Note: these constants are arbitrary.  The max subsets variable might need to be increased.
#define MAX_MDJ_SUBSETS         8
#define MAX_MDJ_SUBSET_INDEX    255


// This information is only used internally by the loading function, and is read directly
// from the MDJ file
#pragma pack(push, 1)

struct MDJBoneData
{
    DWORD MinAffectedVertexIndex;
};

struct MDJVertex
{
    FLOAT x, y, z;
    DWORD Color;
    DWORD ChildBone;
};

struct MDJQuad
{
    WORD indices[4];
    FLOAT texcoords[8];
    WORD texture, bone, flags; // flags & 128 means it specifies a child bone
};

struct MDJVertexPadded
{
    FLOAT x, y, z;
    DWORD Color;
    DWORD ChildBone;
    BYTE Unknown; // functionality unknown, and is usually 0
};

struct MDJBoneDataPadded
{
    BYTE Unknown; // functionality unknown, and is usually 0
    DWORD MinAffectedVertexIndex;
};

#pragma pack(pop)

struct Triangle
{
    GeometryIndex index[3];
};

typedef std::vector<Triangle> TriangleList;
typedef TriangleList::value_type TriangleEntry;
typedef TriangleList::const_iterator TriangleIterator;


//------------------------------------------------------------------------------------------------
// Name:  LoadMDJMeshData
// Desc:  Loads the bone data and a list quads and vertices from the source file
//------------------------------------------------------------------------------------------------
HRESULT LoadMDJMeshData(const CHAR* FileName, MDJBoneData pBoneData[50], MDJVertex** ppVertices,
                         DWORD* pNumVertices, MDJQuad** ppQuads, DWORD* pNumQuads)
{
    // We have to have all of these
    if (APP_ERROR(!ppVertices || !pNumVertices || !ppQuads || !pNumQuads || !FileName)
		("All parameters to LoadMDJMeshData must be valid"))
        return E_FAIL;

    // Open the file
    FILE* pf = NULL;
    fopen_s(&pf, FileName, "rb");
	if (APP_ERROR(!pf)("Unable to open the given MDJ file"))
		return E_FAIL;

    // Check the file format
    DWORD typeIdentifier = 0;
    fread(&typeIdentifier, sizeof(DWORD), 1, pf);

    // Make sure this type can be handled
    bool paddedHeader, paddedVertices;
    switch(typeIdentifier)
    {
        case 50:    // This is the more simple MDJ file type
            paddedHeader = false;
            paddedVertices = false;
            break;
        case 12865:
            paddedHeader = true;
            paddedVertices = false;
            break;
        case 12866:   // This one has both header and vertices padded for some reason
            paddedHeader = true;
            paddedVertices = true;
            break;

        // We can't process any other types
        default:
            fclose(pf);
            DEBUG_ERROR("MDJ file is missing a valid header identifier");
			return E_FAIL;
    }

    // All of this data is read from the file
    DWORD vertices;
    MDJBoneData boneData[50];
    MDJVertex* pVB;
    DWORD quads;
    MDJQuad* pQB;

    // Get the bone data from the start of the file
    if (!paddedHeader)
    {
        // Read the bone data directly
        fread(boneData, sizeof(MDJBoneData), 50, pf);

    }
    else
    {
        MDJBoneDataPadded boneDataPadded[50];

        // Scan the data from the file
        fread(boneDataPadded, sizeof(MDJBoneDataPadded), 50, pf);
        fseek(pf, 1, SEEK_CUR); // Move past the last padding byte

        // Transfer data
        for (int i = 0; i < 50; ++i)
            boneData[i].MinAffectedVertexIndex = boneDataPadded[i].MinAffectedVertexIndex;
    }

    // Read the number of vertices
    fread(&vertices, sizeof(DWORD), 1, pf);
    vertices++;

    // Allocate the vertex buffer
    pVB = new MDJVertex[vertices];

    // Get the vertices from the file
    if (!paddedVertices)
    {
        // Read the vertices into the buffer
        fread(pVB, sizeof(MDJVertex), vertices, pf);
    }
    else
    {
        // Allocate the temporary buffer
        MDJVertexPadded* pPaddedVB = new MDJVertexPadded[vertices];

        // Read the vertices into the padded buffer
        fread(pPaddedVB, sizeof(MDJVertexPadded), vertices, pf);

        // Convert the vertices into the correct form by copying only the part
        // of the vertex that actually matters.  Because the packing is absolute
        // we can be sure that this will function as intended.
        for (DWORD i = 0; i < vertices; ++i)
            memcpy(&pVB[i], &pPaddedVB[i], sizeof(MDJVertex));

        // Erase the temporary buffer
        delete[] pPaddedVB;
    }

    // Read the number of quads
    fread(&quads, sizeof(DWORD), 1, pf);
    quads++;

    // Allocate the quad buffer
    pQB = new MDJQuad[quads];

    // Get the quads from the file (these are never padded)
    fread(pQB, sizeof(MDJQuad), quads, pf);

    // Close the file
    fclose(pf);

    // Copy data into the output locations
    if (pBoneData) memcpy(pBoneData, boneData, sizeof(boneData));
    *ppVertices = pVB;
    *pNumVertices = vertices;
    *ppQuads = pQB;
    *pNumQuads = quads;

    // Success
    return S_OK;
}


//------------------------------------------------------------------------------------------------
// Name:  FreeMDJMeshData
// Desc:  Frees the data that was allocated using the loading function
//------------------------------------------------------------------------------------------------
VOID FreeMDJMeshData(MDJVertex* pVertices, MDJQuad* pQuads)
{
    // Check these because the program doesn't
    if (pVertices) delete[] pVertices;
    if (pQuads) delete[] pQuads;
}



/// Resolve this class's implementation linker
dcClass<MDJMesh>::Implements<Mesh> implementsMesh;


//------------------------------------------------------------------------------------------------
// Name:  MDJMesh
// Desc:  Initializes this class with the given name
//------------------------------------------------------------------------------------------------
MDJMesh::MDJMesh()
{
    member("Source File", &mySourceFile);
}



//------------------------------------------------------------------------------------------------
// Name:  obtainSourceGeometry
// Desc:  Loads geometry from the source file into the output subset geometry
//------------------------------------------------------------------------------------------------
bool MDJMesh::obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const
{
    // If the source file doesn't exist, just succeed
    if (mySourceFile.getValue().empty()) return false;

    // Check params
    if (APP_ERROR(!subsetGeometry)("Bad parameters to EditableMDJMesh"))
		return false;

    // This data is loaded directly from the file
    DWORD numSourceVertices, numSourceQuads;
    MDJVertex* pVB;
    MDJQuad* pQB;

    // Load this model from the file
    if (APP_ERROR(FAILED(LoadMDJMeshData(mySourceFile.getValue().c_str(), NULL, &pVB, &numSourceVertices, &pQB, &numSourceQuads)))
		("Couln't load source model file"))
		return false;

    // Types used in processing the source MDJ mesh
    typedef std::pair<FLOAT,FLOAT> Texcoord;
    typedef std::map<std::pair<WORD,Texcoord>,SubsetIndex> VertexIndexMap;
    typedef std::set<WORD> MDJSubsetList;
    MDJSubsetList subsets;

    // Add all of the subsets to the set, so that each one is only processed once.
    for (DWORD dw = 0; dw < numSourceQuads; ++dw)
        subsets.insert(pQB[dw].texture);

    // For each of the mesh subsets, look through the quads
    for (MDJSubsetList::iterator s = subsets.begin(); s != subsets.end(); ++s)
    {
        // These vertices and triangles are built ONLY for the subset
        VertexIndexMap vertices;
        DWORD vertexAccumulator = 0;
        TriangleList triangles;

        // For each of the quads, build triangles
        for (DWORD q = 0; q < numSourceQuads; ++q)
        {
            MDJQuad* pQuad = &pQB[q];

            // Make sure this quad is part of the subset
            if (pQuad->texture != (*s)) continue;

            // Build the standard vertices from the quad vertices
            VertexIndexMap::key_type quadVertexValues[4] = {
                VertexIndexMap::key_type(pQuad->indices[0], Texcoord(pQuad->texcoords[0], pQuad->texcoords[1])),
                VertexIndexMap::key_type(pQuad->indices[1], Texcoord(pQuad->texcoords[2], pQuad->texcoords[3])),
                VertexIndexMap::key_type(pQuad->indices[2], Texcoord(pQuad->texcoords[4], pQuad->texcoords[5])),
                VertexIndexMap::key_type(pQuad->indices[3], Texcoord(pQuad->texcoords[6], pQuad->texcoords[7])),
            };

            // Specifies the winding order for the triangles
            const int tris[] = { 0, 1, 2, 0, 2, 3 };

            // Process each triangle in the quad
            for (int i = 0; i < 2; ++i)
            {
                // Get the base index
                int base = i * 3;

                // Build a set of MDJ indices
                WORD mdjIndices[] = { quadVertexValues[tris[base+0]].first,
                                      quadVertexValues[tris[base+1]].first,
                                      quadVertexValues[tris[base+2]].first };

                // Check to make sure the triangle isn't degenerate
                if (mdjIndices[0] != mdjIndices[1] &&
                    mdjIndices[1] != mdjIndices[2] &&
                    mdjIndices[0] != mdjIndices[2] &&
                    quadVertexValues[tris[i*3+0]].first < numSourceVertices &&
                    quadVertexValues[tris[i*3+1]].first < numSourceVertices &&
                    quadVertexValues[tris[i*3+2]].first < numSourceVertices)
                {
                    // The vertices for the triangle being constructed
                    VertexIndexMap::iterator triVertices[3];

                    // Find/add all of the vertices to the map
                    for (int vtx = 0; vtx < 3; ++vtx)
                    {
                        // Get the vertex being processed

                        VertexIndexMap::iterator qvtx = vertices.find(quadVertexValues[tris[i*3+vtx]]);
                        if (qvtx == vertices.end())
                            qvtx = vertices.insert(VertexIndexMap::value_type(quadVertexValues[tris[i*3+vtx]], vertexAccumulator++)).first;

                        // Save the iterator
                        triVertices[vtx] = qvtx;
                    }

                    // Build the triangle
                    Triangle tri;
                    tri.index[0] = triVertices[0]->second;
                    tri.index[1] = triVertices[1]->second;
                    tri.index[2] = triVertices[2]->second;

                    // Add the vertices for this triangle to the list
                    triangles.push_back(TriangleList::value_type(tri));
                }
                else
                {
                    //DEBUG_INFO("Removed degenerate triangle from MDJ file");
                }
            }
        }

        // Calculate the number of indices and vertices we need for the buffer
        DWORD numGeometryVertices = (DWORD)(vertices.size());
        DWORD numGeometryIndices = (DWORD)(triangles.size() * 3);

        // Allocate the destination geometry
        Geometry* pGeometry = NULL;
        if (APP_ERROR(AllocateGeometry(numGeometryVertices, numGeometryIndices, &pGeometry))("Failed to allocate dest geometry for MDJ"))
        {
            // Erase any geometry we made
            DeallocateGeometry(subsetGeometry);

            // Get rid of the mesh
            FreeMDJMeshData(pVB, pQB);

            // Error!
            DEBUG_ERROR("Couldn't allocate geometry for .MDJ subset");
			return false;
        }

        // Copy the vertices needed for this subset into the buffer
        GeometryVertex* pVertices = pGeometry->pVertices;
        for (VertexIndexMap::iterator i = vertices.begin(); i != vertices.end(); ++i)
        {
            MDJVertex* pSrcVtx = &pVB[i->first.first];
            GeometryVertex* pDestVtx = &pVertices[i->second];
            pDestVtx->x = pSrcVtx->x;
            pDestVtx->y = pSrcVtx->y;
            pDestVtx->z = pSrcVtx->z;
            D3DXVECTOR3 normal(pSrcVtx->x, pSrcVtx->y, pSrcVtx->z);
            pDestVtx->u = i->first.second.first;
            pDestVtx->v = i->first.second.second;

            // Generate a mesh normal
            // TODO:  generate a *REAL* mesh normal
            D3DXVec3Normalize(&normal,&normal);
            pDestVtx->nx = normal.x;
            pDestVtx->ny = normal.y;
            pDestVtx->nz = normal.z;
            //pVertices[i->second].color = D3DCOLOR_XRGB(255,255,255);
            // todo: enable color?
        }

        // Copy triangles into the indices buffer
        DWORD index = 0;
        GeometryIndex* pIndices = pGeometry->pIndices;
        for (TriangleIterator t = triangles.begin(); t != triangles.end(); ++t)
        {
            pIndices[index + 0] = t->index[0];
            pIndices[index + 1] = t->index[1];
            pIndices[index + 2] = t->index[2];

            // Increment the index counter
            index += 3;
        }

        // Store the buffers in the main array
        std::pair<SubsetGeometry::iterator,bool> result =
            subsetGeometry->insert(SubsetGeometry::value_type((SubsetIndex)(*s), pGeometry));

        if (!result.second)
        {
            // Get rid of this geometry
            DeallocateGeometry(pGeometry);
            DeallocateGeometry(subsetGeometry);

            // Erase the mesh
            FreeMDJMeshData(pVB, pQB);

            // Return error
            DEBUG_ERROR("Couldn't insert geometry subset in .MDJ mesh table");
			      return false;
        }
    }

    // Erase the info we loaded from the file
    FreeMDJMeshData(pVB, pQB);

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string MDJMesh::staticTypeName()
{
    return ".MDJ Mesh";
}

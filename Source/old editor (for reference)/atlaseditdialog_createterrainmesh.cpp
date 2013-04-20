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
#include <d3dx9.h>
#include "atlasterrainmesh.h"


//------------------------------------------------------------------------------------------------
// Name:  CopyYRotatedGeometry
// Desc:  Copies geometry from one location to another, and rotates it about the Y-axis.  This
//        method returns a pointer to the the next empty vertex in destGeometry
//------------------------------------------------------------------------------------------------
GeometryVertex* CopyYRotatedGeometry(GeometryVertex* destGeometry, const GeometryVertex* sourceGeometry, int vertices, float angle)
{
    // Copy all of the geometry
    memcpy(destGeometry, sourceGeometry, sizeof(GeometryVertex) * vertices);

    // Create the transformation matrix
    D3DXMATRIXA16 matrix;
    D3DXMatrixRotationY(&matrix, -angle); // this has to be negative for it to work

    // Transform this geometry in place
    D3DXVec3TransformCoordArray((D3DXVECTOR3*)destGeometry, sizeof(GeometryVertex),
                           (const D3DXVECTOR3*)destGeometry, sizeof(GeometryVertex), &matrix, vertices);

    for (int i = 0; i < vertices; ++i) {
      destGeometry[i].nx =  0.0f;
      destGeometry[i].ny =  1.0f;
      destGeometry[i].nz =  0.0f;
    }

    // Return an empty area of the destination geometry pointer
    return destGeometry + vertices;
}



//------------------------------------------------------------------------------------------------
// Name:  SetTerrainTexcoords
// Desc:  Sets the texture coordinates for a given direction.  
//------------------------------------------------------------------------------------------------
void SetTerrainTexcoords(GeometryVertex* terrainGeometry, unsigned int texcoordDirection)
{
    // The list of texture coordinates
    const float texcoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    // Different orders of assignment
    const int assignmentOrders[] = {
        0, 1, 2, 3, // North
        1, 3, 0, 2, // East
        3, 2, 1, 0, // South
        2, 0, 3, 1, // West
    };

    // Generate the order of assignment based on texcoord direction
    const int* vertexAssignment = &assignmentOrders[texcoordDirection*4];

    // Copy the texture coordinates
    for (unsigned int texcoord = 0; texcoord < 4; ++texcoord)
    {
        // Assign the texture coordinate
        terrainGeometry[vertexAssignment[texcoord]].u = texcoords[texcoord * 2 + 0];
        terrainGeometry[vertexAssignment[texcoord]].v = texcoords[texcoord * 2 + 1];
    }
}


WORD* PrintSquareIndices(WORD* destIndices, int squareOffset, int squareCopies)
{
    // Repeat for all of the copies
    for (int i = 0; i < squareCopies; ++i)
    {
        // Calculate the offset for these indices
        WORD offset = (squareOffset + i) * 4;

        // Copy the indices
        destIndices[0] = offset + 0;
        destIndices[1] = offset + 1;
        destIndices[2] = offset + 2;
        destIndices[3] = offset + 2;
        destIndices[4] = offset + 1;
        destIndices[5] = offset + 3;

        // Increment the pointer
        destIndices += 6;
    }

    // Return the next available location
    return destIndices;
}

DWORD* PrintAttributes(DWORD* destAttributes, DWORD attribute, int squareCopies)
{
    for (int i = 0; i < squareCopies * 2; ++i)
        destAttributes[i] = attribute;
    return destAttributes + squareCopies * 2;
}



//------------------------------------------------------------------------------------------------
// Name:  CreateTerrainMesh
// Desc:  
//------------------------------------------------------------------------------------------------
bool CreateTerrainMesh(LPDIRECT3DDEVICE9 d3dDevice, LPD3DXMESH* terrainMesh)
{
    // Calculate the number of faces and vertices required
    const unsigned int faces = (TMS_COUNT + 3) * 2;
    const unsigned int vertices = (TMS_COUNT + 3) * 4;

    // The internal terrain mesh
    LPD3DXMESH internalTerrainMesh;

    // Create the mesh
    HRESULT hr = D3DXCreateMeshFVF(faces, vertices, D3DXMESH_MANAGED, D3DFVF_GEOMETRYVERTEX, d3dDevice, &internalTerrainMesh);
    if (APP_ERROR(FAILED(hr))("Unable to create the terrain mesh"))
        return false;

    // Lock the mesh buffers
    GeometryVertex* lockedVertices = 0;
    WORD* lockedIndices = 0;
    DWORD* lockedAttributes = 0;
    if (APP_ERROR(FAILED(internalTerrainMesh->LockAttributeBuffer(0, &lockedAttributes)) ||
                   FAILED(internalTerrainMesh->LockVertexBuffer(0, (VOID**)&lockedVertices)) ||
                   FAILED(internalTerrainMesh->LockIndexBuffer(0, (VOID**)&lockedIndices)))("Unable to lock terrain mesh"))
    {
        // Deallocate the mesh
        SAFE_RELEASE(internalTerrainMesh);

        // Exit the method
        return false;
    }

    // Vertices that will be rotated 4x about Y at
    // 90-degree intervals to produce the wall mesh
    GeometryVertex wallSide[] = {
        { -0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f, -1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { -0.5f, -1.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // The flat square with unit size
    GeometryVertex flat[] = {
        { -0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // Geometry template for the high-corner meshes,
    // with the high corner in the north-west
    GeometryVertex highCorner[] = {
        { -0.5f, +1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // Geometry template for the low-corner meshes,
    // with the low corner in the north-west
    GeometryVertex lowCorner[] = {
        { -0.5f, -1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // Geometry template for the high-side meshes,
    // with the high side to the north
    GeometryVertex highSide[] = {
        { -0.5f, +1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f, +1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // Geometry template for the low-corner meshes,
    // with the low corner in the north-west
    GeometryVertex raisedLowCorner[] = {
        { -0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f, +1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f, +1.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f, +1.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // Geometry template for the low-side meshes,
    // with the low side to the north
    GeometryVertex lowSide[] = {
        { -0.5f, -1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { +0.5f, -1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { +0.5f,  0.0f, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
    };

    // The subset that is currently being written
    unsigned int currentSubset = TMS_WALL_SIDES;

    // Copy the wall first--it does not have any texture rotations
    lockedVertices = CopyYRotatedGeometry(lockedVertices, wallSide, 4, 0.0f * D3DX_PI / 2.0f);
    lockedVertices = CopyYRotatedGeometry(lockedVertices, wallSide, 4, 1.0f * D3DX_PI / 2.0f);
    lockedVertices = CopyYRotatedGeometry(lockedVertices, wallSide, 4, 2.0f * D3DX_PI / 2.0f);
    lockedVertices = CopyYRotatedGeometry(lockedVertices, wallSide, 4, 3.0f * D3DX_PI / 2.0f);

    // Copy four sets of squares
    lockedIndices = PrintSquareIndices(lockedIndices, 0, 4);
    lockedAttributes = PrintAttributes(lockedAttributes, currentSubset, 4);

    // Move to the next subset
    currentSubset++;

    // Write the flat mesh first, since it only has texture rotations
    for (unsigned int textureDirection = 0; textureDirection < 4; ++textureDirection, ++currentSubset)
    {
        SetTerrainTexcoords(flat, textureDirection);
        lockedVertices = CopyYRotatedGeometry(lockedVertices, flat, 4, 0.0f);
        lockedIndices = PrintSquareIndices(lockedIndices, 4 + currentSubset - 1, 1);
        lockedAttributes = PrintAttributes(lockedAttributes, currentSubset, 1);
    }

    // Repeat for all combinations of texture direction and rotation for the remaining types
    for (int type = 0; type < 5; ++type)
    {
        GeometryVertex* sourceGeometry;
        switch(type)
        {
            case 0: sourceGeometry = highCorner; break;
            case 1: sourceGeometry = lowCorner;  break;
            case 2: sourceGeometry = highSide;   break;
            case 3: sourceGeometry = raisedLowCorner; break;
            case 4: sourceGeometry = lowSide;    break;
        }

        // Repeat for all rotations
        for (int rotation = 0; rotation < 4; ++rotation)
        {
            // Calculate the rotation angle
            float rotationAngle = D3DX_PI / 2.0f * rotation;

            // Repeat for all texture directions
            for (unsigned int textureDirection = 0; textureDirection < 4; ++textureDirection, ++currentSubset)
            {
                // Reverse the rotation of the texture by the rotation of the element so that we get
                // consistent texture directions (i.e. north is texture-up on all tiles)
                SetTerrainTexcoords(sourceGeometry, (textureDirection - rotation + 4) % 4);
                lockedVertices = CopyYRotatedGeometry(lockedVertices, sourceGeometry, 4, rotationAngle);
                lockedIndices = PrintSquareIndices(lockedIndices, 4 + currentSubset - 1, 1);
                lockedAttributes = PrintAttributes(lockedAttributes, currentSubset, 1);
            }
        }
    }

    // Unlock the buffers
    internalTerrainMesh->UnlockVertexBuffer();
    internalTerrainMesh->UnlockIndexBuffer();
    internalTerrainMesh->UnlockAttributeBuffer();

    // Normalize
    //CONFIRM(SUCCEEDED(D3DXComputeNormals(internalTerrainMesh, NULL)));

    // Assign the output mesh
    *terrainMesh = internalTerrainMesh;

    // Success
    return true;
}
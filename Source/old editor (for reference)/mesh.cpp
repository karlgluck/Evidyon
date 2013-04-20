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



//------------------------------------------------------------------------------------------------
// Name:  Mesh
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
Mesh::Mesh()
{
    member("Subset Remap", &mySubsetRemap);
    member("Rotation", &myRotation);
    member("Scaling", &myScaling);
    member("Translation", &myTranslation);

    // Initialize the scaling with unity
    myScaling.getX()->setValue(1.0f);
    myScaling.getY()->setValue(1.0f);
    myScaling.getZ()->setValue(1.0f);
}



//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Loads a preview of this mesh into the provided structure
//------------------------------------------------------------------------------------------------
bool Mesh::loadPreview(LPDIRECT3DDEVICE9 d3dDevice, Preview* preview)
{
    // Check parameters
    if (APP_ERROR(!d3dDevice || !preview)("Invalid parameter to Mesh::loadPreview"))
        return false;
    
    // Set the source mesh and basic info
    preview->mesh = this;
    preview->d3dxMesh = NULL;

    // Temporary geometry storage
    SubsetGeometry geometry;

    // Load the geometry
    if (APP_WARNING(!obtainGeometry(d3dDevice, &geometry))
            ("Couldn't load geometry data for mesh %s", getName().c_str()))
        return false;

    // Create the D3DX mesh from the source geometry
    DWORD totalVertices = 0;
    DWORD totalIndices = 0;

    // Sum all of the geometry data
    for (SubsetGeometry::iterator s = geometry.begin(); s != geometry.end(); ++s)
    {
        totalVertices += s->second->NumVertices;
        totalIndices  += s->second->NumIndices;
    }

    // Create a new mesh
    if (APP_ERROR(FAILED(D3DXCreateMeshFVF(totalIndices / 3, totalVertices, D3DXMESH_MANAGED|D3DXMESH_32BIT, D3DFVF_GEOMETRYVERTEX, d3dDevice, &preview->d3dxMesh)))
        ("Couldn't create the preview mesh for %s", getName().c_str()))
        return false;

    // Vertex/index pointers
    GeometryVertex* pVertices;
    DWORD* pIndices;
    DWORD* pAttrib;

    // Save the vertices, indices and attributes
    if (SUCCEEDED(preview->d3dxMesh->LockVertexBuffer(0, (VOID**)&pVertices)) &&
        SUCCEEDED(preview->d3dxMesh->LockIndexBuffer(0, (VOID**)&pIndices)) &&
        SUCCEEDED(preview->d3dxMesh->LockAttributeBuffer(0, &pAttrib)))
    {
        // Sums the number of vertices/indices/attributes
        DWORD vertices = 0;
        DWORD indices = 0;
        DWORD attributes = 0;

        // Write data from each subset
        for (SubsetGeometry::iterator s = geometry.begin(); s != geometry.end(); ++s)
        {
            DWORD subsetVertices = s->second->NumVertices;
            DWORD subsetIndices = s->second->NumIndices;
            DWORD subsetPolygons = subsetIndices / 3;

            // Copy the vertices
            memcpy(&pVertices[vertices], s->second->pVertices, subsetVertices * sizeof(GeometryVertex));

            // Copy the indices (we have to offset the value though)
            for (DWORD k = 0; k < subsetIndices; ++k)
                pIndices[indices + k] = s->second->pIndices[k] + vertices;

            // Set the attribute elements for the indices
            for (DWORD k = 0; k < subsetPolygons; ++k)
                pAttrib[attributes + k] = s->first;

            // Add to the total values
            vertices += subsetVertices;
            indices += subsetIndices;
            attributes += subsetPolygons;
        }

        // Unlock the buffers
        preview->d3dxMesh->UnlockVertexBuffer();
        preview->d3dxMesh->UnlockIndexBuffer();
        preview->d3dxMesh->UnlockAttributeBuffer();
    }

    // Successfully loaded the preview
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  obtainGeometry
// Desc:  Loads the geometry specified by this mesh class in its final form
//------------------------------------------------------------------------------------------------
bool Mesh::obtainGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const
{
    // Generate the geometry for this mesh
    if (!obtainSourceGeometry(pd3dDevice, subsetGeometry)) return false;
/*
    // Get the end of the subset iteration (so that we don't make a lot of calls to the method)
    SubsetGeometry::iterator end = subsetGeometry->end();

    // The container into which we'll be putting the remapped geometry
    SubsetGeometryCollection geometryCollection;

    // Transform the geometry of the mesh
    for (SubsetGeometry::iterator i = subsetGeometry->begin(); i != end; ++i)
    {
        // Look up this entry
        dc::dcMap<dcIndex32,dcIndex32>::Element* remappedSubset = mySubsetRemap.find((dcIndex32::BasicType)i->first);

        // Get the entry to insert
        DWORD insertSubset = remappedSubset ? (DWORD)remappedSubset->getValue()->getValue() : i->first;

        // Put this into the remap
        geometryCollection.insert(SubsetGeometryCollection::value_type(insertSubset, i->second));
    }

    // Erase everything in the subset geometry
    subsetGeometry->clear();

    // Collect this geometry into another geometry table
    SubsetGeometry collectedGeometry;
    if (FAILED(CollectGeometry(&geometryCollection, &collectedGeometry)))
    {
        // Free the memory we allocated
        DeallocateGeometry(&geometryCollection);

        // Exit with failure
        return false;
    }

    // Clear the uncollected geometry now that we've reduced it
    DeallocateGeometry(&geometryCollection);

    // Erase (but do not deallocate) the set of original geometry
    subsetGeometry->clear();

    // Copy this new geometry into the buffer
    subsetGeometry->insert(collectedGeometry.begin(), collectedGeometry.end());
*/
    // Remove any subsets without geometry
    for (SubsetGeometry::iterator i = subsetGeometry->begin(); i != subsetGeometry->end(); )
    {
        if (i->second->NumVertices == 0 || i->second->NumIndices == 0)
        {
            SubsetGeometry::iterator j = i;
            ++j;
            subsetGeometry->erase(i);
            i = j;
        }
        else
            ++i;
    }

    // Build a transformation matrix
    D3DXMATRIX world;
    {
        D3DXMATRIXA16 scale, rotate, translate;
        D3DXMatrixScaling(&scale, myScaling.getX()->getValue(), myScaling.getY()->getValue(), myScaling.getZ()->getValue());
        D3DXMatrixRotationYawPitchRoll(&rotate,
                                       D3DXToRadian(myRotation.getY()->getValue()),
                                       D3DXToRadian(myRotation.getX()->getValue()),
                                       D3DXToRadian(myRotation.getZ()->getValue()));
        D3DXMatrixTranslation(&translate, myTranslation.getX()->getValue(), myTranslation.getY()->getValue(), myTranslation.getZ()->getValue());

        // Combine all of the matrices
        D3DXMatrixMultiply(&world, &scale, &rotate);
        D3DXMatrixMultiply(&world, &world, &translate);
    }

    // Finally, transform the geometry based on the parameters in this class
    for (SubsetGeometry::iterator i = subsetGeometry->begin(); i != subsetGeometry->end(); ++i)
    {
        D3DXVec3TransformCoordArray((D3DXVECTOR3*)(i->second->pVertices), sizeof(GeometryVertex),
                                    (D3DXVECTOR3*)(i->second->pVertices), sizeof(GeometryVertex),
                                    &world, i->second->NumVertices);
    }
/*
    // These values keep track of scaling information
    float unityScalingFactor;
    float max[3] = { FLT_MIN, FLT_MIN, FLT_MIN },
          min[3] = { FLT_MAX, FLT_MAX, FLT_MAX }; // x, y, z, coordinates

    // Look through the geometry
    for (SubsetGeometry::iterator i = subsetGeometry->begin(); i != subsetGeometry->end(); ++i)
    {
        // Go through each of the vertices
        for (DWORD vtx = 0; vtx < i->second->NumVertices; ++vtx)
        {
            // Get the vertex reference
            GeometryVertex* pVertex = &i->second->pVertices[vtx];

            // Compare the vertex
            if (pVertex->x < min[0]) min[0] = pVertex->x;
            if (pVertex->x > max[0]) max[0] = pVertex->x;
            if (pVertex->y < min[1]) min[1] = pVertex->y;
            if (pVertex->y > max[1]) max[1] = pVertex->y;
            if (pVertex->z < min[2]) min[2] = pVertex->z;
            if (pVertex->z > max[2]) max[2] = pVertex->z;
        }
    }
    // Get the difference in coordinates
    float dx = max[0] - min[0],
          dy = max[1] - min[1],
          dz = max[2] - min[2];

    // Find the largest difference
    float maxDXY  = dx > dy ? dx : dy,
          maxDXZ  = dx > dz ? dx : dz;
    float maxDXYZ = maxDXY > maxDXZ ? maxDXY : maxDXZ;

    // Scale the largest difference to unity
    unityScalingFactor = 1.0f / maxDXYZ;

    char scalingFactorText[512];
    sprintf_s(scalingFactorText, "Mesh %s:  %f\n", getName().c_str(), unityScalingFactor);
    OutputDebugString(scalingFactorText);*/

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  getUnityScalingFactor
// Desc:  Determines the scaling factor required to make the largest dimension 1.0
//------------------------------------------------------------------------------------------------
bool Mesh::getUnityScalingFactor(LPDIRECT3DDEVICE9 pd3dDevice, float* scalingFactor) const
{
    // Make sure the parameters are valid
    if (!pd3dDevice || !scalingFactor) return false;

    // Load the geometry
    SubsetGeometry subsetGeometry;

    // Generate the geometry for this mesh
    if (!obtainSourceGeometry(pd3dDevice, &subsetGeometry)) return false;

    // These values keep track of scaling information
    float unityScalingFactor;
    float max[3] = { FLT_MIN, FLT_MIN, FLT_MIN },
          min[3] = { FLT_MAX, FLT_MAX, FLT_MAX }; // x, y, z, coordinates

    // Look through the geometry
    for (SubsetGeometry::iterator i = subsetGeometry.begin(); i != subsetGeometry.end(); ++i)
    {
        // Go through each of the vertices
        for (DWORD vtx = 0; vtx < i->second->NumVertices; ++vtx)
        {
            // Get the vertex reference
            GeometryVertex* pVertex = &i->second->pVertices[vtx];

            // Compare the vertex
            if (pVertex->x < min[0]) min[0] = pVertex->x;
            if (pVertex->x > max[0]) max[0] = pVertex->x;
            if (pVertex->y < min[1]) min[1] = pVertex->y;
            if (pVertex->y > max[1]) max[1] = pVertex->y;
            if (pVertex->z < min[2]) min[2] = pVertex->z;
            if (pVertex->z > max[2]) max[2] = pVertex->z;
        }
    }

    // Get rid of the geometry
    DeallocateGeometry(&subsetGeometry);

    // Get the difference in coordinates
    float dx = max[0] - min[0],
          dy = max[1] - min[1],
          dz = max[2] - min[2];

    // Find the largest difference
    float maxDXY  = dx > dy ? dx : dy,
          maxDXZ  = dx > dz ? dx : dz;
    float maxDXYZ = maxDXY > maxDXZ ? maxDXY : maxDXZ;

    // Scale the largest difference to unity
    unityScalingFactor = 1.0f / maxDXYZ;

    // Return the factor
    *scalingFactor = unityScalingFactor;
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  getRotation
// Desc:  Obtains an editable reference to the internal rotation structure
//------------------------------------------------------------------------------------------------
dcxVector3<dcFloat>* Mesh::getRotation() const
{
    return (dcxVector3<dcFloat>*)&myRotation;
}


//------------------------------------------------------------------------------------------------
// Name:  getScaling
// Desc:  Obtains an editable reference to the internal scaling structure
//------------------------------------------------------------------------------------------------
dcxVector3<dcFloat>* Mesh::getScaling() const
{
    return (dcxVector3<dcFloat>*)&myScaling;
}



//------------------------------------------------------------------------------------------------
// Name:  getTranslation
// Desc:  Obtains an editable reference to the internal translation structure
//------------------------------------------------------------------------------------------------
dcxVector3<dcFloat>* Mesh::getTranslation() const
{
    return (dcxVector3<dcFloat>*)&myTranslation;
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string Mesh::staticTypeName()
{
    return "Mesh";
}


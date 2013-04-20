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
#include "allgameresources.h"
#include "resourcepack.h"
#include "packmesh.h"


// Resolve the implementation linker
dcClass<PackMesh>::Implements<Mesh> PackMesh::implementsMesh;


//------------------------------------------------------------------------------------------------
// Name:  PackMesh
// Desc:  Initializes this resource
//------------------------------------------------------------------------------------------------
PackMesh::PackMesh()
{
    // Link members
    member("Resource Pack", &myResourcePack);
    member("Resource Index", &myResourceIndex);

    // Reset internal values
    myResourceIndex.setValue(-1);
}



//------------------------------------------------------------------------------------------------
// Name:  obtainSourceGeometry
// Desc:  Loads the geometry held by this class into the destination set
//------------------------------------------------------------------------------------------------
bool PackMesh::obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const
{
    // Fail without a device or if something is wrong with the output pointer
    if (APP_ERROR(!pd3dDevice || !subsetGeometry)("Invalid parameter to obtainSourceGeometry"))
        return false;

    // Get the pack from which to load this mesh
    dc::dcTable<ResourcePack>::Element* packElement = ((PackMesh*)this)->myResourcePack.getReferencedResource();
    if (!packElement) return false;
    ResourcePack* pack = packElement->getImplementation();
    if (!pack) return false;

    // Obtain the mesh in the pack
    dcBuffer buffer;
    if (APP_ERROR(!pack->openResource(myResourceIndex.getValue(), &buffer))
        ("Failed to open image %i in pack file", myResourceIndex))
        return false;

    // Set up a buffer reader to scan data from the returned information
    dcBuffer::Reader bufferReader(&buffer);

    // Get the number of subsets in this mesh
    size_t subsets;
    if (!bufferReader.read(&subsets, sizeof(subsets))) return false;

    // Whether or not loading failed
    bool failed = false;

    // Load each of the subsets
    for (size_t s = 0; s < subsets; ++s)
    {
        // Stores subset information while it is being loaded
        Geometry* geometry = NULL;
        SubsetIndex subset;
        DWORD vertices, indices;

        // Read the subset index information and the geometry data from the file
        if (APP_ERROR(!bufferReader.read(&subset, sizeof(subset)) ||
                       !bufferReader.read(&vertices, sizeof(vertices)) ||
                       !bufferReader.read(&indices, sizeof(indices)))
           ("Error while reading mesh data"))
        {
            // We were unable to load this mesh
            failed = true;

            // Exit the subset loop
            break;
        }

        // Create geometry for this mesh subset's data
       if (APP_FATAL(FAILED(AllocateGeometry(vertices, indices, &geometry)))
            ("Out of memory while allocating subset %i of %s (%i vertices/%i indices)",
              subset, getPathString().c_str(), vertices, indices))
       {
           // Couldn't load the mesh
           failed = true;
           
           // Exit this loop
           break;
       }

        // Load the geometry buffers
       if (APP_ERROR(!bufferReader.read(geometry->pVertices, sizeof(GeometryVertex) * vertices) ||
                      !bufferReader.read(geometry->pIndices, sizeof(GeometryIndex) * indices))
          ("Couldn't load geometry vertex/index data for subset %i of %s",
            subset, getPathString().c_str()))
       {
           // Unable to load
           failed = true;

           // Quit the loading process
           break;
       }

       // Add the data we just loaded as a mesh subset
       subsetGeometry->insert(SubsetGeometry::value_type(subset, geometry));
    }

    // If we failed, get rid of anything that managed to be loaded
    if (failed)
        DeallocateGeometry(subsetGeometry);

    // Return whether or not this operation succeeded
    return !failed;
}




//------------------------------------------------------------------------------------------------
// Name:  setResourceIndex
// Desc:  Gets the string name of this resource type
//------------------------------------------------------------------------------------------------
void PackMesh::setResourceIndex(int index)
{
    myResourceIndex.setValue(index);
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the string name of this resource type
//------------------------------------------------------------------------------------------------
std::string PackMesh::staticTypeName()
{
    return "Pack Mesh";
}

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
#include "unseenskinnedmeshresource.h"
#include <dc/filestream>


//------------------------------------------------------------------------------------------------
// Name: UnseenSkinnedMeshResource
// Desc: Initializes this resource
//------------------------------------------------------------------------------------------------
UnseenSkinnedMeshResource::UnseenSkinnedMeshResource()
{
    member("Source File", &mySourceFile);
    member("Reverse Winding Order", &myReverseWindingOrderFlag);

    // all of the converted meshes need their winding order reversed.  this is a bug in the
    // compiler that still needs to be fixed (it's not a huge deal tho)
    myReverseWindingOrderFlag.setValue(true);
}



//------------------------------------------------------------------------------------------------
// Name: loadSkinnedMesh
// Desc: Loads the referenced mesh from the source file
//------------------------------------------------------------------------------------------------
bool UnseenSkinnedMeshResource::loadSkinnedMesh(UnseenSkinnedMesh* skinnedMesh) const
{
    // Open the source stream
    dc::dcFileStream fs;
    if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source skinned mesh file %s", mySourceFile.getValue().c_str()))
        return false;

    // Load the mesh
    bool failed = APP_ERROR(!loadUnseenSkinnedMesh(&fs, skinnedMesh))("Invalid Unseen skinned mesh file %s", mySourceFile.getValue().c_str());

    // Close the stream
    fs.close();

    // Reverse the winding order in the mesh, if necessary
    if (!failed && myReverseWindingOrderFlag.getValue())
    {
        for (DWORD textureGroupIndex = 0; textureGroupIndex < skinnedMesh->info.numberOfTextureGroups; ++textureGroupIndex)
        {
            UnseenSkinnedMesh::TextureGroup* textureGroup = &skinnedMesh->textureGroups[textureGroupIndex];

            for (DWORD influenceGroupIndex = 0; influenceGroupIndex < textureGroup->info.numberOfVertexInfluenceGroups; ++influenceGroupIndex)
            {
                UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influenceGroup = &textureGroup->vertexInfluenceGroups[influenceGroupIndex];

                WORD* indices = (WORD*)influenceGroup->indices;
                for (DWORD triangleIndex = 0; triangleIndex < influenceGroup->info.numberOfTriangles; ++triangleIndex)
                {
                    WORD temp = indices[0];
                    indices[0] = indices[1];
                    indices[1] = temp;
                    indices += 3;
                }
            }
        }
    }

    // Return the result
    return !failed;
}



//------------------------------------------------------------------------------------------------
// Name: getFileName
// Desc: Gets the source file name for this skinned mesh
//       @return Pointer to the internal file name object
//------------------------------------------------------------------------------------------------
dcFileName* UnseenSkinnedMeshResource::getFileName()
{
    return &mySourceFile;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string UnseenSkinnedMeshResource::staticTypeName()
{
    return "UnseenSkinnedMeshResource";
}


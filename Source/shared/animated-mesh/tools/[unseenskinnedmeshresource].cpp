//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#include "unseenskinnedmeshresource.h"
#include <dc/filestream>
#include "common/safemacros.h"
#include <assert.h>
#include "shared/animated-mesh/animatedmeshutils.h"
#include "shared/animated-mesh/animatedmeshdata.h"



//-----------------------------------------------------------------------------
// Name: UnseenSkinnedMeshResource
// Desc: Initializes this resource
//-----------------------------------------------------------------------------
UnseenSkinnedMeshResource::UnseenSkinnedMeshResource() {
  member("Source File", &mySourceFile);
  member("Reverse Winding Order", &myReverseWindingOrderFlag);

  // all of the converted meshes need their winding order reversed.  this is a bug in the
  // compiler that still needs to be fixed (it's not a huge deal tho)
  myReverseWindingOrderFlag.setValue(true);

  loaded_mesh_ = NULL;
}



//-----------------------------------------------------------------------------
// Name: loadSkinnedMesh
// Desc: Loads the referenced mesh from the source file
//-----------------------------------------------------------------------------
bool UnseenSkinnedMeshResource::loadSkinnedMesh(bool force_reload,
                                                UnseenSkinnedMesh** skinnedMesh) {
  if (!force_reload && loaded_mesh_) {
    *skinnedMesh = loaded_mesh_;
    return true;
  } else {
    unloadSkinnedMesh();
    loaded_mesh_ = new UnseenSkinnedMesh;
    memset(loaded_mesh_, 0, sizeof(UnseenSkinnedMesh));
    *skinnedMesh = loaded_mesh_;
  }
  
  // Open the source stream
  dc::dcFileStream fs;
  if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source skinned mesh file %s", mySourceFile.getValue().c_str()))
      return false;

  // Load the mesh
  bool failed = APP_ERROR(!loadUnseenSkinnedMesh(&fs, loaded_mesh_))("Invalid Unseen skinned mesh file %s", mySourceFile.getValue().c_str());

  // Close the stream
  fs.close();

  // Reverse the winding order in the mesh, if necessary
  if (!failed && myReverseWindingOrderFlag.getValue()) {
    for (DWORD textureGroupIndex = 0; textureGroupIndex < loaded_mesh_->info.numberOfTextureGroups; ++textureGroupIndex) {
      UnseenSkinnedMesh::TextureGroup* textureGroup = &loaded_mesh_->textureGroups[textureGroupIndex];

      for (DWORD influence_groupIndex = 0; influence_groupIndex < textureGroup->info.numberOfVertexInfluenceGroups; ++influence_groupIndex) {
        UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influence_group = &textureGroup->vertexInfluenceGroups[influence_groupIndex];

        WORD* indices = (WORD*)influence_group->indices;
        for (DWORD triangleIndex = 0; triangleIndex < influence_group->info.numberOfTriangles; ++triangleIndex) {
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



//----[  unloadSkinnedMesh  ]--------------------------------------------------
void UnseenSkinnedMeshResource::unloadSkinnedMesh() {
  if (loaded_mesh_) {
    deallocateUnseenSkinnedMesh(loaded_mesh_);
    SAFE_DELETE(loaded_mesh_);
  }
}





//----[  loadAnimatedMeshData  ]-----------------------------------------------
bool UnseenSkinnedMeshResource::loadAnimatedMeshData(
    bool force_reload,
    Evidyon::AnimatedMesh::AnimatedMeshData* animated_mesh_data) {
  UnseenSkinnedMesh* unseen_mesh;
  if (!loadSkinnedMesh(force_reload, &unseen_mesh)) return false;
  if (!AnimatedMeshUtil_InterpretUnseenSkinnedMesh(
        unseen_mesh,
        animated_mesh_data)) {
    return false;
  }

  return true;
}


//----[  getFileName  ]--------------------------------------------------------
dc::dcFileName* UnseenSkinnedMeshResource::getFileName() {
    return &mySourceFile;
}






//----[  numberOfTextureGroups  ]----------------------------------------------
int UnseenSkinnedMeshResource::numberOfTextureGroups() {
  UnseenSkinnedMesh* mesh;
  if (loadSkinnedMesh(false, &mesh)) {
    return mesh->info.numberOfTextureGroups;
  } else {
    return 0;
  }
}




//----[  staticTypeName  ]-----------------------------------------------------
std::string UnseenSkinnedMeshResource::staticTypeName() {
    return "UnseenSkinnedMeshResource";
}

